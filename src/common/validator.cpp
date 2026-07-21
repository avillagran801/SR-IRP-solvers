#include "common/validator.hpp"

#include <vector>
#include <iostream>
#include <unordered_set>
#include <cmath>
#include <stdexcept>

namespace validator
{
  void validate_solution(std::vector<types::DaySolution> final_solution,
                         std::deque<types::DayContext> day_contexts,
                         bool verbose)
  {
    bool valid = true;

    if (verbose)
    {
      std::cout << "Validating final solution...\n\n";
    }
    for (int i = 0; i < final_solution.size(); i++)
    {
      if (verbose)
      {
        std::cout << "--- Day " << i + 1 << " ---\n";
      }

      std::vector<std::vector<types::Visit>> day_routes = final_solution[i].routes;
      types::DaySolution day_solution = final_solution[i];
      types::DayContext ctx = day_contexts[i];

      // 1. EVERY CLIENT CAN BE VISITED AT MOST ONCE
      bool client_duplicate = false;
      std::unordered_set<int> visited_clients;

      for (int worker = 0; worker < day_routes.size(); worker++)
      {
        for (int visit = 0; visit < day_routes[worker].size(); visit++)
        {
          int client_id = day_routes[worker][visit].node_id;

          if (!visited_clients.insert(client_id).second)
          {
            client_duplicate = true;
            valid = false;
            if (verbose)
            {
              std::cout << "[✗] Client " << client_id << " was already visited\n";
            }
          }
        }
      }

      if (!client_duplicate && verbose)
      {
        std::cout << "[✓] All clients were visited at most once\n";
      }

      // 2. ALL VISITS REQUIRE RESTOCKING AT LEAST 1 UNIT OF ANY PRODUCT
      bool valid_visit = true;

      for (int worker = 0; worker < day_routes.size(); worker++)
      {
        for (int visit = 0; visit < day_routes[worker].size(); visit++)
        {
          int client_id = day_routes[worker][visit].node_id;
          int total_restocked = 0;

          for (int product_id = 0; product_id < day_routes[worker][visit].restocked_product.size(); product_id++)
          {
            total_restocked += day_routes[worker][visit].restocked_product[product_id];
          }

          if (total_restocked == 0)
          {
            valid_visit = false;
            valid = false;
            if (verbose)
            {
              std::cout << "[✗] Client " << client_id << " was visited, but no product was restocked\n";
            }
          }
        }
      }

      if (valid_visit && verbose)
      {
        std::cout << "[✓] All visits restocked at least 1 unit of product\n";
      }

      // TRACKERS FOR SECTIONS 3, 4, 5, 6, AND 8
      bool capacity_exceeded = false;
      std::vector<int> total_restocked_products_by_id(ctx.num_products, 0);
      std::vector<std::vector<int>> client_total_restocked(ctx.num_clients, std::vector<int>(ctx.num_products, 0));

      // 3. THE NUMBER OF RESTOCKED UNITS OF PRODUCT CANNOT EXCEED THE MAXIMUM CAPACITY OF THE SHELVES
      for (int worker = 0; worker < day_routes.size(); worker++)
      {
        for (int visit = 0; visit < day_routes[worker].size(); visit++)
        {
          int client_id = day_routes[worker][visit].node_id;

          for (int product_id = 0; product_id < day_routes[worker][visit].restocked_product.size(); product_id++)
          {
            int maximum_restock = ctx.max_inventory[client_id][product_id] - ctx.start_inventory[client_id][product_id] + ctx.demand[client_id][product_id];
            int restocked_product = day_routes[worker][visit].restocked_product[product_id];

            if (restocked_product > maximum_restock)
            {
              capacity_exceeded = true;
              valid = false;
              if (verbose)
              {
                std::cout << "[✗] The number of units of product " << product_id << " restocked for client " << client_id << " exceeded the maximum capacity\n";
              }
            }

            // Accumulate for Sections 4, 5, 6, and 8
            client_total_restocked[client_id][product_id] += restocked_product;
            total_restocked_products_by_id[product_id] += restocked_product;
          }
        }
      }

      if (!capacity_exceeded && verbose)
      {
        std::cout << "[✓] No restocked product exceeded the maximum capacity of the clients' shelves\n";
      }

      // 4 & 5. VALIDATE FINAL INVENTORY AND TOTAL MISSING PRODUCTS FOR ALL CLIENTS
      bool correct_final_inventory = true;
      bool correct_missing_products = true;
      int expected_total_missing = 0;

      for (int client_id = 0; client_id < ctx.num_clients; client_id++)
      {
        for (int product_id = 0; product_id < ctx.num_products; product_id++)
        {
          int final_stock = ctx.start_inventory[client_id][product_id] - ctx.demand[client_id][product_id] + client_total_restocked[client_id][product_id];

          int expected_missing = 0;
          int expected_eod_inventory = final_stock;

          if (final_stock < 0)
          {
            expected_missing = -final_stock;
            expected_eod_inventory = 0;
            expected_total_missing += expected_missing;
          }

          if (expected_eod_inventory != day_solution.end_of_day_inventory[client_id][product_id])
          {
            correct_final_inventory = false;
            valid = false;
            if (verbose)
            {
              std::cout << "[✗] The final inventory of product " << product_id << " for client " << client_id << " wasn't calculated correctly\n";
            }
          }

          if (expected_missing != day_solution.missing_inventory[client_id][product_id])
          {
            correct_missing_products = false;
            valid = false;
            if (verbose)
            {
              std::cout << "[✗] The number of missing units of product " << product_id << " for client " << client_id << " wasn't calculated correctly\n";
            }
          }
        }
      }

      if (correct_missing_products && verbose)
      {
        std::cout << "[✓] The total number of missing units of products was calculated correctly\n";
      }
      if (correct_final_inventory && verbose)
      {
        std::cout << "[✓] The final inventory of all clients was calculated correctly\n";
      }

      // 6. VALIDATE TOTAL RESTOCKED PRODUCTS
      int aux_total_day_restocked_products = 0;
      bool correct_restocked_products = true;

      for (int i = 0; i < ctx.num_products; i++)
      {
        aux_total_day_restocked_products += total_restocked_products_by_id[i];
      }

      if (aux_total_day_restocked_products != day_solution.total_day_restocked_products)
      {
        correct_restocked_products = false;
        valid = false;
        if (verbose)
        {
          std::cout << "[✗] The total number of restocked units of products wasn't calculated correctly\n";
        }
      }
      else
      {
        if (verbose)
        {
          std::cout << "[✓] The total number of restocked units of products was calculated correctly\n";
        }
      }

      // 7. VALIDATE THE WORKERS' ROUTES (TIME & MISSED SCHEDULES)
      bool time_exceeded = false;
      bool correct_total_time = true;
      bool correct_missed_schedules = true;
      int expected_total_missed_schedules = 0;

      for (int worker = 0; worker < day_routes.size(); worker++)
      {
        float route_total_time = 0.0f;

        for (int visit = 0; visit < day_routes[worker].size(); visit++)
        {
          int client_id = day_routes[worker][visit].node_id;
          bool missed_schedule = false;

          // Calculate the travel time to this client
          if (visit == 0)
          {
            route_total_time += ctx.time_matrix[worker][ctx.num_workers + client_id];
          }
          else
          {
            int prev_client_id = day_routes[worker][visit - 1].node_id;
            route_total_time += ctx.time_matrix[ctx.num_workers + prev_client_id][ctx.num_workers + client_id];
          }

          // Calculate the wait time for this client
          if (route_total_time < ctx.starting_hour[client_id])
          {
            route_total_time += (ctx.starting_hour[client_id] - route_total_time);
          }

          for (int product_id = 0; product_id < day_routes[worker][visit].restocked_product.size(); product_id++)
          {
            int restocked_product = day_routes[worker][visit].restocked_product[product_id];
            route_total_time += (restocked_product * ctx.restock_time[product_id]);
          }

          if (route_total_time > (ctx.finishing_hour[client_id] + 0.1f))
          {
            missed_schedule = true;
            expected_total_missed_schedules++;
          }

          if (missed_schedule != day_solution.missed_schedule[client_id])
          {
            correct_missed_schedules = false;
            valid = false;
            if (verbose)
            {
              std::cout << "[✗] The missed schedule for client " << client_id << " wasn't marked correctly\n";
            }
          }
        }

        // Return time
        if (!day_routes[worker].empty())
        {
          int last_client_id = day_routes[worker].back().node_id;
          route_total_time += ctx.time_matrix[ctx.num_workers + last_client_id][worker];
        }

        if (route_total_time >= ctx.L)
        {
          time_exceeded = true;
          valid = false;
          if (verbose)
          {
            std::cout << "[✗] The route of worker " << worker << " exceeded the working hours\n";
          }
        }

        // Using a tiny epsilon to account for floating-point inaccuracies
        if (std::abs(route_total_time - day_solution.workers_total_time[worker]) > 0.1f)
        {
          correct_total_time = false;
          valid = false;
          if (verbose)
          {
            std::cout << "[✗] The route total time for worker " << worker << " wasn't calculated correctly\n";
          }
        }
      }

      if (!time_exceeded && verbose)
      {
        std::cout << "[✓] No route exceeded the maximum working hours\n";
      }
      if (correct_total_time && verbose)
      {
        std::cout << "[✓] The total time for all routes were calculated correctly\n";
      }
      if (correct_missed_schedules && verbose)
      {
        std::cout << "[✓] The total missed schedules for all routes were calculated correctly\n";
      }

      // 8. VALIDATE TOTAL DAY UTILITY
      float expected_utility = 0.0f;

      // 8.1 Add utilities from restocked products
      for (int p = 0; p < ctx.num_products; p++)
      {
        expected_utility += total_restocked_products_by_id[p] * ctx.utility[p];
      }

      expected_utility -= (expected_total_missing * ctx.delta);
      expected_utility -= (expected_total_missed_schedules * ctx.theta);

      // Compare using a small epsilon to account for floating-point inaccuracies
      if (std::abs(expected_utility - day_solution.total_day_utility) > 0.1f)
      {
        valid = false;
        if (verbose)
        {
          std::cout << "[✗] The total day utility wasn't calculated correctly\n";
        }
      }
      else
      {
        if (verbose)
        {
          std::cout << "[✓] The total day utility was calculated correctly\n";
        }
      }

      if (verbose)
      {
        std::cout << "\n";
      }
    }

    if (valid)
    {
      if (verbose)
      {
        std::cout << "[✓] All the routes in the solution are valid\n";
      }
    }
    else
    {
      if (verbose)
      {
        std::cout << "[✗] At least one route in the solution is not valid\n";
      }
      throw std::runtime_error("Solution violates routing or inventory constraints.");
    }
  }
}