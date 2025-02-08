#include "helpers/VideoManager.hpp"

int main()
{
    VideoManager videoManager("../resources/motorway.mp4", "../resources/yolo/");
    videoManager.Run();
}
