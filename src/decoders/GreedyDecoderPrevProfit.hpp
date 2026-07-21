#pragma once

#include "DecoderInterface.hpp"

namespace routing
{
    /**
     * @class GreedyDecoderPrevProfit
     * @brief A greedy two-pass decoder that incorporates historical multi-period profit.
     *
     * This decoder operates identically to the standard GreedyDecoder (prioritizing
     * demand fulfillment first, then using leftover shift time to maximize capacity).
     * However, it specifically injects the accumulated profit from previous days
     * (`ctx.prev_utilities`) into the final evaluated `DaySolution`.
     *
     * Use this decoder when your overarching multi-period algorithm (like Beam Search)
     * evaluates node fitness based on the global accumulated utility rather than just
     * the isolated performance of the current day.
     */
    class GreedyDecoderPrevProfit : public DecoderInterface
    {
    public:
        /**
         * @brief Decodes the sequence and calculates total multi-period utility.
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