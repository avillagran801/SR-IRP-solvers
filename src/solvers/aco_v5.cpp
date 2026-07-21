#include "aco_v5.hpp"

#include <vector>
#include <string>
#include <chrono>

#include "common/instance.hpp"
#include "decoders/GreedyDecoderLessStock.hpp"
#include "daily/ACOPrevProfit.hpp"
#include "multiperiod/BeamSearchFutureEffects.hpp"

namespace solvers
{
  common::SolverResult RunACOV5(
      std::string instance_name,
      int max_time_seconds,
      int beam_width,
      float future_effect_weight,
      int num_ants,
      double alpha,
      double beta,
      double q,
      double evaporation_rate)
  {
    // Read and format the instance
    instance::InstanceSolverFormat instance = instance::readInstance(instance_name);

    // Instantiate the decoder
    routing::GreedyDecoderLessStock decoder;

    // Configure and instantiate ACO
    daily_solver::ACOConfigPrevProfit aco_config{
        num_ants, alpha, beta, q, evaporation_rate};

    daily_solver::ACOPrevProfit aco_solver(aco_config, decoder);

    // Wrap the daily solver's execution into a lambda matching the generic interface
    auto daily_algorithm_lambda = [&](const types::DayContext &ctx, double time_limit)
    {
      return aco_solver.solve(ctx, time_limit);
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