#pragma once

#include "DailyInterface.hpp"
#include "decoders/DecoderInterface.hpp"

namespace daily_solver
{
  /**
   * @struct GAConfigPrevProfit
   * @brief Configuration parameters for the Genetic Algorithm (GA) version
   * that considers previous profit.
   */
  struct GAConfigPrevProfit
  {
    int population_size;    /**< Number of individuals (chromosomes) in the population. */
    int selection_method;   /**< Identifier for the parent selection strategy (0: Tournament, 1: Roulette, 2: Rank, 3: Sigma, 4: Boltzmann). */
    int replacement_method; /**< Identifier for the generational replacement strategy (0: KeepBest, 1: KeepChildren, 2: Elitism). */
    int crossover;          /**< Identifier for the crossover operator to use (0: Order1, 1: Order2, 2: Position, 3: Edge, 4: Partially Mapped). */
    int mutation;           /**< Identifier for the mutation operator to use (0: Inversion, 1: Swap2, 2: Swap3, 3: Shuffle, 4: Shift). */
    float crossover_rate;   /**< Probability (0.0 to 1.0) of applying crossover to selected parents. */
    float mutation_rate;    /**< Probability (0.0 to 1.0) of mutating an offspring. */
  };

  /**
   * @class GeneticAlgorithmPrevProfit
   * @brief Evolutionary algorithm solver for the daily routing and inventory problem
   * that considers previous profit.
   *
   * This class evolves a population of routing sequences over multiple generations
   * to optimize utility, decoding the chromosomes into actual routes using the
   * provided DecoderInterface.
   */
  class GeneticAlgorithmPrevProfit
  {
  private:
    GAConfigPrevProfit config;
    const routing::DecoderInterface &decoder;

  public:
    /**
     * @brief Constructs the GeneticAlgorithmPrevProfit solver.
     *
     * @param config The hyperparameter configuration defining evolutionary rates and operators.
     * @param decoder The decoder strategy used to translate chromosomes into actual daily routes.
     */
    GeneticAlgorithmPrevProfit(GAConfigPrevProfit config, const routing::DecoderInterface &decoder);

    /**
     * @brief Executes the GeneticAlgorithmPrevProfit solver for a single day.
     *
     * @param ctx The context (inventory, demand, constraints) for the current day.
     * @param time_limit_seconds Maximum allowed execution time for this daily solve.
     * @return A vector of candidate DailyResult solutions from the final population.
     */
    std::vector<DailyResult> solve(const types::DayContext &ctx, double time_limit_seconds) const;
  };
}