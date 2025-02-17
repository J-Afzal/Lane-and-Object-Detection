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

#include "helpers/Globals.hpp"
#include "helpers/Performance.hpp"

namespace LaneAndObjectDetection
{
    Performance::Performance() :
        m_frameCount(0),
        m_averageFps(0),
        m_currentFps(0) {}

    void Performance::StartTimer()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    void Performance::StopTimer()
    {
        const uint32_t MILLISECONDS_IN_SECOND = 1000;

        m_frameTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_startTime).count());
        m_currentFps = MILLISECONDS_IN_SECOND / static_cast<double>(m_frameTimes.back());

        // Calculate Average FPS
        m_frameCount++;
        if (m_frameCount > 1)
        {
            m_averageFps = (m_averageFps * static_cast<double>(m_frameCount - 2) + m_currentFps) / static_cast<double>(m_frameCount - 1);
        }
    }

    void Performance::PrintFpsToFrame(cv::Mat& p_frame) const
    {
        const int32_t TEXT_HORIZONTAL_START = 30;
        const int32_t TEXT_VERTICAL_START = 25;
        const int32_t TEXT_VERTICAL_OFFSET = 50;

        // Display the previous average FPS and previous frame FPS
        cv::rectangle(p_frame, Globals::G_FPS_RECT, Globals::G_OPENCV_BLACK, cv::FILLED);

        std::stringstream ss1;
        ss1 << std::fixed << std::setprecision(2) << m_averageFps;
        std::string fpsText = "Average FPS: " + ss1.str();
        // The next four lines are used to center the text horizontally and vertically
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(fpsText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
        baseline += Globals::G_DEFAULT_FONT_THICKNESS;
        cv::Point textOrg = cv::Point(TEXT_HORIZONTAL_START, TEXT_VERTICAL_START + baseline + textSize.height);
        cv::putText(p_frame, fpsText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);

        std::stringstream ss2;
        ss2 << std::fixed << std::setprecision(2) << m_currentFps;
        fpsText = "Current FPS: " + ss2.str();
        // The next four lines are used to center the text horizontally and vertically
        baseline = 0;
        textSize = cv::getTextSize(fpsText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
        baseline += Globals::G_DEFAULT_FONT_THICKNESS;
        textOrg = cv::Point(TEXT_HORIZONTAL_START, TEXT_VERTICAL_OFFSET + TEXT_VERTICAL_START + baseline + textSize.height);
        cv::putText(p_frame, fpsText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);
    }

    std::vector<uint32_t> Performance::GetFrameTimes()
    {
        return m_frameTimes;
    }
}
