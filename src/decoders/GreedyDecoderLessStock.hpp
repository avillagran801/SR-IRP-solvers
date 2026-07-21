#pragma once

#include "DecoderInterface.hpp"

namespace routing
{
    /**
     * @class GreedyDecoderLessStock
     * @brief A greedy decoder that prioritizes products closest to a stock-out and incorporates historical profit.
     *
     * Instead of evaluating products in their default ID order, this decoder dynamically
     * sorts a client's products based on their projected end-of-day inventory level
     * (Start Inventory - Demand). Products that are projected to have the least stock
     * (or the largest negative deficit) are restocked first.
     *
     * Additionally, like the PrevProfit decoder, it injects the accumulated profit from
     * previous days (`ctx.prev_utilities`) into the final evaluated `DaySolution`, making it
     * ideal for multi-period evaluations where both long-term utility and immediate shortage
     * prevention are critical.
     */
    class GreedyDecoderLessStock : public DecoderInterface
    {
    public:
        /**
         * @brief Decodes the sequence using a lowest-stock-first heuristic and calculates total multi-period utility.
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