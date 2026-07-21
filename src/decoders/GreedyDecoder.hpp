#pragma once

#include "DecoderInterface.hpp"

namespace routing
{
    /**
     * @class GreedyDecoder
     * @brief A heuristic decoder that builds routes using a two-pass greedy strategy.
     *
     * This decoder iterates through the provided client sequence and builds worker routes sequentially:
     * - **Pass 1 (Fulfill Demand):** Attempts to restock products up to the client's current demand
     *   without exceeding the worker's maximum time limit (L). If a worker runs out of time,
     *   the remaining sequence is passed to the next worker.
     * - **Pass 2 (Maximize Capacity):** Once the base sequence is routed, the decoder iterates back
     *   over the established routes. If a worker has leftover shift time, they use it to restock
     *   additional products up to the client's absolute maximum shelf capacity.
     *
     * Finally, it calculates all end-of-day inventories, missing product penalties, and the final
     * utility score.
     */
    class GreedyDecoder : public DecoderInterface
    {
    public:
        /**
         * @brief Decodes the sequence using the greedy two-pass heuristic.
         *
         * @param sequence The ordered list of client IDs to visit.
         * @param ctx The context defining the constraints and demands for the current day.
         * @return types::DaySolution The evaluated routes and inventory metrics.
         */
        types::DaySolution decode(
            const std::vector<size_t> &sequence,
            const types::DayContext &ctx) const override;
    };
}