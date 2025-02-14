// NOLINTBEGIN

#pragma once

#include <cstdint>
#include <deque>
#include <vector>

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
 */
namespace LaneAndObjectDetection
{
    /**
     * @class RollingAverage
     * @brief TODO
     */
    class RollingAverage
    {
    public:
        /**
         * @brief TODO
         */
        RollingAverage(const uint32_t& p_sizeOfRollingAverage, const uint32_t& p_numberOfStates);

        /**
         * @brief TODO
         */
        ~RollingAverage() = default;

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

// NOLINTEND
