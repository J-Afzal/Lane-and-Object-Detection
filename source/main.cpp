#include <string>
#include <vector>

#include "helpers/VideoManager.hpp"

int main(int argc, char* argv[])
{
    LaneAndObjectDetection::VideoManager videoManager(std::vector<std::string>(argv, argv + argc));
    videoManager.RunLaneAndObjectDetector();
}
