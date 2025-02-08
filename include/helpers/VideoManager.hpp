// NOLINTBEGIN

#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "detectors/LaneDetector.hpp"
#include "detectors/ObjectDetector.hpp"
#include "helpers/Performance.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
 */
namespace LaneAndObjectDetection
{
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
        explicit VideoManager(const std::string& p_inputVideoFilePath,
                              const std::string& p_yoloResourcesFolderPath,
                              const Detector& p_objectDetectorType = Detector::STANDARD,
                              const BackEnd& p_backEndType = BackEnd::CPU,
                              const BlobSize& p_blobSize = BlobSize::FIVE);

        /**
         * @brief TODO
         */
        ~VideoManager();

        /**
         * @brief An std::optional is used so that the PerformanceTests class can extract frame times from
         * the VideoManager class, when needed, while not effecting the usage of VideoManager
         * outside of performance testing
         */
        std::optional<std::vector<uint32_t>> Run();

    private:
        /**
         * @brief TODO
         */
        ///@{
        cv::VideoCapture m_inputVideo;
        cv::VideoWriter m_outputVideo;
        cv::Mat m_frame;
        bool m_recordOutput = false;
        std::unique_ptr<ObjectDetector> m_objectDetector; // Why pointer???
        std::unique_ptr<LaneDetector> m_laneDetector;
        std::unique_ptr<Performance> m_performance;

        const uint32_t m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_THICKNESS = 1, m_FONT_SCALE = 1;
        const cv::Rect m_RECORDING_STATUS_RECT = cv::Rect(1495, 410, 400, 50);
        ///@}

        /**
         * @brief TODO
         */
        void PrintRecordingStatusToFrame();

        /**
         * @brief TODO
         */
        void ToggleRecording();
    };
}

// NOLINTEND
