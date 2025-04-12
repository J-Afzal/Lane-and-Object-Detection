#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "helpers/Globals.hpp"
#include "helpers/PerformanceTest.hpp"
#include "helpers/VideoManager.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    PerformanceTest::PerformanceTest(std::string& p_inputVideoFilePath, std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions) :
        m_inputVideoFilePath(std::move(p_inputVideoFilePath)),
        m_yoloFolderPath(std::move(p_yoloFolderPath)),
        m_numberOfRepetitions(p_numberOfRepetitions)
    {}

    PerformanceTest::PerformanceTest(const std::vector<std::string>& p_commandLineArguments)
    {
        uint32_t index = 0;

        bool numberOfRepetitionsGiven = false;

        for (const std::string& argument : p_commandLineArguments)
        {
            if (argument == "-h" || argument == "--help")
            {
                std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            try
            {
                if (argument == "-i" || argument == "--input")
                {
                    m_inputVideoFilePath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-y" || argument == "--yolo-folder-path")
                {
                    m_yoloFolderPath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-r" || argument == "--repetitions")
                {
                    m_numberOfRepetitions = std::stoi(p_commandLineArguments.at(index + 1));
                    numberOfRepetitionsGiven = true;
                }
            }

            catch (...)
            {
                std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            index++;
        }

        // Check that the required arguments have been provided
        if (m_inputVideoFilePath.empty() || m_yoloFolderPath.empty() || !numberOfRepetitionsGiven)
        {
            std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
            std::exit(1);
        }
    }

    void PerformanceTest::RunPerformanceTest()
    {
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";
        std::cout << "\n\nSettings:";
        std::cout << "\n    Number of tests: " << Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS;
        std::cout << "\n    Number of repetitions: " << m_numberOfRepetitions;

        VideoManager videoManager;

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        for (uint32_t currentTestNumber = 0; currentTestNumber < Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS; currentTestNumber++)
        {
            std::cout << std::format("\n\n######## {} ({}/{}) ########",
                                     Globals::G_PERFORMANCE_TESTS_OUTPUT_FILE_BASE_NAMES.at(currentTestNumber),
                                     currentTestNumber,
                                     Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS);

            for (uint32_t currentRepetition = 0; currentRepetition < m_numberOfRepetitions; currentRepetition++)
            {
                videoManager.SetProperties(m_inputVideoFilePath,
                                           m_yoloFolderPath,
                                           Globals::G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES.at(currentTestNumber),
                                           Globals::G_PERFORMANCE_TESTS_BACK_END_TYPES.at(currentTestNumber),
                                           Globals::G_PERFORMANCE_TESTS_BLOB_SIZES.at(currentTestNumber));

                videoManager.RunLaneAndObjectDetector();

                std::cout << std::format("\n    Finished repetition {}/{}!", currentRepetition + 1, m_numberOfRepetitions);

                const std::string OUTPUT_FILE_NAME = Globals::G_PERFORMANCE_TESTS_OUTPUT_FILE_BASE_NAMES.at(currentTestNumber) + "-" + std::to_string(currentRepetition) + ".txt";
                std::ofstream outputFile(OUTPUT_FILE_NAME);

                if (!outputFile.is_open())
                {
                    std::cout << "\nERROR: Output file '" + OUTPUT_FILE_NAME + "' failed to open!\n";
                    std::exit(1);
                }

                for (const uint32_t& frameTime : videoManager.GetPerformanceInformation().m_frameTimes)
                {
                    outputFile << frameTime << '\n';
                }
            }
        }

        std::cout << std::format("\n\nTotal elapsed time = {} (H?:mm:ss)", Globals::GetTimeElapsed(START_TIME));
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";
    }
}
