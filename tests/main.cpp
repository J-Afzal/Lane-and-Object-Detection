// NOLINTBEGIN

#include "helpers/PerformanceTests.hpp"

int main()
{
    LaneAndObjectDetection::PerformanceTests performanceTests("../../resources/benchmark.mp4", "../../resources/yolo/");
    performanceTests.Run();
}

// NOLINTEND
