#include "ga_v3.hpp"

#include <vector>
#include <string>
#include <chrono>

#include "common/instance.hpp"
#include "decoders/GreedyDecoderPrevProfit.hpp"
#include "daily/GeneticAlgorithmPrevProfit.hpp"
#include "multiperiod/BeamSearchFutureEffects.hpp"

namespace solvers
{
  common::SolverResult RunGAV3(
      std::string instance_name,
      int max_time_seconds,
      int beam_width,
      float future_effect_weight,
      int population_size,
      int selection_method,
      int replacement_method,
      int crossover,
      int mutation,
      float crossover_rate,
      float mutation_rate)
  {
    // Read and format the instance
    instance::InstanceSolverFormat instance = instance::readInstance(instance_name);

    // Instantiate the decoder
    routing::GreedyDecoderPrevProfit decoder;

    // Configure and instantiate the Genetic Algorithm
    daily_solver::GAConfigPrevProfit ga_config{
        population_size, selection_method, replacement_method,
        crossover, mutation, crossover_rate, mutation_rate};

    daily_solver::GeneticAlgorithmPrevProfit ga_solver(ga_config, decoder);

    // Wrap the daily solver's execution into a lambda matching the generic interface
    auto daily_algorithm_lambda = [&](const types::DayContext &ctx, double time_limit)
    {
      return ga_solver.solve(ctx, time_limit);
    };

    auto execution_time_start = std::chrono::steady_clock::now();

    // Run the multi-period solver
    std::vector<types::DaySolution> raw_solution = multi_period::RunBeamSearchFutureEffects(
        instance,
        beam_width,
        future_effect_weight,
        max_time_seconds,
        daily_algorithm_lambda);

    auto execution_time_finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = execution_time_finish - execution_time_start;

    return common::SolverResult(raw_solution, instance, elapsed_seconds.count());
  }
}