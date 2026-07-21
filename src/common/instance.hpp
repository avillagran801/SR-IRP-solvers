#pragma once

#include <vector>
#include <string>

namespace instance
{
  /**
   * @struct InstanceSolverFormat
   * @brief Holds all static parameters, capacities, and demands for the multi-period routing problem.
   *
   * This structure is populated directly from the parsed instance files.
   */
  struct InstanceSolverFormat
  {
    int num_clients;  /**< Total number of client nodes to serve. */
    int num_workers;  /**< Total number of workers available per day. */
    int num_products; /**< Number of distinct product types. */
    int T;            /**< Total number of days in the planning horizon. */
    float L;          /**< Maximum working hours allowed per worker per day. */
    float delta;      /**< Penalty coefficient applied per unit of missing inventory. */
    float theta;      /**< Penalty coefficient applied per missed delivery schedule. */

    std::vector<std::vector<float>> time_matrix; /**< Travel time matrix between all nodes (workers and clients). */
    std::vector<int> utility;                    /**< Profit or utility gained per unit of each product restocked. */
    std::vector<float> restock_time;             /**< Time required to restock one unit of each product type. */
    std::vector<float> starting_hour;            /**< Earliest allowed service start time for each client. */
    std::vector<float> finishing_hour;           /**< Latest allowed service finish time for each client. */

    std::vector<std::vector<int>> max_inventory;       /**< Maximum shelf capacity [client_id][product_id]. */
    std::vector<std::vector<int>> initial_inventory;   /**< Inventory levels at the start of Day 1 [client_id][product_id]. */
    std::vector<std::vector<std::vector<int>>> demand; /**< Expected daily demand [day][client_id][product_id]. */
  };

  /**
   * @brief Reads the text file and parses it directly into the solver format.
   *
   * @param instance_path The relative or absolute file path to the instance definition text file.
   * @return An InstanceSolverFormat struct populated with the parsed data.
   */
  InstanceSolverFormat readInstance(std::string instance_path);

  /**
   * @brief Prints all the parsed instance information to standard output for debugging purposes.
   *
   * @param inst The instance format object to print.
   */
  void printInstance(const InstanceSolverFormat &inst);
}