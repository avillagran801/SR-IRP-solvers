#pragma once

#include <vector>
#include "common/types.hpp"
#include "daily/DailyInterface.hpp"

namespace multi_period
{
  /**
   * @brief Executes a Beam Search strategy to solve the multi-period routing and inventory problem.
   *
   * This function acts as the global orchestrator. It builds a decision tree day by day:
   * 1. Evaluates candidate starting inventories for the current day using the provided daily solver.
   * 2. Sorts the resulting solutions by their daily utility.
   * 3. Truncates the pool to keep only the top `beam_width` solutions.
   * 4. Uses the final inventories of those top solutions as the starting states for the next day.
   *
   * The total available time (`max_time_seconds`) is automatically distributed equally
   * among all the required daily solver runs. Once all days (T) are evaluated, it backtracks
   * from the highest-utility node on the final day to return the complete continuous schedule.
   *
   * @param instance The global instance containing demands, capacities, and static constraints.
   * @param beam_width The maximum number of candidate states to preserve at the end of each day.
   *                   (A width of 1 devolves into a pure Greedy search).
   * @param max_time_seconds The total time budget allowed for the entire multi-period execution.
   * @param daily_algorithm A functional callback (e.g., ACO or GA) used to generate single-day solutions.
   *
   * @return std::vector<types::DaySolution> The optimal sequence of daily solutions from Day 1 to Day T.
   */
  std::vector<types::DaySolution> RunBeamSearch(
      const instance::InstanceSolverFormat &instance,
      int beam_width,
      int max_time_seconds,
      daily_solver::SolverAlgorithm daily_algorithm);
}