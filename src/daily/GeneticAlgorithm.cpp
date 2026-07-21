#include "daily/GeneticAlgorithm.hpp"
#include <gapp/gapp.hpp>
#include <chrono>
#include <stdexcept>

#include <iostream>

namespace daily_solver
{

  // PRIVATE HELPER CLASSES & FUNCTIONS
  namespace
  {
    class DailyReplenishmentFitness : public gapp::FitnessFunctionBase<gapp::PermutationGene>
    {
    private:
      const types::DayContext &ctx;
      const routing::DecoderInterface &decoder;

    public:
      DailyReplenishmentFitness(const types::DayContext &context, const routing::DecoderInterface &dec)
          : gapp::FitnessFunctionBase<gapp::PermutationGene>(context.num_clients), ctx(context), decoder(dec) {}

      gapp::FitnessVector invoke(const gapp::Candidate<gapp::PermutationGene> &solution) const override
      {
        types::DaySolution result = decoder.decode(solution.chromosome, ctx);
        double fitness_value = static_cast<double>(result.total_day_utility);
        return {fitness_value};
      }
    };

    gapp::algorithm::SingleObjective get_algorithm(int selection_method, int replacement_method)
    {
      gapp::algorithm::SingleObjective aux;

      switch (selection_method)
      {
      case 0:
        aux.selection_method(gapp::selection::Tournament());
        break;
      case 1:
        aux.selection_method(gapp::selection::Roulette());
        break;
      case 2:
        aux.selection_method(gapp::selection::Rank());
        break;
      case 3:
        aux.selection_method(gapp::selection::Sigma());
        break;
      case 4:
        aux.selection_method(gapp::selection::Boltzmann());
        break;
      default:
        throw std::invalid_argument("Invalid selection method.");
      }

      switch (replacement_method)
      {
      case 0:
        aux.replacement_method(gapp::replacement::KeepBest());
        break;
      case 1:
        aux.replacement_method(gapp::replacement::KeepChildren());
        break;
      case 2:
        aux.replacement_method(gapp::replacement::Elitism());
        break;
      default:
        throw std::invalid_argument("Invalid replacement method.");
      }
      return aux;
    }

    void configureGA(gapp::GA<gapp::PermutationGene> &ga, const GAConfig &config)
    {
      ga.max_gen(10000);
      ga.stop_condition(gapp::stopping::FitnessBestStall{10});

      ga.population_size(config.population_size);
      ga.algorithm(get_algorithm(config.selection_method, config.replacement_method));

      switch (config.crossover)
      {
      case 0:
        ga.crossover_method(gapp::crossover::perm::Order2(config.crossover_rate));
        break;
      case 1:
        ga.crossover_method(gapp::crossover::perm::Order1(config.crossover_rate));
        break;
      case 2:
        ga.crossover_method(gapp::crossover::perm::Position(config.crossover_rate));
        break;
      case 3:
        ga.crossover_method(gapp::crossover::perm::Cycle(config.crossover_rate));
        break;
      case 4:
        ga.crossover_method(gapp::crossover::perm::Edge(config.crossover_rate));
        break;
      case 5:
        ga.crossover_method(gapp::crossover::perm::PMX(config.crossover_rate));
        break;
      default:
        throw std::invalid_argument("Invalid crossover method.");
      }

      switch (config.mutation)
      {
      case 0:
        ga.mutation_method(gapp::mutation::perm::Inversion(config.mutation_rate));
        break;
      case 1:
        ga.mutation_method(gapp::mutation::perm::Swap2(config.mutation_rate));
        break;
      case 2:
        ga.mutation_method(gapp::mutation::perm::Swap3(config.mutation_rate));
        break;
      case 3:
        ga.mutation_method(gapp::mutation::perm::Shuffle(config.mutation_rate));
        break;
      case 4:
        ga.mutation_method(gapp::mutation::perm::Shift(config.mutation_rate));
        break;
      default:
        throw std::invalid_argument("Invalid mutation method.");
      }
    }
  }

  // SOLVER IMPLEMENTATION

  GeneticAlgorithm::GeneticAlgorithm(GAConfig config, const routing::DecoderInterface &decoder)
      : config(config), decoder(decoder) {}

  std::vector<DailyResult> GeneticAlgorithm::solve(const types::DayContext &ctx, double time_limit_seconds) const
  {
    auto local_start_time = std::chrono::steady_clock::now();

    gapp::execution_threads(1);

    // Create the fitness problem
    DailyReplenishmentFitness problem(ctx, decoder);

    // Configure the GA
    gapp::GA<gapp::PermutationGene> ga;
    configureGA(ga, config);

    // Track final population
    gapp::Population<gapp::PermutationGene> final_population;
    ga.on_generation_end([&final_population](const gapp::GaInfo &ga_info)
                         {
            const auto &current_ga = static_cast<const gapp::GA<gapp::PermutationGene> &>(ga_info);
            final_population = current_ga.population(); });

    // Set time limit condition
    ga.stop_condition([local_start_time, time_limit_seconds](const gapp::GaInfo &ga_info)
                      {
            auto current_time = std::chrono::steady_clock::now();
            std::chrono::duration<double> current_elapsed = current_time - local_start_time;
            return current_elapsed.count() >= time_limit_seconds; });

    // Solve
    ga.solve(problem);

    // Decode final population and return generic results
    std::vector<DailyResult> results;
    for (const auto &candidate : final_population)
    {
      auto sequence = candidate.chromosome;
      types::DaySolution decoded_state = decoder.decode(sequence, ctx);

      results.push_back({sequence, decoded_state});
    }

    return results;
  }
}