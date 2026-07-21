#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 3 Ant Colony Optimization (ACO) solver.
   *
   * This pipeline builds upon the historical tracking of V2, but introduces a look-ahead
   * heuristic to proactively prevent stock-outs. It combines:
   * - **Multi-Period Strategy:** Beam Search FutureEffects (evaluates nodes based on
   *   total accumulated profit, but applies a soft penalty if the resulting end-of-day
   *   inventory is insufficient to meet the next day's demand).
   * - **Daily Algorithm:** ACO PrevProfit variation.
   * - **Decoder:** Greedy Decoder PrevProfit (injects accumulated past utility).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param future_effect_weight Multiplier determining how heavily the potential future missing
   *                             inventory penalty affects the day's sorting score (default: 0.63).
   * @param num_ants Number of ants used per iteration in the daily ACO (default: 200).
   * @param alpha Pheromone trail weight; dictates reliance on past experience (default: 1.0).
   * @param beta Heuristic information weight; dictates reliance on greedy choices (default: 2.0).
   * @param q Pheromone deposit constant (default: 100.0).
   * @param evaporation_rate Rate at which pheromones evaporate per iteration, from 0.0 to 1.0 (default: 0.94).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              cumulative utility metrics, and execution time.
   */
  common::SolverResult RunACOV3(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      float future_effect_weight = 0.63,
      int num_ants = 200,
      double alpha = 1,
      double beta = 2,
      double q = 100,
      double evaporation_rate = 0.94);
}