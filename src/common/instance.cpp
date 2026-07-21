#include "common/instance.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace instance
{
  InstanceSolverFormat readInstance(string instance_path)
  {
    InstanceSolverFormat data;

    ifstream specs_file(instance_path);

    if (!specs_file.is_open())
    {
      throw std::runtime_error("Could not open instance file: " + instance_path);
    }

    string line;

    // Read general data
    while (getline(specs_file, line))
    {
      if (line.empty() || line[0] == '#')
        continue;

      stringstream ss(line);
      ss >> data.num_workers >> data.num_clients >> data.num_products >> data.T >> data.L >> data.theta >> data.delta;
      break;
    }

    // Read products info
    for (int i = 0; i < data.num_products; i++)
    {
      while (getline(specs_file, line))
      {
        if (line.empty() || line[0] == '#')
          continue;

        stringstream ss(line);
        float restock, profit;
        ss >> restock >> profit;

        data.restock_time.push_back(restock);
        data.utility.push_back(profit);
        break;
      }
    }

    // Discard worker nodes info, since the solver uses the time matrix
    for (int i = 0; i < data.num_workers; i++)
    {
      while (getline(specs_file, line))
      {
        if (line.empty() || line[0] == '#')
          continue;
        break;
      }
    }

    // Initialize the demand vector: [Day][Client][Product]
    data.demand.assign(data.T, vector<vector<int>>(data.num_clients, vector<int>(data.num_products, 0)));

    // Read client nodes info
    for (int i = 0; i < data.num_clients; i++)
    {
      while (getline(specs_file, line))
      {
        if (line.empty() || line[0] == '#')
          continue;

        stringstream ss(line);
        int id;
        float lat, lon, start_time, end_time;

        // Discard cliend id, lat and lon, since the solver uses the time matrix
        ss >> id >> lat >> lon >> start_time >> end_time;
        data.starting_hour.push_back(start_time);
        data.finishing_hour.push_back(end_time);

        // Read initial stock for all products
        vector<int> initial_stock(data.num_products);
        for (int j = 0; j < data.num_products; j++)
        {
          ss >> initial_stock[j];
        }
        data.initial_inventory.push_back(initial_stock);

        // Read max stock for all products
        vector<int> max_stock(data.num_products);
        for (int j = 0; j < data.num_products; j++)
        {
          ss >> max_stock[j];
        }
        data.max_inventory.push_back(max_stock);

        // Read demand for all days and all products
        for (int k = 0; k < data.T; k++)
        {
          for (int j = 0; j < data.num_products; j++)
          {
            ss >> data.demand[k][i][j];
          }
        }
        break;
      }
    }

    // Read time matrix
    while (getline(specs_file, line))
    {
      if (line.empty() || line[0] == '#')
        continue;

      stringstream ss(line);
      string value;
      vector<float> row;

      while (getline(ss, value, ' '))
      {
        row.push_back(stof(value));
      }
      if (!row.empty())
      {
        data.time_matrix.push_back(row);
      }
    }

    specs_file.close();
    return data;
  }

  void printInstance(const InstanceSolverFormat &inst)
  {
    cout << "==========================================\n";
    cout << "INSTANCE CONFIGURATION\n";
    cout << "==========================================\n";
    cout << "Workers: " << inst.num_workers
         << " | Clients: " << inst.num_clients
         << " | Products: " << inst.num_products
         << " | Days: " << inst.T << "\n";
    cout << "Workday Limit (L): " << inst.L << "s\n";
    cout << "Missed Schedule Penalty (theta): " << inst.theta << "\n";
    cout << "Missing Stock Penalty (delta): " << inst.delta << "\n\n";

    cout << "--- PRODUCT DETAILS ---\n";
    for (int i = 0; i < inst.num_products; i++)
    {
      cout << " Product " << i << ": Restock Time = " << inst.restock_time[i]
           << "s, Profit = " << inst.utility[i] << "\n";
    }
    cout << "\n";

    cout << "--- CLIENT DETAILS ---\n";
    for (int i = 0; i < inst.num_clients; i++)
    {
      cout << " Client " << i << ": Window [" << inst.starting_hour[i] << " - " << inst.finishing_hour[i] << "]\n";

      cout << "   Initial Stock: [ ";
      for (int p : inst.initial_inventory[i])
        cout << p << " ";
      cout << "]\n   Max Stock:     [ ";
      for (int p : inst.max_inventory[i])
        cout << p << " ";
      cout << "]\n";

      for (int t = 0; t < inst.T; t++)
      {
        cout << "   Day " << t << " Demand:  [ ";
        for (int p = 0; p < inst.num_products; p++)
        {
          cout << inst.demand[t][i][p] << " ";
        }
        cout << "]\n";
      }
      cout << "\n";
    }

    cout << "--- TIME MATRIX DIMENSIONS ---\n";
    if (inst.time_matrix.empty())
    {
      cout << " 0 rows, 0 columns\n";
    }
    else
    {
      cout << " " << inst.time_matrix.size() << " rows x " << inst.time_matrix[0].size() << " columns\n";
    }
    cout << "==========================================\n\n";
  }
}