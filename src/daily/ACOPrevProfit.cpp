#include "daily/ACOPrevProfit.hpp"
#include <chrono>
#include <vector>
#include <cmath>
#include <random>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace daily_solver
{

  ACOPrevProfit::ACOPrevProfit(ACOConfigPrevProfit config, const routing::DecoderInterface &decoder)
      : config(config), decoder(decoder) {}

  std::vector<DailyResult> ACOPrevProfit::solve(const types::DayContext &ctx, double time_limit_seconds) const
  {
    int elite_sequences_size = 5;
    std::vector<DailyResult> elite_sequences;
    elite_sequences.reserve(elite_sequences_size + 1);

    int generations_without_improvement = 0;
    int patience = 10;
    double delta = 1E-6;
    double best_historical_utility = -std::numeric_limits<double>::max();

    // Current ant solutions (giant tour sequences)
    std::vector<std::vector<size_t>> ant_sequences(config.num_ants);
    std::vector<types::DaySolution> decoded_solutions(config.num_ants, types::DaySolution(ctx.num_workers, ctx.num_clients, ctx.num_products));

    // We add a dummy start node (node 0) that will help us choose the best starter city in the sequence
    // This means that all the indexes in this vector will be shifted one position (i => i + 1)

    // The pheromones for all the arcs (even self-loops that won't be used) are initialized as 0.1
    std::vector<std::vector<double>> pheromones(ctx.num_clients + 1, std::vector<double>(ctx.num_clients + 1, 0.1));

    std::vector<std::vector<double>> precalculated_mult(ctx.num_clients + 1, std::vector<double>(ctx.num_clients + 1));
    std::vector<std::vector<double>> delta_pheromones(ctx.num_clients + 1, std::vector<double>(ctx.num_clients + 1, 0.0));
    std::vector<std::pair<int, double>> probabilities;

    // Start with 1 to avoid a 0 in the denominator later
    std::vector<double> client_heuristics(ctx.num_clients, 1.0);

    // Use the total demand of products as the heuristic
    for (int i = 0; i < ctx.num_clients; i++)
    {
      for (int k = 0; k < ctx.num_products; k++)
      {
        client_heuristics[i] += ctx.demand[i][k];
      }
    }

    std::vector<double> precalculated_heuristics(ctx.num_clients);
    for (int i = 0; i < ctx.num_clients; i++)
    {
      precalculated_heuristics[i] = std::pow(client_heuristics[i], config.beta);
    }

    std::mt19937 rng((unsigned)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<double> unit01(0.0, 1.0);

    auto local_start_time = std::chrono::steady_clock::now();

    while (true)
    {
      // Check time condition
      auto current_time = std::chrono::steady_clock::now();
      double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(current_time - local_start_time).count();

      if (elapsed >= time_limit_seconds)
      {
        break;
      }

      // Precalculate (pheromones[i][j]^alpha) * (heuristic[i][j]^beta) for all the arcs
      for (int start_node = 0; start_node < ctx.num_clients + 1; start_node++)
      {
        for (int end_node = 1; end_node < ctx.num_clients + 1; end_node++)
        {
          precalculated_mult[start_node][end_node] = std::pow(pheromones[start_node][end_node], config.alpha) * precalculated_heuristics[end_node - 1];
        }
      }

      // STEP 1: EACH ANT CONSTRUCTS A GIANT TOUR
      for (int ant = 0; ant < config.num_ants; ant++)
      {
        std::vector<size_t> sequence;
        std::vector<bool> allowed_clients(ctx.num_clients, true);

        int available_clients = ctx.num_clients;

        while (available_clients > 0)
        {
          // -1 is the dummy starter node
          int last_client = -1;

          if (!sequence.empty())
          {
            last_client = sequence.back();
          }

          // Calculate the sum of (pheromones[i][j]^alpha) * (heuristic[i][j]^beta) for all the allowed clients
          double mult_sum = 0.0;

          for (int client_id = 0; client_id < ctx.num_clients; client_id++)
          {
            if (allowed_clients[client_id])
            {
              mult_sum += precalculated_mult[last_client + 1][client_id + 1];
            }
          }

          // The probability of each allowed client is saved as (client index, probability)
          probabilities.clear();

          // Each probability is (pheromones[i][j]^alpha) * (heuristic[i][j]^beta) / sum of (pheromones[i][k]^alpha) * (heuristic[i][k]^beta) for all the allowed nodes
          for (int client_id = 0; client_id < ctx.num_clients; client_id++)
          {
            if (allowed_clients[client_id])
            {
              probabilities.emplace_back(client_id, precalculated_mult[last_client + 1][client_id + 1] / mult_sum);
            }
          }

          // Randomly select an allowed client based on its probabilities
          double r = unit01(rng);
          double cumulative = 0.0;
          int selected_client = probabilities.back().first; // Falback just in case

          for (int j = 0; j < probabilities.size(); j++)
          {
            cumulative += probabilities[j].second;
            if (r < cumulative)
            {
              selected_client = probabilities[j].first;
              break;
            }
          }

          // Add the selected client to the sequence
          sequence.push_back(selected_client);

          // Remove the selected client from the allowed clients
          allowed_clients[selected_client] = false;
          available_clients--;
        }

        // Add the sequence to ant_sequences
        ant_sequences[ant] = (sequence);
      }

      // STEP 2: UPDATE THE PHEROMONES

      float worst_current_solution_value;
      float best_current_solution_value;

      // Decode all the solutions and save the worst and best solution values for this iteration
      for (int i = 0; i < config.num_ants; i++)
      {
        decoded_solutions[i] = decoder.decode(ant_sequences[i], ctx);

        if (i == 0)
        {
          // CHANGE LATER
          worst_current_solution_value = decoded_solutions[i].total_accumulated_profit;
          best_current_solution_value = decoded_solutions[i].total_accumulated_profit;
        }
        else
        {
          if (decoded_solutions[i].total_accumulated_profit < worst_current_solution_value)
          {
            worst_current_solution_value = decoded_solutions[i].total_accumulated_profit;
          }
          if (decoded_solutions[i].total_accumulated_profit > best_current_solution_value)
          {
            best_current_solution_value = decoded_solutions[i].total_accumulated_profit;
          }
        }

        bool is_duplicate = false;
        for (const auto &elite : elite_sequences)
        {
          if (elite.sequence == ant_sequences[i])
          {
            is_duplicate = true;
            break;
          }
        }

        if (!is_duplicate)
        {
          DailyResult current_result = {ant_sequences[i], decoded_solutions[i]};

          if (elite_sequences.size() < elite_sequences_size)
          {
            elite_sequences.push_back(current_result);
            std::sort(elite_sequences.begin(), elite_sequences.end(), [](const DailyResult &a, const DailyResult &b)
                      { return a.solution.total_accumulated_profit > b.solution.total_accumulated_profit; });
          }
          else
          {
            if (current_result.solution.total_accumulated_profit > elite_sequences.back().solution.total_accumulated_profit)
            {
              auto it = std::lower_bound(elite_sequences.begin(), elite_sequences.end(), current_result, [](const DailyResult &a, const DailyResult &b)
                                         { return a.solution.total_accumulated_profit > b.solution.total_accumulated_profit; });
              elite_sequences.insert(it, current_result);
              elite_sequences.pop_back();
            }
          }
        }
      }

      if (!elite_sequences.empty())
      {
        double current_best_utility = elite_sequences.front().solution.total_accumulated_profit;

        // Check if the current best improves upon the historical best by at least 'delta'
        if (current_best_utility > best_historical_utility + delta)
        {
          best_historical_utility = current_best_utility;
          generations_without_improvement = 0;
        }
        else
        {
          generations_without_improvement++;
        }

        // Stop if we've run out of patience
        if (generations_without_improvement >= patience)
        {
          break;
        }
      }

      for (auto &row : delta_pheromones)
      {
        std::fill(row.begin(), row.end(), 0.0);
      }

      // Calculate the delta pheromones
      for (int i = 0; i < config.num_ants; i++)
      {
        // The deposit uses a normalized solution quality value
        double deposit = config.q * ((decoded_solutions[i].total_accumulated_profit - worst_current_solution_value + 0.001) /
                                     (best_current_solution_value - worst_current_solution_value + 0.001));

        for (int j = 0; j < ant_sequences[i].size(); j++)
        {
          int last_client = -1; // The dummy starter node
          if (j != 0)
          {
            last_client = ant_sequences[i][j - 1];
          }

          int client_id = ant_sequences[i][j];

          delta_pheromones[last_client + 1][client_id + 1] += deposit;
        }
      }

      // Update the pheromones
      for (int start_node = 0; start_node < ctx.num_clients + 1; start_node++)
      {
        for (int end_node = 1; end_node < ctx.num_clients + 1; end_node++)
        {
          pheromones[start_node][end_node] = (1.0 - config.evaporation_rate) * pheromones[start_node][end_node] + delta_pheromones[start_node][end_node];
        }
      }
    }

    return elite_sequences;
  }
}
