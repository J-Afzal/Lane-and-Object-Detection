#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include <opencv2/core/mat.hpp>

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
 */
namespace LaneAndObjectDetection
{
    /**
     * @class Performance
     * @brief TODO
     */
    class Performance
    {
    public:
        /**
         * @brief TODO
         *
         */
        explicit Performance();

        /**
         * @brief TODO
         *
         */
        void StartTimer();

        /**
         * @brief TODO
         *
         */
        void StopTimer();

        /**
         * @brief TODO
         *
         */
        void PrintFpsToFrame(cv::Mat& p_frame) const;

        /**
         * @brief TODO
         *
         */
        std::vector<uint32_t> GetFrameTimes();

    private:
        /**
         * @brief TODO
         */
        ///@{
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        std::vector<uint32_t> m_frameTimes;
        uint64_t m_frameCount;
        double m_averageFps;
        double m_currentFps;
        ///@}
    };
}
