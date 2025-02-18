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
 * @brief Contains all Lane-and-Object-Detection objects.
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
        explicit VideoManager() = default;

        /**
         * @brief TODO
         *
         * @param p_inputVideoCamera TODO
         * @param p_yoloResourcesFolderPath TODO
         * @param p_objectDetectorType TODO
         * @param p_backEndType TODO
         * @param p_blobSize TODO
         */
        explicit VideoManager(const uint32_t& p_inputVideoCamera,
                              const std::string& p_yoloResourcesFolderPath,
                              const Detectors& p_objectDetectorType,
                              const BackEnds& p_backEndType,
                              const BlobSizes& p_blobSize);

        /**
         * @brief TODO
         *
         * @param p_inputVideoFilePath TODO
         * @param p_yoloResourcesFolderPath TODO
         * @param p_objectDetectorType TODO
         * @param p_backEndType TODO
         * @param p_blobSize TODO
         */
        explicit VideoManager(const std::string& p_inputVideoFilePath,
                              const std::string& p_yoloResourcesFolderPath,
                              const Detectors& p_objectDetectorType,
                              const BackEnds& p_backEndType,
                              const BlobSizes& p_blobSize);

        /**
         * @brief TODO
         *
         * @param p_commandLineArguments TODO
         */
        explicit VideoManager(const std::vector<std::string>& p_commandLineArguments);

        /**
         * @brief TODO
         */
        ~VideoManager();

        /**
         * @brief TODO
         *
         * @param p_inputVideoCamera TODO
         * @param p_yoloResourcesFolderPath TODO
         * @param p_objectDetectorType TODO
         * @param p_backEndType TODO
         * @param p_blobSize TODO
         */
        void SetProperties(const uint32_t& p_inputVideoCamera,
                           const std::string& p_yoloResourcesFolderPath,
                           const Detectors& p_objectDetectorType,
                           const BackEnds& p_backEndType,
                           const BlobSizes& p_blobSize);

        /**
         * @brief TODO
         *
         * @param p_inputVideoFilePath TODO
         * @param p_yoloResourcesFolderPath TODO
         * @param p_objectDetectorType TODO
         * @param p_backEndType TODO
         * @param p_blobSize TODO
         */
        void SetProperties(const std::string& p_inputVideoFilePath,
                           const std::string& p_yoloResourcesFolderPath,
                           const Detectors& p_objectDetectorType,
                           const BackEnds& p_backEndType,
                           const BlobSizes& p_blobSize);

        /**
         * @brief TODO
         *
         */
        void Run();

        /**
         * @brief TODO
         *
         * @return std::vector<uint32_t> TODO
         */
        std::vector<uint32_t> GetFrameTimes();

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
         *
         */
        void UpdateCurrentFrame();

        /**
         * @brief TODO
         *
         */
        void ToggleRecording();

        /**
         * @brief TODO
         */
        cv::VideoCapture m_inputVideo;

        /**
         * @brief TODO
         */
        cv::VideoWriter m_outputVideo;

        /**
         * @brief TODO
         */
        cv::Mat m_currentFrame;

        /**
         * @brief TODO
         */
        LaneDetector m_laneDetector;

        /**
         * @brief TODO
         */
        ObjectDetector m_objectDetector;

        /**
         * @brief TODO
         */
        Performance m_performance;

        /**
         * @brief TODO
         */
        bool m_recordOutput;
    };
}
