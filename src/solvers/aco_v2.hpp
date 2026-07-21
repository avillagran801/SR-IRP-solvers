#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 2 Ant Colony Optimization (ACO) solver.
   *
   * This specific solver pipeline is built around cumulative historical tracking. It combines:
   * - **Multi-Period Strategy:** Beam Search PrevProfit (evaluates and truncates nodes based on
   *   their total accumulated profit across the time horizon, rather than isolated daily performance).
   * - **Daily Algorithm:** ACO PrevProfit variation.
   * - **Decoder:** Greedy Decoder PrevProfit (injects accumulated past utility into the evaluation).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param num_ants Number of ants used per iteration in the daily ACO (default: 200).
   * @param alpha Pheromone trail weight; dictates reliance on past experience (default: 1.0).
   * @param beta Heuristic information weight; dictates reliance on greedy choices (default: 1.0).
   * @param q Pheromone deposit constant (default: 90.0).
   * @param evaporation_rate Rate at which pheromones evaporate per iteration, from 0.0 to 1.0 (default: 0.89).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              cumulative utility metrics, and execution time.
   */
  common::SolverResult RunACOV2(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 1,
      int num_ants = 200,
      double alpha = 1,
      double beta = 1,
      double q = 30,
      double evaporation_rate = 0.88);
}