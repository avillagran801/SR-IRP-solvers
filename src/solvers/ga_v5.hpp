#pragma once

#include <vector>
#include "common/SolverResult.hpp"

namespace solvers
{
  /**
   * @brief Executes the Version 5 Genetic Algorithm (GA) solver.
   *
   * This pipeline is heavily optimized for stock-out prevention, combining long-term
   * look-ahead heuristics with an aggressive low-stock daily decoding strategy. It incorporates:
   * - **Multi-Period Strategy:** Beam Search FutureEffects (evaluates nodes based on
   *   total accumulated profit, applying a soft penalty if the resulting end-of-day
   *   inventory is insufficient to meet the next day's demand).
   * - **Daily Algorithm:** Genetic Algorithm PrevProfit variation.
   * - **Decoder:** Greedy Decoder LessStock (dynamically sorts and restocks products
   *   that are closest to a stock-out first, while injecting accumulated past utility).
   *
   * @param instance_name The relative or absolute file path to the problem instance text file.
   * @param max_time_seconds Total multi-period time budget in seconds (default: 90).
   * @param beam_width Number of candidate states to preserve per day in the Beam Search (default: 2).
   * @param future_effect_weight Multiplier determining how heavily the potential future missing
   *                             inventory penalty affects the day's sorting score (default 0.29).
   * @param population_size Number of individuals (chromosomes) in the population (default 20).
   * @param selection_method Identifier for the parent selection strategy (0: Tournament, 1: Roulette, 2: Rank, 3: Sigma, 4: Boltzmann) (default 2).
   * @param replacement_method Identifier for the generational replacement strategy (0: KeepBest, 1: KeepChildren, 2: Elitism) (default 0).
   * @param crossover Identifier for the crossover operator to use (0: Order1, 1: Order2, 2: Position, 3: Edge, 4: Edge, 5: Partially Mapped) (default 5).
   * @param mutation Identifier for the mutation operator to use (0: Inversion, 1: Swap2, 2: Swap3, 3: Shuffle, 4: Shift) (default 1).
   * @param crossover_rate Probability (0.0 to 1.0) of applying crossover to selected parents (default 0.60).
   * @param mutation_rate Probability (0.0 to 1.0) of mutating an offspring (default 0.94).
   *
   * @return common::SolverResult An object containing the decoded final routes, multi-period inventory states,
   *                              cumulative utility metrics, and execution time.
   */
  common::SolverResult RunGAV5(
      std::string instance_name,
      int max_time_seconds = 90,
      int beam_width = 2,
      float future_effect_weight = 0.29,
      int population_size = 20,
      int selection_method = 2,
      int replacement_method = 0,
      int crossover = 5,
      int mutation = 1,
      float crossover_rate = 0.60f,
      float mutation_rate = 0.94f);
}