#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class Performance
     * @brief Calculates the frame times and current/average frames per second for real-time performance tracking.
     */
    class Performance
    {
    public:
        /**
         * @brief Constructs a new %Performance object.
         */
        explicit Performance();

        /**
         * @brief Start the internal timer.
         */
        void StartTimer();

        /**
         * @brief End the internal timer.
         */
        void EndTimer();

        /**
         * @brief Get the PerformanceInformation struct.
         *
         * @return PerformanceInformation The PerformanceInformation struct.
         */
        PerformanceInformation GetInformation();

        /**
         * @brief Get the times to compute each and every frame.Globals
         *
         * @return `std::vector<uint32_t>` A list of the time to compute each frame.
         */
        std::vector<uint32_t> GetFrameTimes();

    private:
        /**
         * @brief The PerformanceInformation struct containing all performance-related information.
         */
        PerformanceInformation m_performanceInformation;

        /**
         * @brief The frame times that have been measured.
         */
        std::vector<uint32_t> m_frameTimes;

        /**
         * @brief The time the internal timer was started within `StartTimer()`.
         */
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
    };
}
