#include "helpers/PerformanceTests.hpp"

int main()
{
    PerformanceTests performanceTests("../../resources/benchmark.mp4", "../../resources/yolo/");
    performanceTests.Run();
}
