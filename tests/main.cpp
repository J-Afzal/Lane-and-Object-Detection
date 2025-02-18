#include <chrono>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "helpers/Globals.hpp"
#include "helpers/VideoManager.hpp"

int main() // TODO: CLI args to parse input file paths and number of repetitions and then clean up rest of function.
{
    try
    {
        const std::string INPUT_VIDEO_FILE_PATH = "../../resources/benchmark.mp4";
        const std::string YOLO_RESOURCES_FOLDER_PATH = "../../resources/yolo/";

        std::cout << "\n\nNumber of tests: " << LaneAndObjectDetection::Globals::G_NUMBER_OF_TESTS
                  << "\nNumber of repetitions: " << LaneAndObjectDetection::Globals::G_NUMBER_OF_REPETITIONS << '\n';

        LaneAndObjectDetection::VideoManager videoManager;
        std::vector<std::vector<uint32_t>> frameTimes;

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        for (uint32_t i = 0; i < LaneAndObjectDetection::Globals::G_NUMBER_OF_TESTS; i++)
        {
            frameTimes.clear();

            for (uint32_t j = 0; j < LaneAndObjectDetection::Globals::G_NUMBER_OF_REPETITIONS; j++)
            {
                videoManager.SetProperties(INPUT_VIDEO_FILE_PATH,
                                           YOLO_RESOURCES_FOLDER_PATH,
                                           LaneAndObjectDetection::Globals::G_YOLO_TYPES.at(i),
                                           LaneAndObjectDetection::Globals::G_BACK_END_TYPES.at(i),
                                           LaneAndObjectDetection::Globals::G_BLOB_SIZES.at(i));

                videoManager.Run();

                frameTimes.push_back(videoManager.GetFrameTimes());

                std::cout << LaneAndObjectDetection::Globals::G_OUTPUT_FILE_NAMES.at(i)
                          << ": Finished iteration " << j + 1 << "/" << LaneAndObjectDetection::Globals::G_NUMBER_OF_REPETITIONS
                          << " for test " << i + 1 << "/" << LaneAndObjectDetection::Globals::G_NUMBER_OF_TESTS << '\n';
            }

            std::ofstream outputFile(LaneAndObjectDetection::Globals::G_OUTPUT_FILE_NAMES.at(i));

            if (!outputFile.is_open())
            {
                std::cout << "\nERROR: Output file '" + LaneAndObjectDetection::Globals::G_OUTPUT_FILE_NAMES.at(i) + "' failed to open!\n";
                exit(1);
            }

            for (uint32_t j = 0; j < frameTimes[0].size(); j++)
            {
                uint32_t frameTimeSum = 0;
                for (const auto& frameTime : frameTimes)
                {
                    frameTimeSum += frameTime.at(j);
                }

                outputFile << std::to_string(frameTimeSum / static_cast<double>(LaneAndObjectDetection::Globals::G_NUMBER_OF_REPETITIONS)) << '\n';
            }
        }

        // Calculate total time taken in hrs mins secs
        const double SECONDS_IN_HOUR = 3600;
        const double MINUTES_IN_HOUR = 60;
        const double SECONDS_IN_MINUTES = 60;

        const uint32_t TOTAL = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - START_TIME).count();
        const double HOURS = TOTAL / SECONDS_IN_HOUR;
        const double MINUTES = (HOURS - floor(HOURS)) * MINUTES_IN_HOUR;
        const double SECONDS = (MINUTES - floor(MINUTES)) * SECONDS_IN_MINUTES;

        std::cout << "\n\nAll test complete!\nTotal time taken = " << floor(HOURS) << "hrs " << floor(MINUTES) << "mins " << floor(SECONDS) << "secs\n\n";

        return 0;
    }

    catch (...)
    {
        exit(1);
    }
}
