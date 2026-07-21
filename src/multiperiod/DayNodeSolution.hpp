#pragma once

#include <vector>
#include "common/types.hpp"

namespace multi_period
{
  struct DayNodeSolution
  {
    int node_id;
    int parent_id;
    std::vector<size_t> chromosome_sequence;
    types::DaySolution decoded_solution;

    DayNodeSolution(int id, int parent, std::vector<size_t> sequence, types::DaySolution solution)
        : node_id(id), parent_id(parent), chromosome_sequence(std::move(sequence)), decoded_solution(std::move(solution)) {}
  };
}