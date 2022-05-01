/**
 * @file VideoManager.hpp
 * @author Junaid Afzal
 * @brief Handles all frame related functions and
 * thus manages all other classes including the detectors
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"
#include "detectors/ObjectDetector.hpp"
#include "detectors/LaneDetector.hpp"
#include "helpers/Performance.hpp"
#include "helpers/Enumerations.hpp"

class VideoManager
{
public:
    explicit VideoManager(const std::string &InputVideoPath,
                          const Enumerations::Detector &ObjectDetectorType = Enumerations::Detector::STANDARD,
                          const Enumerations::BackEnd &BackEndType = Enumerations::BackEnd::CUDA,
                          const Enumerations::BlobSize &BlobSize = Enumerations::BlobSize::FIVE,
                          const bool &RecordFrameTimes = false);

    ~VideoManager();

    std::optional<std::vector<int>> Run();

private:
    cv::VideoCapture m_InputVideo;
    cv::VideoWriter m_OutputVideo;
    cv::Mat m_Frame, m_UneditedFrame;
    bool m_RecordOutput = false, m_RecordFrameTimes{};

    Performance m_Perf;
    ObjectDetector m_Obj;
    LaneDetector m_Lane;

    const int m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
    const cv::Rect m_RECORDING_STATUS_RECT = cv::Rect(1495, 410, 400, 50);

    static bool String_Is_Integer(const std::string &Input);

    void Generate_Next_Frame();

    void Process_Current_Frame();

    void Print_Recording_Status_To_Frame();

    void Toggle_Recording();
};
