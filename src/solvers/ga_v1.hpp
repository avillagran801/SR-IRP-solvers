#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 1 Genetic Algorithm (GA) solver.
   *
   * This specific solver pipeline combines the baseline versions of all components:
   * - **Multi-Period Strategy:** Standard Beam Search (evaluates nodes based on isolated daily utility).
   * - **Daily Algorithm:** Standard Genetic Algorithm (GA).
   * - **Decoder:** Standard Greedy Decoder (demand fulfillment followed by capacity maximization).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param population_size Number of individuals (chromosomes) in the population (default: 20).
   * @param selection_method Identifier for the parent selection strategy (0: Tournament, 1: Roulette, 2: Rank, 3: Sigma, 4: Boltzmann) (default: 3).
   * @param replacement_method Identifier for the generational replacement strategy (0: KeepBest, 1: KeepChildren, 2: Elitism) (default: 0).
   * @param crossover Identifier for the crossover operator to use (0: Order1, 1: Order2, 2: Position, 3: Edge, 4: Partially Mapped) (default: 3).
   * @param mutation Identifier for the mutation operator to use (0: Inversion, 1: Swap2, 2: Swap3, 3: Shuffle, 4: Shift) (default: 1).
   * @param crossover_rate Probability (0.0 to 1.0) of applying crossover to selected parents (default: 0.2).
   * @param mutation_rate Probability (0.0 to 1.0) of mutating an offspring (default: 0.98).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              utility metrics, and execution time.
   */
  common::SolverResult RunGAV1(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      int population_size = 20,
      int selection_method = 3,
      int replacement_method = 0,
      int crossover = 3,
      int mutation = 1,
      float crossover_rate = 0.2f,
      float mutation_rate = 0.98f);
}