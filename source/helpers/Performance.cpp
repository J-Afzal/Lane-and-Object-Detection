/**
 * @file Performance.cpp
 * @author Junaid Afzal
 * @brief Implementation of Performance.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "helpers/Performance.hpp"

void Performance::Start_Timer() { m_StartTime = std::chrono::high_resolution_clock::now(); }

void Performance::Stop_Timer()
{
    m_FrameTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count());
    m_CurrentFPS = 1000 / (double)m_FrameTimes.back();

    // Calculate Average FPS
    m_FrameCount++;
    if (m_FrameCount != 1)
        m_AverageFPS = (m_AverageFPS * (double)(m_FrameCount - 2) + m_CurrentFPS) / (double)(m_FrameCount - 1);
}

void Performance::Print_FPS_To_Frame(cv::Mat &Frame)
{
    // Display the previous average FPS and previous frame FPS
    cv::rectangle(Frame, m_FPS_RECT, cv::Scalar(0), cv::FILLED);

    std::stringstream ss1;
    ss1 << std::fixed << std::setprecision(2) << m_AverageFPS;
    std::string FPSText = "Average FPS: " + ss1.str();
    // The next four lines are used to center the text horizontally and vertically
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(FPSText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
    baseline += m_FONT_THICKNESS;
    cv::Point textOrg = cv::Point(30, 25 + baseline + textSize.height);
    cv::putText(Frame, FPSText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);

    std::stringstream ss2;
    ss2 << std::fixed << std::setprecision(2) << m_CurrentFPS;
    FPSText = "Current FPS: " + ss2.str();
    // The next four lines are used to center the text horizontally and vertically
    baseline = 0;
    textSize = cv::getTextSize(FPSText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
    baseline += m_FONT_THICKNESS;
    textOrg = cv::Point(30, 75 + baseline + textSize.height);
    cv::putText(Frame, FPSText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
}

std::vector<int> Performance::Get_Frame_Times() { return m_FrameTimes; }
