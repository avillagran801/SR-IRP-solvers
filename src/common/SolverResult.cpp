#include "common/SolverResult.hpp"
#include "common/validator.hpp"
#include <iostream>

namespace common
{
  SolverResult::SolverResult(const std::vector<types::DaySolution> &sol,
                             const instance::InstanceSolverFormat &inst,
                             double exec_time)
      : solution(sol), instance(inst), execution_time_seconds(exec_time)
  {
    build_contexts();
  }

  void SolverResult::build_contexts()
  {
    std::vector<std::vector<int>> tracking_inventory = instance.initial_inventory;
    for (int i = 0; i < instance.T; i++)
    {
      day_contexts.emplace_back(instance, tracking_inventory, instance.demand[i]);
      if (i < solution.size())
      {
        tracking_inventory = solution[i].end_of_day_inventory;
      }
    }
  }

  const std::vector<types::DaySolution> &SolverResult::get_solution() const
  {
    return solution;
  }

  bool SolverResult::validate(bool verbose) const
  {
    try
    {
      validator::validate_solution(solution, day_contexts, verbose);
      return true;
    }
    catch (const std::exception &e)
    {
      std::cerr << "Validation Failed: " << e.what() << "\n";
      return false;
    }
  }

  float SolverResult::get_total_profit() const
  {
    float total_utility = 0.0f;
    for (const auto &day : solution)
    {
      total_utility += day.total_day_utility;
    }
    return total_utility;
  }

  int SolverResult::get_total_restocked() const
  {
    int total_restocked = 0;
    for (const auto &day : solution)
    {
      total_restocked += day.total_day_restocked_products;
    }
    return total_restocked;
  }

  int SolverResult::get_total_missing() const
  {
    int total_missing = 0;
    for (const auto &day : solution)
    {
      for (const auto &client_missing : day.missing_inventory)
      {
        for (int missing_product_amount : client_missing)
        {
          total_missing += missing_product_amount;
        }
      }
    }
    return total_missing;
  }

  int SolverResult::get_total_visits() const
  {
    int total = 0;
    for (const auto &day : solution)
    {
      for (const auto &route : day.routes)
      {
        total += route.size();
      }
    }
    return total;
  }

  int SolverResult::get_total_missed_schedules() const
  {
    int total = 0;
    for (const auto &day : solution)
    {
      total += day.total_day_missed_schedules;
    }
    return total;
  }

  int SolverResult::get_avg_working_hours() const
  {
    int schedules_counter = 0;
    int working_hours_average = 0;

    for (const auto &day : solution)
    {
      for (float time : day.workers_total_time)
      {
        working_hours_average += time;
        schedules_counter++;
      }
    }

    if (schedules_counter > 0)
    {
      return working_hours_average / schedules_counter;
    }
    return 0;
  }

  void SolverResult::print_summary(bool tuning) const
  {
    if (tuning)
    {
      std::cout << -1 * get_total_profit() << "\n";
    }
    else
    {

      std::cout << "\n=== Solver Result Summary ===\n"
                << "Total Profit: " << get_total_profit() << "\n"
                << "Total Restocked Units: " << get_total_restocked() << "\n"
                << "Total Missing Units: " << get_total_missing() << "\n"
                << "Total Visits: " << get_total_visits() << "\n"
                << "Total Missed Schedules: " << get_total_missed_schedules() << "\n"
                << "Average Working Hours: " << get_avg_working_hours() << " s\n"
                << "Total Execution Time: " << execution_time_seconds << " s\n"
                << "=============================\n\n";
    }
  }
}