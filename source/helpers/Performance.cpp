// NOLINTBEGIN

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "helpers/Performance.hpp"

namespace LaneAndObjectDetection
{
    void Performance::StartTimer()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    void Performance::StopTimer()
    {
        m_frameTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime).count());
        m_currentFps = 1000 / static_cast<double>(m_frameTimes.back());

        // Calculate Average FPS
        m_frameCount++;
        if (m_frameCount != 1)
        {
            m_averageFps = (m_averageFps * static_cast<double>(m_frameCount - 2) + m_currentFps) / static_cast<double>(m_frameCount - 1);
        }
    }

    void Performance::PrintFpsToFrame(cv::Mat& p_frame)
    {
        // Display the previous average FPS and previous frame FPS
        cv::rectangle(p_frame, m_FPS_RECT, cv::Scalar(0), cv::FILLED);

        std::stringstream ss1;
        ss1 << std::fixed << std::setprecision(2) << m_averageFps;
        std::string fpsText = "Average FPS: " + ss1.str();
        // The next four lines are used to center the text horizontally and vertically
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(fpsText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        cv::Point textOrg = cv::Point(30, 25 + baseline + textSize.height);
        cv::putText(p_frame, fpsText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);

        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(2) << m_currentFps;
        fpsText = "Current FPS: " + ss2.str();
        // The next four lines are used to center the text horizontally and vertically
        baseline = 0;
        textSize = cv::getTextSize(fpsText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        textOrg = cv::Point(30, 75 + baseline + textSize.height);
        cv::putText(p_frame, fpsText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
    }

    std::vector<uint32_t> Performance::GetFrameTimes()
    {
        return m_frameTimes;
    }
}

// NOLINTEND
