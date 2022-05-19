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

class RollingAverage
{
public:
    RollingAverage(const int &SizeOfRollingAverage, const int &NumberOfStates);

    ~RollingAverage() = default;

    int calculateRollingAverage(const int &NextInput);

private:
    std::deque<int> RollingAverageArray;
    std::vector<int> OccurrenceOfEachState;
};
