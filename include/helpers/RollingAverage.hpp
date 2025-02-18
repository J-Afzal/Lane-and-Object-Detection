#pragma once

#include <cstdint>
#include <deque>
#include <vector>

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class RollingAverage
     * @brief TODO
     */
    class RollingAverage // TODO: Move to be within lane detection somehow?
    {
    public:
        /**
         * @brief TODO
         */
        explicit RollingAverage() = default;

        /**
         * @brief TODO
         */
        explicit RollingAverage(const uint32_t& p_sizeOfRollingAverage, const uint32_t& p_numberOfStates);

        /**
         * @brief TODO
         */
        uint32_t CalculateRollingAverage(const uint32_t& p_nextInput);

    private:
        /**
         * @brief TODO
         */
        std::deque<uint32_t> m_rollingAverageArray;

        /**
         * @brief TODO
         */
        std::vector<uint32_t> m_occurrenceOfEachState;
    };
}
