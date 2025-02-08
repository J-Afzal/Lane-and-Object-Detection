// NOLINTBEGIN

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

#include "helpers/PerformanceTests.hpp"
#include "helpers/VideoManager.hpp"

namespace LaneAndObjectDetection
{
    PerformanceTests::PerformanceTests(std::string p_inputVideoFilePath, std::string p_yoloResourcesFolderPath) :
        m_inputVideoFilePath(std::move(p_inputVideoFilePath)),
        m_yoloResourcesFolderPath(std::move(p_yoloResourcesFolderPath)) {}

    void PerformanceTests::Run()
    {
        std::cout << "\n\nNumber of tests: " << m_NUMBER_OF_TESTS << "\nNumber of repetitions: " << m_NUMBER_OF_REPETITIONS << '\n';

        m_startTime = std::chrono::high_resolution_clock::now();

        for (uint32_t i = 0; i < m_NUMBER_OF_TESTS; i++)
        {
            m_frameTimes.clear();

            for (uint32_t j = 0; j < m_NUMBER_OF_REPETITIONS; j++)
            {
                VideoManager vid(m_inputVideoFilePath, m_yoloResourcesFolderPath, m_YOLO_TYPES[i], m_BACK_END_TYPES[i], m_BLOB_SIZES[i]);
                m_frameTimes.push_back(vid.Run());

                std::cout << m_OUTPUT_FILE_NAMES[i] << ": Finished iteration " << j + 1 << "/" << m_NUMBER_OF_REPETITIONS << " for test " << i + 1 << "/" << m_NUMBER_OF_TESTS << '\n';
            }

            std::ofstream outputFile(m_OUTPUT_FILE_NAMES[i]);
            if (!outputFile.is_open())
            {
                std::cout << "\nError opening output file: " + m_OUTPUT_FILE_NAMES[i] + '\n';
                exit(1);
            }

            for (uint32_t j = 0; j < m_frameTimes[0]->size(); j++)
            {
                uint32_t frameTimeSum = 0;
                for (const auto& frameTime : m_frameTimes)
                {
                    frameTimeSum += frameTime->at(j);
                }

                outputFile << std::to_string(frameTimeSum / static_cast<double>(m_NUMBER_OF_REPETITIONS)) << '\n';
            }
        }

        // Calculate total time taken in hrs mins secs
        const uint32_t TOTAL = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_startTime).count();
        double hours = TOTAL / 3600.0;
        double minutes = (hours - floor(hours)) * 60;
        double seconds = (minutes - floor(minutes)) * 60;

        hours = floor(hours);
        minutes = floor(minutes);
        seconds = floor(seconds);

        std::cout << "\n\nAll test complete!\nTotal time taken = " << hours << "hrs " << minutes << "mins " << seconds << "secs\n\n";
    }
}

// NOLINTEND
