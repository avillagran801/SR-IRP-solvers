#include <iostream>
#include "solvers/ga_v4.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  string instance_name = "../data/test_10_50_1_1_1.v1.txt";

  cout << "Running GA_V4...\n";

  auto result4 = solvers::RunGAV4(instance_name);

  result4.print_summary();

  result4.print_routes();

  return 0;
}