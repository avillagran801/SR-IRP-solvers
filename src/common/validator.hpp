#include "common/types.hpp"
#include <vector>
#include <deque>

namespace validator
{
  /**
   * @brief Validates a complete multi-period routing solution against instance constraints.
   *
   * This function performs a rigorous check on the solution, ensuring that:
   * - Clients are visited at most once per day.
   * - At least 1 unit of product is restocked per visit.
   * - Client inventory capacities are never exceeded.
   * - Missing products and final inventories are calculated correctly.
   * - Worker route times do not exceed maximum working hours.
   * - Day utilities are calculated correctly factoring in penalties.
   *
   * @param final_solution The sequence of calculated daily solutions.
   * @param day_contexts The sequence of daily tracking parameters (inventories and demands).
   * @param verbose If true, prints detailed check results for every constraint to standard output.
   *
   * @throws std::runtime_error if the solution violates any routing or inventory constraint.
   */
  void validate_solution(std::vector<types::DaySolution> final_solution,
                         std::deque<types::DayContext> day_contexts,
                         bool verbose = false);
}