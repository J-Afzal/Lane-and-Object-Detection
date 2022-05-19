/**
 * @file PerformanceTests.cpp
 * @author Junaid Afzal
 * @brief Implementation of PerformanceTests.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/PerformanceTests.hpp"
#include "helpers/VideoManager.hpp"

PerformanceTests::PerformanceTests(std::string InputVideoFilePath, std::string YoloResourcesFolderPath) : m_InputVideoFilePath(std::move(InputVideoFilePath)), m_YoloResourcesFolderPath(std::move(YoloResourcesFolderPath)) { }

void PerformanceTests::Run()
{
    std::cout << "\n\nNumber of tests: " << m_NUMBER_OF_TESTS << "\nNumber of repetitions: " << m_NUMBER_OF_REPETITIONS << '\n';

    m_StartTime = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < m_NUMBER_OF_TESTS; i++)
    {
        m_FrameTimes.clear();

        for (int j = 0; j < m_NUMBER_OF_REPETITIONS; j++)
        {
            VideoManager Vid(m_InputVideoFilePath, m_YoloResourcesFolderPath, m_YOLO_TYPES[i], m_BACK_END_TYPES[i], m_BLOB_SIZES[i]);
            m_FrameTimes.push_back(Vid.Run());

            std::cout << m_OUTPUT_FILE_NAMES[i] << ": Finished iteration " << j + 1 << "/" << m_NUMBER_OF_REPETITIONS << " for test " << i + 1 << "/" << m_NUMBER_OF_TESTS << '\n';
        }

        std::ofstream OutputFile(m_OUTPUT_FILE_NAMES[i]);
        if (!OutputFile.is_open())
        {
            std::cout << "\nError opening output file: " + m_OUTPUT_FILE_NAMES[i] + '\n';
            exit(1);
        }

        for (int j = 0; j < m_FrameTimes[0]->size(); j++)
        {
            int FrameTimeSum = 0;
            for (const auto &FrameTime : m_FrameTimes)
                FrameTimeSum += FrameTime->at(j);

            OutputFile << std::to_string(FrameTimeSum / (double)m_NUMBER_OF_REPETITIONS) << '\n';
        }
    }

    // Calculate total time taken in hrs mins secs
    int total = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count();
    double hours = total / 3600.0;
    double minutes = (hours - floor(hours)) * 60;
    double seconds = (minutes - floor(minutes)) * 60;

    hours = floor(hours);
    minutes = floor(minutes);
    seconds = floor(seconds);

    std::cout << "\n\nAll test complete!\nTotal time taken = " << hours << "hrs " << minutes << "mins " << seconds << "secs\n\n";
}
