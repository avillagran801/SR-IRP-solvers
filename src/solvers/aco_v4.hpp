#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 4 Ant Colony Optimization (ACO) solver.
   *
   * This pipeline combines historical tracking and look-ahead heuristics with a
   * demand-focused daily decoding strategy. It incorporates:
   * - **Multi-Period Strategy:** Beam Search FutureEffects (evaluates nodes based on
   *   total accumulated profit, applying a soft penalty if the resulting end-of-day
   *   inventory is insufficient to meet the next day's demand).
   * - **Daily Algorithm:** ACO PrevProfit variation.
   * - **Decoder:** Greedy Decoder MoreDemand (dynamically sorts and restocks products
   *   with the highest daily demand first, while injecting accumulated past utility).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param future_effect_weight Multiplier determining how heavily the potential future missing
   *                             inventory penalty affects the day's sorting score (default: 0.20).
   * @param num_ants Number of ants used per iteration in the daily ACO (default: 200).
   * @param alpha Pheromone trail weight; dictates reliance on past experience (default: 1.0).
   * @param beta Heuristic information weight; dictates reliance on greedy choices (default: 1.0).
   * @param q Pheromone deposit constant (default: 100.0).
   * @param evaporation_rate Rate at which pheromones evaporate per iteration, from 0.0 to 1.0 (default: 1.00).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              cumulative utility metrics, and execution time.
   */
  common::SolverResult RunACOV4(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      float future_effect_weight = 0.20,
      int num_ants = 200,
      double alpha = 1,
      double beta = 1,
      double q = 100,
      double evaporation_rate = 1.00);
}