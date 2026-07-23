#pragma once

#include <vector>
#include "common/instance.hpp"

namespace types
{
  /**
   * @struct DayContext
   * @brief Represents the isolated problem state for a single specific day.
   *
   * It holds references to the static instance data to avoid memory duplication,
   * while maintaining the specific starting inventory, demand, and accumulated
   * utilities unique to the current day being solved.
   */
  struct DayContext
  {
    const std::vector<std::vector<int>> start_inventory; /**< Inventory levels at the beginning of this day. */
    const std::vector<std::vector<int>> demand;          /**< Expected demand for this specific day. */

    const int num_clients;                              /**< Total number of client nodes to serve. */
    const int num_workers;                              /**< Total number of workers available per day. */
    const int num_products;                             /**< Number of distinct product types. */
    const int T;                                        /**< Total number of days in the planning horizon. */
    const float L;                                      /**< Maximum working hours allowed per worker per day. */
    const float delta;                                  /**< Penalty coefficient applied per unit of missing inventory. */
    const float theta;                                  /**< Penalty coefficient applied per missed delivery schedule. */
    const std::vector<std::vector<float>> &time_matrix; /**< Travel time matrix between all nodes (workers and clients). */
    const std::vector<int> &utility;                    /**< Profit or utility gained per unit of each product restocked. */
    const std::vector<float> &restock_time;             /**< Time required to restock one unit of each product type. */
    const std::vector<float> &starting_hour;            /**< Earliest allowed service start time for each client. */
    const std::vector<float> &finishing_hour;           /**< Latest allowed service finish time for each client. */
    const std::vector<std::vector<int>> &max_inventory; /**< Maximum shelf capacity [client_id][product_id]. */
    const int prev_utilities;                           /**< Accumulated utility from all previous days. */

    /**
     * @brief Constructs a new DayContext object from the global instance and current day's state.
     *
     * @param inst The global static instance.
     * @param inv The starting inventory matrix for this day.
     * @param dem The demand matrix for this day.
     * @param prev_util Accumulated total utility from previous days (default: 0).
     */
    DayContext(const instance::InstanceSolverFormat &inst, const std::vector<std::vector<int>> &inv, const std::vector<std::vector<int>> &dem, const int &prev_util = 0)
        : num_clients(inst.num_clients),
          num_workers(inst.num_workers),
          num_products(inst.num_products),
          T(inst.T),
          L(inst.L),
          delta(inst.delta),
          theta(inst.theta),
          time_matrix(inst.time_matrix),
          utility(inst.utility),
          restock_time(inst.restock_time),
          starting_hour(inst.starting_hour),
          finishing_hour(inst.finishing_hour),
          max_inventory(inst.max_inventory),
          prev_utilities(prev_util),

          start_inventory(inv),
          demand(dem)
    {
    }
  };

  /**
   * @struct Visit
   * @brief Represents a single client visit made by a worker within a route.
   */
  struct Visit
  {
    int node_id;                        /**< The ID of the client being visited. */
    std::vector<int> restocked_product; /**< Amount of each product type restocked during this visit. */
    float wait_time;                    /**< Time spent waiting before service could start. */
    float visit_start_time;             /**< The exact time the worker began restocking. */
    float visit_ending_time;            /**< The exact time the worker finished restocking. */

    /**
     * @brief Constructs a Visit with empty restock arrays and zeroed times.
     *
     * @param id The client node ID.
     * @param num_products Total number of distinct products to initialize the restock array.
     */
    Visit(int id, int num_products)
        : node_id(id), restocked_product(num_products, 0), wait_time(0.0f), visit_start_time(0.0f), visit_ending_time(0.0f) {}
  };

  /**
   * @struct DaySolution
   * @brief Encapsulates the complete routing and inventory solution for a single day.
   */
  struct DaySolution
  {
    float total_accumulated_profit;       /**< Utility from this day PLUS all previous days. */
    float total_day_utility = 0;          /**< Utility generated strictly during this day (including penalties). */
    int total_day_restocked_products = 0; /**< Total sum of all product units restocked this day. */
    int total_day_missing_inventory = 0;  /**< Total sum of all missing product units at the end of the day. */
    int total_day_missed_schedules = 0;   /**< Total number of clients whose finishing hour was missed. */

    std::vector<std::vector<Visit>> routes;             /**< The assigned sequence of visits per worker [worker_id][visit_index]. */
    std::vector<float> workers_total_time;              /**< Total shift duration used by each worker [worker_id]. */
    std::vector<std::vector<int>> end_of_day_inventory; /**< Final inventory levels [client_id][product_id]. */
    std::vector<std::vector<int>> missing_inventory;    /**< Amount of missing product units [client_id][product_id]. */
    std::vector<bool> missed_schedule;                  /**< True if the client's service ended after their finishing hour [client_id]. */

    /**
     * @brief Constructs an empty DaySolution, properly sizing all vectors.
     *
     * @param num_workers Number of workers (determines routes and times vectors).
     * @param num_clients Number of clients (determines inventory and schedule arrays).
     * @param num_products Number of products.
     * @param prev_profit The accumulated profit from prior days to carry over (default: 0).
     */
    DaySolution(int num_workers, int num_clients, int num_products, int prev_profit = 0)
        : routes(num_workers),
          workers_total_time(num_workers, 0.0f),
          end_of_day_inventory(num_clients, std::vector<int>(num_products, 0)),
          missing_inventory(num_clients, std::vector<int>(num_products, 0)),
          missed_schedule(num_clients, false),
          total_accumulated_profit(prev_profit)
    {
    }
  };
}