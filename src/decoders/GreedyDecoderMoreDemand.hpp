#pragma once

#include "DecoderInterface.hpp"

namespace routing
{
    /**
     * @class GreedyDecoderMoreDemand
     * @brief A greedy decoder that prioritizes products with the highest demand and incorporates historical profit.
     *
     * Unlike the standard decoder which processes products sequentially by ID, this
     * variation dynamically sorts a client's products in descending order based strictly
     * on their expected daily demand. High-volume products are serviced first, ensuring
     * that if a worker's shift time runs out, the most heavily demanded items are already restocked.
     *
     * Additionally, it tracks multi-period fitness by injecting the accumulated profit from
     * previous days (`ctx.prev_utilities`) into the final evaluated `DaySolution`.
     */
    class GreedyDecoderMoreDemand : public DecoderInterface
    {
    public:
        /**
         * @brief Decodes the sequence using a highest-demand-first heuristic and calculates total multi-period utility.
         *
         * @param sequence The ordered list of client IDs to visit.
         * @param ctx The context defining constraints, demands, and historical profit.
         * @return types::DaySolution The evaluated routes and inventory metrics.
         */
        types::DaySolution decode(
            const std::vector<size_t> &sequence,
            const types::DayContext &ctx) const override;
    };
}