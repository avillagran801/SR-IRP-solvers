#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 3 Genetic Algorithm (GA) solver.
   *
   * This pipeline builds upon the historical tracking of V2, but introduces a look-ahead
   * heuristic to proactively prevent stock-outs. It combines:
   * - **Multi-Period Strategy:** Beam Search FutureEffects (evaluates nodes based on
   *   total accumulated profit, but applies a soft penalty if the resulting end-of-day
   *   inventory is insufficient to meet the next day's demand).
   * - **Daily Algorithm:** Genetic Algorithm PrevProfit variation.
   * - **Decoder:** Greedy Decoder PrevProfit (injects accumulated past utility).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param future_effect_weight Multiplier determining how heavily the potential future missing
   *                             inventory penalty affects the day's sorting score (default: 0.33).
   * @param population_size Number of individuals (chromosomes) in the population (default: 40).
   * @param selection_method Identifier for the parent selection strategy (0: Tournament, 1: Roulette, 2: Sigma, 3: Boltzmann) (default: 0).
   * @param replacement_method Identifier for the generational replacement strategy (0: KeepBest, 1: KeepChildren, 2: Elitism) (default: 0).
   * @param crossover Identifier for the crossover operator to use (0: Order1, 1: Order2, 2: Position, 3: Edge, 4: Partially Mapped) (default: 0).
   * @param mutation Identifier for the mutation operator to use (0: Inversion, 1: Swap2, 2: Swap3, 3: Shuffle, 4: Shift) (default: 1).
   * @param crossover_rate Probability (0.0 to 1.0) of applying crossover to selected parents (default: 0.15).
   * @param mutation_rate Probability (0.0 to 1.0) of mutating an offspring (default: 0.99).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              cumulative utility metrics, and execution time.
   */
  common::SolverResult RunGAV3(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      float future_effect_weight = 0.33,
      int population_size = 40,
      int selection_method = 0,
      int replacement_method = 0,
      int crossover = 0,
      int mutation = 1,
      float crossover_rate = 0.15f,
      float mutation_rate = 0.99f);
}