#include "BeamSearch.hpp"
#include "DayNodeSolution.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

namespace multi_period
{
  std::vector<types::DaySolution> RunBeamSearch(
      const instance::InstanceSolverFormat &instance,
      int beam_width,
      int max_time_seconds,
      daily_solver::SolverAlgorithm daily_algorithm)
  {
    int global_node_id = 0;
    std::vector<std::vector<DayNodeSolution>> decision_tree;

    int total_daily_algorithm_runs = 1 + (instance.T - 1) * beam_width;
    double time_per_run = static_cast<double>(max_time_seconds) / total_daily_algorithm_runs;

    // For each day
    for (int i = 0; i < instance.T; i++)
    {
      std::vector<DayNodeSolution> current_day_pool;
      int num_runs = (i == 0) ? 1 : beam_width;

      for (int j = 0; j < num_runs; j++)
      {
        int parent_id;
        std::vector<std::vector<int>> current_start_inventory;

        if (i == 0)
        {
          parent_id = -1;
          current_start_inventory = instance.initial_inventory;
        }
        else
        {
          parent_id = decision_tree[i - 1][j].node_id;
          current_start_inventory = decision_tree[i - 1][j].decoded_solution.end_of_day_inventory;
        }

        types::DayContext ctx(instance, current_start_inventory, instance.demand[i]);

        std::vector<daily_solver::DailyResult> daily_solutions = daily_algorithm(ctx, time_per_run);

        // Convert results into tree nodes using std::move to prevent const reference errors
        for (auto &result : daily_solutions)
        {
          DayNodeSolution node(
              global_node_id++,
              parent_id,
              std::move(result.sequence),
              std::move(result.solution));
          current_day_pool.push_back(std::move(node));
        }
      }

      // Sort descending by utility
      std::sort(current_day_pool.begin(), current_day_pool.end(),
                [](const DayNodeSolution &a, const DayNodeSolution &b)
                {
                  return a.decoded_solution.total_day_utility > b.decoded_solution.total_day_utility;
                });

      // Truncate based on beam width
      if (current_day_pool.size() > beam_width)
      {
        current_day_pool.erase(current_day_pool.begin() + beam_width, current_day_pool.end());
      }

      decision_tree.push_back(current_day_pool);
    }

    // Backtracking phase
    DayNodeSolution best_final_node = decision_tree.back()[0];

    for (const auto &node : decision_tree[instance.T - 1])
    {
      if (node.decoded_solution.total_day_utility > best_final_node.decoded_solution.total_day_utility)
      {
        best_final_node = node;
      }
    }

    std::vector<DayNodeSolution> node_schedule(instance.T, best_final_node);
    std::vector<types::DaySolution> final_schedule(instance.T, best_final_node.decoded_solution);

    DayNodeSolution current_trace_node = best_final_node;
    node_schedule[instance.T - 1] = current_trace_node;

    for (int i = instance.T - 1; i > 0; i--)
    {
      int target_parent_id = current_trace_node.parent_id;

      for (const auto &candidate_parent : decision_tree[i - 1])
      {
        if (candidate_parent.node_id == target_parent_id)
        {
          current_trace_node = candidate_parent;
          node_schedule[i - 1] = current_trace_node;
          final_schedule[i - 1] = current_trace_node.decoded_solution;
          break;
        }
      }
    }

    return final_schedule;
  }
}