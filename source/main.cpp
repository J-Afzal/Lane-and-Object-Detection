/**
 * @file main.cpp
 * @author Junaid Afzal
 * @brief Executes lane and object detection
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/VideoManager.hpp"

int main()
{
    VideoManager VideoManager("../../resources/motorway.mp4");
    VideoManager.Run();
}
