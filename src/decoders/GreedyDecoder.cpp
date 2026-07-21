#include "decoders/GreedyDecoder.hpp"
#include <algorithm>

namespace routing
{
  types::DaySolution GreedyDecoder::decode(
      const std::vector<size_t> &sequence,
      const types::DayContext &ctx) const
  {
    std::size_t seq_size = sequence.size();
    types::DaySolution result(ctx.num_workers, ctx.num_clients, ctx.num_products);

    // Last worker who was able to successfully visit a client
    int active_worker = 0;

    // Auxiliary vector to save the current total time of the route, excluding the return time
    std::vector<float> workers_partial_time(ctx.num_workers, 0.0f);

    // 1. ASSIGN CLIENTS TO THE ROUTES
    for (std::size_t i = 0; i < seq_size; i++)
    {
      int client_id = sequence[i];
      bool end_visit = false;

      // For each product
      for (int j = 0; j < ctx.num_products; j++)
      {
        int amount_to_restock = ctx.demand[client_id][j];

        // Continue to the next product if a restock is not possible
        if (amount_to_restock <= 0)
        {
          continue;
        }

        float service_time = amount_to_restock * ctx.restock_time[j];

        // Check through the workers if anyone can restock the current product for the current client
        while (active_worker < ctx.num_workers)
        {
          float travel_time;
          float service_starting_time;

          // If this is the worker's first visit, they have to travel from their starting node
          if (result.routes[active_worker].empty())
          {
            travel_time = ctx.time_matrix[active_worker][ctx.num_workers + client_id];
            service_starting_time = travel_time;
          }
          else
          {
            int previous_client = result.routes[active_worker].back().node_id;

            // If the previous restock service was provided to another client, the worker has to travel from that node
            if (previous_client != client_id)
            {
              travel_time = ctx.time_matrix[ctx.num_workers + previous_client][ctx.num_workers + client_id];
              service_starting_time = workers_partial_time[active_worker] + travel_time;
            }
            // If the previous restock service was provided to this client, there's no extra travel time
            else
            {
              travel_time = 0.0f;
              service_starting_time = workers_partial_time[active_worker];
            }
          }

          float wait_time = 0.0f;

          // If the worker arrived early, they have to wait until the client's starting hour
          if (service_starting_time < ctx.starting_hour[client_id])
          {
            wait_time = ctx.starting_hour[client_id] - service_starting_time;
            service_starting_time = ctx.starting_hour[client_id];
          }

          float service_ending_time = service_starting_time + service_time;
          float required_time_with_return = service_ending_time + ctx.time_matrix[ctx.num_workers + client_id][active_worker];

          // If the worker CAN'T complete this restock service and return to their starting node within the time limit
          if (required_time_with_return > ctx.L)
          {
            // Try to reduce the restocked amount based on the available time considering the service starting time and the required return travel time
            float available_time = ctx.L - service_starting_time - ctx.time_matrix[ctx.num_workers + client_id][active_worker];
            int possible_restock_by_time = available_time / ctx.restock_time[j];

            if (possible_restock_by_time > 0)
            {
              amount_to_restock = possible_restock_by_time;

              service_ending_time = service_starting_time + amount_to_restock * ctx.restock_time[j];
              required_time_with_return = service_ending_time + ctx.time_matrix[ctx.num_workers + client_id][active_worker];
            }
            else
            {
              // Check if the current worker has already registered a visit to this client
              bool already_visiting = (!result.routes[active_worker].empty() &&
                                       result.routes[active_worker].back().node_id == client_id);

              if (already_visiting)
              {
                // We already started servicing them; we must abandon the rest to avoid split deliveries
                end_visit = true;
                active_worker++;
                break;
              }
              else
              {
                // The worker is full, but hasn't touched this client.
                // Let the next worker try to service this exact same product.
                active_worker++;
                continue;
              }
            }
          }

          // If the service ends after the finishing hour, count it as a missed schedule
          if (service_ending_time > (ctx.finishing_hour[client_id] + 0.1f))
          {
            result.missed_schedule[client_id] = true;
          }

          result.workers_total_time[active_worker] = required_time_with_return;
          workers_partial_time[active_worker] = service_ending_time;

          // If the previous restock service was provided to this client
          if (!result.routes[active_worker].empty() && result.routes[active_worker].back().node_id == client_id)
          {
            result.routes[active_worker].back().restocked_product[j] = amount_to_restock;
            result.routes[active_worker].back().visit_ending_time = service_ending_time;
          }
          // If this is the worker's first visit or the previous restock service was provided to another client
          else
          {
            types::Visit new_visit(client_id, ctx.num_products);
            std::fill(new_visit.restocked_product.begin(), new_visit.restocked_product.end(), 0);

            new_visit.restocked_product[j] = amount_to_restock;
            new_visit.visit_start_time = service_starting_time;
            new_visit.visit_ending_time = service_ending_time;
            new_visit.wait_time = wait_time;

            result.routes[active_worker].push_back(new_visit);
          }

          // Break the while(active_worker < ctx.num_workers) cycle, as this product was already restocked
          break;
        }

        if (end_visit)
        {
          break;
        }
      }
    }

    // 2. RESTOCK MORE PRODUCT TO THE CLIENTS THAT ARE ALREADY CONSIDERED IN THE ROUTE
    for (int i = 0; i < ctx.num_workers; i++)
    {
      // For each client in the worker's route
      for (int j = 0; j < result.routes[i].size(); j++)
      {
        int client_id = result.routes[i][j].node_id;

        // Update the wait and starting time from the second visited client onward
        if (j > 0)
        {
          int prev_client_id = result.routes[i][j - 1].node_id;
          float new_arriving_time = result.routes[i][j - 1].visit_ending_time + ctx.time_matrix[ctx.num_workers + prev_client_id][ctx.num_workers + client_id];

          float old_start_time = result.routes[i][j].visit_start_time;

          // If the worker arrives on time or after the starting hour, there's no wait time
          if (new_arriving_time >= ctx.starting_hour[client_id])
          {
            result.workers_total_time[i] -= result.routes[i][j].wait_time;
            result.routes[i][j].wait_time = 0;
            result.routes[i][j].visit_start_time = new_arriving_time;
          }
          // Otherwise, there could still be a reduction on the wait time
          else
          {
            float new_wait_time = ctx.starting_hour[client_id] - new_arriving_time;
            result.workers_total_time[i] -= (result.routes[i][j].wait_time - new_wait_time);
            result.routes[i][j].wait_time = new_wait_time;
            result.routes[i][j].visit_start_time = new_arriving_time + new_wait_time;
          }

          // Check if the time shift caused a missed schedule
          float time_shift = result.routes[i][j].visit_start_time - old_start_time;
          result.routes[i][j].visit_ending_time += time_shift;

          if (result.routes[i][j].visit_ending_time > (ctx.finishing_hour[client_id] + 0.1f))
          {
            result.missed_schedule[client_id] = true;
          }
        }

        float available_time = ctx.L - result.workers_total_time[i];

        if (available_time <= 0.0f)
        {
          break;
        }

        // For each product
        for (int k = 0; k < ctx.num_products; k++)
        {
          int current_inventory_with_restock = ctx.start_inventory[client_id][k] - ctx.demand[client_id][k] + result.routes[i][j].restocked_product[k];
          int possible_additional_restock_by_capacity = ctx.max_inventory[client_id][k] - current_inventory_with_restock;

          int possible_additional_restock_by_time = available_time / ctx.restock_time[k];

          int additional_restock = std::min(possible_additional_restock_by_capacity, possible_additional_restock_by_time);

          if (additional_restock > 0)
          {
            result.routes[i][j].restocked_product[k] += additional_restock;

            float additional_time_spent = additional_restock * ctx.restock_time[k];

            result.workers_total_time[i] += additional_time_spent;
            result.routes[i][j].visit_ending_time += additional_time_spent;
            available_time -= additional_time_spent;

            if (result.routes[i][j].visit_ending_time > (ctx.finishing_hour[client_id] + 0.1f))
            {
              result.missed_schedule[client_id] = true;
            }
          }

          if (available_time <= 0.0f)
          {
            break;
          }
        }

        if (available_time <= 0.0f)
        {
          break;
        }
      }
    }

    // 3. UPDATE THE INVENTORY
    for (int i = 0; i < ctx.num_workers; i++)
    {
      for (int j = 0; j < result.routes[i].size(); j++)
      {
        int client_id = result.routes[i][j].node_id;
        for (int k = 0; k < ctx.num_products; k++)
        {
          result.total_day_restocked_products += result.routes[i][j].restocked_product[k];
          result.end_of_day_inventory[client_id][k] += result.routes[i][j].restocked_product[k];
        }
      }
    }

    for (int i = 0; i < ctx.num_clients; i++)
    {
      for (int j = 0; j < ctx.num_products; j++)
      {
        result.end_of_day_inventory[i][j] += ctx.start_inventory[i][j];
        result.end_of_day_inventory[i][j] -= ctx.demand[i][j];

        // If there's a product deficit
        if (result.end_of_day_inventory[i][j] < 0)
        {
          int deficit = -result.end_of_day_inventory[i][j];
          result.total_day_missing_inventory += deficit;
          result.missing_inventory[i][j] = deficit;

          // Reset the inventory to 0
          result.end_of_day_inventory[i][j] = 0;
        }
        else
        {
          result.missing_inventory[i][j] = 0;
        }
      }
    }

    for (int i = 0; i < ctx.num_clients; i++)
    {
      if (result.missed_schedule[i])
      {
        result.total_day_missed_schedules++;
      }
    }

    // 4. CALCULATE TOTAL DAY UTILITY
    result.total_day_utility = 0.0f;

    std::vector<int> total_restocked_by_product(ctx.num_products, 0);

    for (int i = 0; i < ctx.num_workers; i++)
    {
      for (int j = 0; j < result.routes[i].size(); j++)
      {
        for (int k = 0; k < ctx.num_products; k++)
        {
          total_restocked_by_product[k] += result.routes[i][j].restocked_product[k];
        }
      }
    }

    for (int k = 0; k < ctx.num_products; k++)
    {
      result.total_day_utility += (total_restocked_by_product[k] * ctx.utility[k]);
    }

    result.total_day_utility -= (result.total_day_missing_inventory * ctx.delta);
    result.total_day_utility -= (result.total_day_missed_schedules * ctx.theta);

    return result;
  }
}
