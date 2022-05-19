/**
 * @file main.cpp
 * @author Junaid Afzal
 * @brief This executes the performance tests.
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/PerformanceTests.hpp"

int main()
{
    PerformanceTests PerformanceTests("../../resources/benchmark.mp4", "../../resources/yolo/");
    PerformanceTests.Run();
}
