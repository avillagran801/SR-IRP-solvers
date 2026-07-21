#pragma once

#include <vector>
#include "common/types.hpp"
#include "daily/DailyInterface.hpp"

namespace multi_period
{
  /**
   * @brief Executes a Beam Search strategy that tracks and evaluates based on accumulated multi-period profit.
   *
   * Unlike the standard Beam Search which evaluates and truncates nodes based solely
   * on their isolated daily utility, this variation tracks the cumulative profit across
   * the time horizon. It injects the parent node's accumulated profit into the child's
   * context, and sorts the daily beam based on the highest `total_accumulated_profit`.
   *
   * This ensures the search favors solutions that build long-term value over those
   * that only perform well on a single isolated day.
   *
   * @param instance The global instance containing demands, capacities, and static constraints.
   * @param beam_width The maximum number of candidate states to preserve at the end of each day.
   * @param max_time_seconds The total time budget allowed for the entire multi-period execution.
   * @param daily_algorithm A functional callback (e.g., ACO or GA) used to generate single-day solutions.
   *
   * @return std::vector<types::DaySolution> The optimal sequence of daily solutions from Day 1 to Day T.
   */
  std::vector<types::DaySolution> RunBeamSearchPrevProfit(
      const instance::InstanceSolverFormat &instance,
      int beam_width,
      int max_time_seconds,
      daily_solver::SolverAlgorithm daily_algorithm);
}