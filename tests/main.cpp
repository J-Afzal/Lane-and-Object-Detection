#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "helpers/Globals.hpp"
#include "helpers/VideoManager.hpp"

static void ParseCommandLineArguments(const std::vector<std::string>& p_commandLineArguments, std::string& p_inputVideoFilePath, std::string& p_yoloFolderPath, uint32_t& p_numberOfRepetitions)
{
    uint32_t index = 0;

    bool numberOfRepetitionsGiven = false;

    for (const std::string& argument : p_commandLineArguments)
    {
        if (argument == "-h" || argument == "--help")
        {
            std::cout << LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
            exit(1);
        }

        try
        {
            if (argument == "-i" || argument == "--input")
            {
                p_inputVideoFilePath = p_commandLineArguments.at(index + 1);
            }

            if (argument == "-y" || argument == "--yolo-folder-path")
            {
                p_yoloFolderPath = p_commandLineArguments.at(index + 1);
            }

            if (argument == "-r" || argument == "--repetitions")
            {
                p_numberOfRepetitions = std::stoi(p_commandLineArguments.at(index + 1));
                numberOfRepetitionsGiven = true;
            }
        }

        catch (...)
        {
            std::cout << LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
            exit(1);
        }

        index++;
    }

    // Check that the required arguments have been provided
    if (p_inputVideoFilePath.empty() || p_yoloFolderPath.empty() || !numberOfRepetitionsGiven)
    {
        std::cout << LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    try
    {
        LaneAndObjectDetection::VideoManager videoManager;
        std::string inputVideoFilePath;
        std::string yoloFolderPath;
        uint32_t numberOfRepetitions = 0;

        ParseCommandLineArguments(std::vector<std::string>(argv, argv + argc), inputVideoFilePath, yoloFolderPath, numberOfRepetitions);

        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";
        std::cout << "\n\nSettings:";
        std::cout << "\n    Number of tests: " << LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS;
        std::cout << "\n    Number of repetitions: " << numberOfRepetitions;

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        for (uint32_t currentTestNumber = 0; currentTestNumber < LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS; currentTestNumber++)
        {
            std::cout << std::format("\n\n######## {} ({}/{}) ########",
                                     LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_OUTPUT_FILE_BASE_NAMES.at(currentTestNumber),
                                     currentTestNumber,
                                     LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS);

            for (uint32_t currentRepetition = 0; currentRepetition < numberOfRepetitions; currentRepetition++)
            {
                videoManager.SetProperties(inputVideoFilePath,
                                           yoloFolderPath,
                                           LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES.at(currentTestNumber),
                                           LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_BACK_END_TYPES.at(currentTestNumber),
                                           LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_BLOB_SIZES.at(currentTestNumber));

                videoManager.Run();

                std::cout << std::format("\n    Finished repetition {}/{}!", currentRepetition + 1, numberOfRepetitions);

                const std::string OUTPUT_FILE_NAME = LaneAndObjectDetection::Globals::G_PERFORMANCE_TESTS_OUTPUT_FILE_BASE_NAMES.at(currentTestNumber) + "-" + std::to_string(currentRepetition) + ".txt";
                std::ofstream outputFile(OUTPUT_FILE_NAME);

                if (!outputFile.is_open())
                {
                    std::cout << "\nERROR: Output file '" + OUTPUT_FILE_NAME + "' failed to open!\n";
                    exit(1);
                }

                for (const uint32_t& frameTime : videoManager.GetPerformanceInformation().m_frameTimes)
                {
                    outputFile << frameTime << '\n';
                }
            }
        }

        // Calculate total time taken in hrs mins secs
        const uint32_t TOTAL_TIME_IN_SECONDS = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - START_TIME).count();

        const double SECONDS_IN_HOUR = 3600;
        const double MINUTES_IN_HOUR = 60;
        const double SECONDS_IN_MINUTES = 60;

        const double HOURS = TOTAL_TIME_IN_SECONDS / SECONDS_IN_HOUR;
        const double MINUTES = (HOURS - floor(HOURS)) * MINUTES_IN_HOUR;
        const double SECONDS = (MINUTES - floor(MINUTES)) * SECONDS_IN_MINUTES;

        std::cout << std::format("\n\nTotal elapsed time = {} hrs {} mins {} secs", floor(HOURS), floor(MINUTES), floor(SECONDS));
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";

        return 0;
    }

    catch (...)
    {
        exit(1);
    }
}
