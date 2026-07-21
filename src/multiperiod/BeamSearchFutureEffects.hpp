#pragma once

#include <vector>
#include "common/types.hpp"
#include "daily/DailyInterface.hpp"

namespace multi_period
{
  /**
   * @brief Executes a Beam Search strategy with a look-ahead heuristic to anticipate future shortages.
   *
   * This solver builds upon the accumulated profit tracking, but introduces a "soft penalty"
   * during the node evaluation phase. When sorting the beam at the end of the day, it looks
   * ahead to the next day's demand. If a solution leaves the end-of-day inventory lower than
   * tomorrow's demand, its sorting score is penalized.
   *
   * This prevents the solver from truncating solutions that might have slightly lower utility
   * today, but perfectly set up the inventory to avoid massive stock-out penalties tomorrow.
   *
   * @param instance The global instance containing demands, capacities, and static constraints.
   * @param beam_width The maximum number of candidate states to preserve at the end of each day.
   * @param future_effect_weight A multiplier (from 0.0 to 1.0) determining how heavily the
   *                             potential future missing inventory penalty should affect today's sorting score.
   * @param max_time_seconds The total time budget allowed for the entire multi-period execution.
   * @param daily_algorithm A functional callback (e.g., ACO or GA) used to generate single-day solutions.
   *
   * @return std::vector<types::DaySolution> The optimal sequence of daily solutions from Day 1 to Day T.
   */
  std::vector<types::DaySolution> RunBeamSearchFutureEffects(
      const instance::InstanceSolverFormat &instance,
      int beam_width,
      float future_effect_weight,
      int max_time_seconds,
      daily_solver::SolverAlgorithm daily_algorithm);
}