#pragma once

#include <vector>
#include "common/SolverResult.hpp"

/**
 * @namespace solvers
 * @brief Contains all daily routing algorithms (ACO and GA).
 */
namespace solvers
{
  /**
   * @brief Executes the Version 1 Ant Colony Optimization (ACO) solver.
   *
   * This specific solver pipeline combines:
   * - **Multi-Period Strategy:** Standard Beam Search (evaluates nodes based on isolated daily utility).
   * - **Daily Algorithm:** Standard Ant Colony Optimization (ACO).
   * - **Decoder:** Standard Greedy Decoder (demand fulfillment followed by capacity maximization).
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
   *                              utility metrics, and execution time.
   */
  common::SolverResult RunACOV1(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      int num_ants = 200,
      double alpha = 1,
      double beta = 1,
      double q = 90,
      double evaporation_rate = 0.89);
}