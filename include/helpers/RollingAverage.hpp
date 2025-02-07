/**
 * @file RollingAverage.hpp
 * @author Junaid Afzal
 * @brief This class represents a rolling average
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"

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
    RollingAverage(const int &SizeOfRollingAverage, const int &NumberOfStates);

    /**
     * @brief TODO
     */
    ~RollingAverage() = default;

    /**
     * @brief TODO
     */
    int calculateRollingAverage(const int &NextInput);

private:

    /**
     * @brief TODO
     */
    std::deque<int> RollingAverageArray;

    /**
     * @brief TODO
     */
    std::vector<int> OccurrenceOfEachState;
};
