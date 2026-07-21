# Shelf Replenishment Inventory Routing Problem (SR-IRP) Solvers

C++ solvers designed to tackle multi-period worker routing and inventory management problems. This project combines meta-heuristic algorithms (Ant Colony Optimization and Genetic Algorithms) with customized greedy decoders and a Beam Search orchestrator to optimize long-term scheduling, demand fulfillment, and inventory levels.

## Features

The solvers separate the daily routing logic from the multi-period orchestration, allowing for customizable pipelines (Versions 1 through 5 of both ACO and GA).

### Daily Solvers
* **Genetic Algorithm (GA):** Evolves a population of routing sequences over multiple generations.
* **Ant Colony Optimization (ACO):** Routes workers by simulating pheromone trails and heuristic information.

### Decoders
Translates abstract sequences into concrete routing schedules and inventory states:
* **Standard Greedy:** Fulfills demand first, then maximizes leftover shelf capacity.
* **PrevProfit:** Injects historical cumulative utility into the daily evaluation.
* **MoreDemand:** Prioritizes high-volume products to ensure critical demand is met before time runs out.
* **LessStock:** Dynamically prioritizes restocking products that are closest to a stock-out.

### Multi-Period Orchestration (Beam Search)
* **Standard Beam Search:** Truncates candidate states based on isolated daily utility.
* **PrevProfit:** Tracks and evaluates based on long-term accumulated profit.
* **FutureEffects:** A look-ahead heuristic that applies soft penalties to solutions that leave inventory vulnerable to stock-outs the next day.

## Prerequisites

* **C++ Compiler:** Supports C++17 or higher (GCC, Clang, or MSVC).
* **CMake:** Version 3.21 or higher.

## Build Instructions

This project uses CMake to manage the build process and compile the core library and example executable.

1. **Clone the repository:**
   ```bash
   git clone https://github.com/avillagran801/SR-IRP-solvers.git
   cd WorkerRoutingSolver
   ```

2. **Generate the build files:**
```bash
mkdir build
cd build
cmake ..
```

3. **Compile the project:**
```bash
make
```
This will output an executable named ```example``` in your ```build/``` directory.

## Usage
You can run the generated example executable to test the solver against a specific problem instance. Ensure you have your instance text files available.

```bash
# Run from inside the build directory
./example ../data/my_instance.txt
```

To swap out algorithms, edit src/example.cpp to call a different solver version (e.g., solvers::RunACOV5() instead of solvers::RunGAV4()) and recompile.

## Credits

The Genetic Algorithm variations in this project are powered by the GAPP library provided by KRM7 in [https://github.com/KRM7/gapp/](https://github.com/KRM7/gapp/].

## License

This project is licensed under the MIT License.

