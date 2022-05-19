/**
 * @file main.cpp
 * @author Junaid Afzal
 * @brief Lane and object detection for use in autonomous vehicles using OpenCV
 * and YOLOv4. The software is run by instantiating the VideoManger class with a
 * string to the input video file and yolo resources folder and then calling it's
 * Run() method. If a camera input is desired, then pass the appropriate integer,
 * as required by OpenCV, as a string.
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/VideoManager.hpp"

int main()
{
    VideoManager VideoManager("../../resources/motorway.mp4", "../../resources/yolo/");
    VideoManager.Run();
}
