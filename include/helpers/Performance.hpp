// NOLINTBEGIN

#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

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
        Performance() = default;

        /**
         * @brief TODO
         *
         */
        ~Performance() = default;

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
        void PrintFpsToFrame(cv::Mat& p_frame);

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
        std::vector<uint32_t> m_frameTimes;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        unsigned long long m_frameCount = 0;
        double m_currentFps = 0, m_averageFps = 0;

        const uint32_t m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
        const cv::Rect m_FPS_RECT = cv::Rect(25, 25, 350, 100);
        ///@}
    };
}

// NOLINTEND
