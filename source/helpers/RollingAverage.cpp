// NOLINTBEGIN

#include "helpers/RollingAverage.hpp"
#include <cstdint>

namespace LaneAndObjectDetection
{
    RollingAverage::RollingAverage(const uint32_t& p_sizeOfRollingAverage, const uint32_t& p_numberOfStates)
    {
        for (uint32_t i = 0; i < p_sizeOfRollingAverage; i++)
        {
            m_rollingAverageArray.push_back(0);
        }

        for (uint32_t i = 0; i < p_numberOfStates; i++)
        {
            m_occurrenceOfEachState.push_back(0);
        }

        m_occurrenceOfEachState[0] = p_sizeOfRollingAverage;
    }

    uint32_t RollingAverage::CalculateRollingAverage(const uint32_t& p_nextInput)
    {
        m_occurrenceOfEachState[m_rollingAverageArray.back()]--;
        m_rollingAverageArray.pop_back();
        m_rollingAverageArray.push_front(p_nextInput);
        m_occurrenceOfEachState[p_nextInput]++;

        uint32_t mostFrequentState = 0;
        for (uint32_t i = 1; i < m_occurrenceOfEachState.size(); i++)
        {
            if (m_occurrenceOfEachState[i] > m_occurrenceOfEachState[mostFrequentState])
            {
                mostFrequentState = i;
            }
        }

        return mostFrequentState;
    }
}

// NOLINTEND
