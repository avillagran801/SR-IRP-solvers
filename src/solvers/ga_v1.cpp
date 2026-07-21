#include "ga_v1.hpp"

#include <vector>
#include <string>
#include <chrono>

#include "common/instance.hpp"
#include "decoders/GreedyDecoder.hpp"
#include "daily/GeneticAlgorithm.hpp"
#include "multiperiod/BeamSearch.hpp"

namespace solvers
{
  common::SolverResult RunGAV1(
      std::string instance_name,
      int max_time_seconds,
      int beam_width,
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
    routing::GreedyDecoder decoder;

    // Configure and instantiate the Genetic Algorithm
    daily_solver::GAConfig ga_config{
        population_size, selection_method, replacement_method,
        crossover, mutation, crossover_rate, mutation_rate};

    daily_solver::GeneticAlgorithm ga_solver(ga_config, decoder);

    // Wrap the daily solver's execution into a lambda matching the generic interface
    auto daily_algorithm_lambda = [&](const types::DayContext &ctx, double time_limit)
    {
      return ga_solver.solve(ctx, time_limit);
    };

    auto execution_time_start = std::chrono::steady_clock::now();

    // Run the multi-period solver
    std::vector<types::DaySolution> raw_solution = multi_period::RunBeamSearch(
        instance,
        beam_width,
        max_time_seconds,
        daily_algorithm_lambda);

    auto execution_time_finish = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = execution_time_finish - execution_time_start;

    return common::SolverResult(raw_solution, instance, elapsed_seconds.count());
  };

}