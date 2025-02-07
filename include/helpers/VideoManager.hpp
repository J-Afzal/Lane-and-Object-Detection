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

class VideoManager
{
public:
    /**
     * @param InputVideoPath
     *  A string to the input video file or if a camera input is desired, then
     *  pass the appropriate integer, as required by OpenCV, as a string
     * @param ObjectDetectorType
     *  Detector::NONE = No object detection
     *  Detector::STANDARD = Standard object detection (default)
     *  Detector::TINY = Lower accuracy but higher FPS object detection
     * @param BackEndType
     *  BackEnd::CPU = CPU
     *  BackEnd::CUDA = NVIDIA CUDA (default)
     * @param BlobSize
     *  BlobSize::ONE = 288
     *  BlobSize::TWO = 320
     *  BlobSize::THREE = 416
     *  BlobSize::FOUR = 512
     *  BlobSize::FIVE = 608 (default)
     */
    explicit VideoManager(const std::string &InputVideoFilePath,
                          const std::string &YoloResourcesFolderPath,
                          const Detector &ObjectDetectorType = Detector::STANDARD,
                          const BackEnd &BackEndType = BackEnd::CPU,
                          const BlobSize &BlobSize = BlobSize::FIVE);

    ~VideoManager();

    /**
     * @brief An std::optional is used so that the PerformanceTests class can extract frame times from
     * the VideoManager class, when needed, while not effecting the usage of VideoManager
     * outside of performance testing
     */
    std::optional<std::vector<int>> Run();

private:
    cv::VideoCapture m_InputVideo;
    cv::VideoWriter m_OutputVideo;
    cv::Mat m_Frame;
    bool m_RecordOutput = false;
    std::unique_ptr<ObjectDetector> m_ObjectDetector;
    std::unique_ptr<LaneDetector> m_LaneDetector;
    std::unique_ptr<Performance> m_Performance;

    const int m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
    const cv::Rect m_RECORDING_STATUS_RECT = cv::Rect(1495, 410, 400, 50);

    void Print_Recording_Status_To_Frame();

    void Toggle_Recording();
};
