// NOLINTBEGIN

#include "helpers/VideoManager.hpp"

int main()
{
    LaneAndObjectDetection::VideoManager videoManager("../resources/motorway.mp4", "../resources/yolo/");
    videoManager.Run();
}

// NOLINTEND
