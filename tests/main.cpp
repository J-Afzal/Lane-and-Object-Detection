#include <string>
#include <vector>

#include "PerformanceTests.hpp"

int main(int argc, char* argv[])
{
    LaneAndObjectDetection::PerformanceTests performanceTests(std::vector<std::string>(argv, argv + argc));
    performanceTests.RunPerformanceTests();
}
