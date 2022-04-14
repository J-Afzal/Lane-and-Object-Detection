/**
 * @file RollingAverage.cpp
 * @author Junaid Afzal
 * @brief Implementation of RollingAverage.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/RollingAverage.hpp"

RollingAverage::RollingAverage(const int &SizeOfRollingAverage, const int &NumberOfStates)
{
    for (unsigned int i = 0; i < SizeOfRollingAverage; i++)
        RollingAverageArray.push_back(0);

    for (unsigned int i = 0; i < NumberOfStates; i++)
        OccurrenceOfEachState.push_back(0);

    OccurrenceOfEachState[0] = SizeOfRollingAverage;
}

int RollingAverage::calculateRollingAverage(const int &nextInput)
{
    OccurrenceOfEachState[RollingAverageArray.back()]--;
    RollingAverageArray.pop_back();
    RollingAverageArray.push_front(nextInput);
    OccurrenceOfEachState[nextInput]++;

    int mostFrequentState = 0;
    for (int i = 1; i < OccurrenceOfEachState.size(); i++)
        if (OccurrenceOfEachState[i] > OccurrenceOfEachState[mostFrequentState])
            mostFrequentState = i;

    return mostFrequentState;
}
