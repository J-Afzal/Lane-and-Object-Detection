/**
 * @file Performance.hpp
 * @author Junaid Afzal
 * @brief This class calculates the current FPS, average FPS,
 * and time to compute each frame (frame time)
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"

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
    void Start_Timer();

    /**
     * @brief TODO
     *
     */
    void Stop_Timer();

    /**
     * @brief TODO
     *
     */
    void Print_FPS_To_Frame(cv::Mat &Frame);

    /**
     * @brief TODO
     *
     */
    std::vector<int> Get_Frame_Times();

private:
    /**
     * @brief TODO
     */
    ///@{
    std::vector<int> m_FrameTimes;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
    unsigned long long m_FrameCount = 0;
    double m_CurrentFPS = 0, m_AverageFPS = 0;

    const int m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
    const cv::Rect m_FPS_RECT = cv::Rect(25, 25, 350, 100);
    ///@}
};
