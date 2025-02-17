#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
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
                              const Detectors& p_objectDetectorType = Detectors::STANDARD,
                              const BackEnds& p_backEndType = BackEnds::CPU,
                              const BlobSizes& p_blobSize = BlobSizes::FIVE);

        /**
         * @brief TODO
         */
        ~VideoManager();

        /**
         * @brief An std::optional (TODO: WRONG) is used so that the PerformanceTests class can extract frame times from
         * the VideoManager class, when needed, while not effecting the usage of VideoManager
         * outside of performance testing
         */
        std::vector<uint32_t> Run();

        /**
         * @brief Disable constructing a new VideoManager object using copy constructor.
         *
         * @param p_videoManager The video manager to copy.
         */
        VideoManager(const VideoManager& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new VideoManager object using move constructor.
         *
         * @param p_videoManager The video manager to copy.
         */
        VideoManager(const VideoManager&& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new VideoManager object using copy assignment operator.
         *
         * @param p_videoManager The video manager to copy.
         */
        VideoManager& operator=(const VideoManager& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new VideoManager object using move assignment operator.
         *
         * @param p_videoManager The video manager to copy.
         */
        VideoManager& operator=(const VideoManager&& p_videoManager) = delete;

    private:
        /**
         * @brief TODO
         */
        ///@{
        cv::VideoCapture m_inputVideo;
        cv::VideoWriter m_outputVideo;
        cv::Mat m_frame;
        LaneDetector m_laneDetector;
        ObjectDetector m_objectDetector;
        Performance m_performance;
        bool m_recordOutput;
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
