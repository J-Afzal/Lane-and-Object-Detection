/**
 * @file VideoManager.hpp
 * @author Junaid Afzal
 * @brief This class handles all video related
 * tasks and thus manages all other classes, including
 * detectors, and contains the main while loop
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

/**
 * @class VideoManager
 * @brief TODO
 */
class VideoManager
{
public:
    /**
     * @brief TODO
     */
    explicit VideoManager(const std::string &InputVideoFilePath,
                          const std::string &YoloResourcesFolderPath,
                          const Detector &ObjectDetectorType = Detector::STANDARD,
                          const BackEnd &BackEndType = BackEnd::CPU,
                          const BlobSize &BlobSize = BlobSize::FIVE);

    /**
     * @brief TODO
     */
    ~VideoManager();

    /**
     * @brief An std::optional is used so that the PerformanceTests class can extract frame times from
     * the VideoManager class, when needed, while not effecting the usage of VideoManager
     * outside of performance testing
     */
    std::optional<std::vector<int>> Run();

private:
    /**
     * @brief TODO
     */
    ///@{
    cv::VideoCapture m_InputVideo;
    cv::VideoWriter m_OutputVideo;
    cv::Mat m_Frame;
    bool m_RecordOutput = false;
    std::unique_ptr<ObjectDetector> m_ObjectDetector;
    std::unique_ptr<LaneDetector> m_LaneDetector;
    std::unique_ptr<Performance> m_Performance;

    const int m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
    const cv::Rect m_RECORDING_STATUS_RECT = cv::Rect(1495, 410, 400, 50);
    ///@}

    /**
     * @brief TODO
     */
    void Print_Recording_Status_To_Frame();

    /**
     * @brief TODO
     */
    void Toggle_Recording();
};
