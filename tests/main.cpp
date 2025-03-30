#include <string>
#include <vector>

#include "helpers/PerformanceTest.hpp"

int main(int argc, char* argv[])
{
    LaneAndObjectDetection::PerformanceTest performanceTests(std::vector<std::string>(argv, argv + argc));
    performanceTests.RunPerformanceTest();
}
