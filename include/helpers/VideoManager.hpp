#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

#include "detectors/LaneDetector.hpp"
#include "detectors/ObjectDetector.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"
#include "helpers/Performance.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class VideoManager
     * @brief Orchestration class which reads the input, runs the lane and object detectors, displays the detected output and
     * saves output video stream locally.
     */
    class VideoManager
    {
    public:
        /**
         * @brief Constructs a new %VideoManager object.
         * @warning `SetProperties()` should be used to initialise the necessary member variables before calling `Run()`.
         */
        explicit VideoManager();

        /**
         * @brief Constructs a new %VideoManager object for use with an OpenCV camera input (for API use).
         *
         * @param p_inputVideoCamera Camera Id of the video capturing device to open using the OpenCV `VideoCapture` class.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_objectDetectorTypes The type of object detector to use with an option to disable object detection. The tiny
         * version is more performant at the cost of accuracy.
         * @param p_objectDetectorBackEnds The supported backends for the object detector to run on. CUDA is significantly more
         * performant.
         * @param p_objectDetectorBlobSizes The supported blob sizes for the object detector to run with. The larger the blob
         * size the more performant the detector at the cost of performance.
         */
        explicit VideoManager(const int32_t& p_inputVideoCamera,
                              const std::string& p_yoloFolderPath,
                              const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                              const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds = Globals::ObjectDetectorBackEnds::CPU,
                              const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes = Globals::ObjectDetectorBlobSizes::ONE);

        /**
         * @brief Constructs a new %VideoManager object for use with a video file input (for API use).
         *
         * @param p_inputVideoFilePath The file path of the video file to open using the OpenCV `VideoCapture` class.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_objectDetectorTypes The type of object detector to use with an option to disable object detection. The tiny
         * version is more performant at the cost of accuracy.
         * @param p_objectDetectorBackEnds The supported backends for the object detector to run on. CUDA is significantly more
         * performant.
         * @param p_objectDetectorBlobSizes The supported blob sizes for the object detector to run with. The larger the blob
         * size the more performant the detector at the cost of performance.
         */
        explicit VideoManager(const std::string& p_inputVideoFilePath,
                              const std::string& p_yoloFolderPath,
                              const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                              const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds = Globals::ObjectDetectorBackEnds::CPU,
                              const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes = Globals::ObjectDetectorBlobSizes::ONE);

        /**
         * @brief Constructs a new %VideoManager object (for CLI use).
         *
         * Usage: lane-and-object-detection --input ... --yolo-folder-path ... --object-detector-type ... [optional]
         *
         * OPTIONS:
         *
         * Generic Options:
         *
         *   -h --help                       Display available options.
         *
         * Required Options:
         *
         *   -i --input                      File path or camera ID
         *   -y --yolo-folder-path           Path to the yolo folder
         *
         * Optional options:
         *
         *   -o --object-detector-type       One of: none, standard or tiny. (default = none)
         *   -b --object-detector-backend    One of: cpu or cuda (default = cpu)
         *   -s --object-detector-blob-size  One of: 208, 320, 416, 512 or 608 (default = 208)
         *
         * @param p_commandLineArguments List of all command line arguments.
         */
        explicit VideoManager(const std::vector<std::string>& p_commandLineArguments);

        /**
         * @brief Destructs a %VideoManager object.
         */
        ~VideoManager();

        /**
         * @brief Set the properties of the %VideoManager object.
         *
         * @param p_inputVideoCamera Camera ID of the video capturing device to open using the OpenCV `VideoCapture` class.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_objectDetectorTypes The type of object detector to use with an option to disable object detection. The tiny
         * version is more performant at the cost of accuracy.
         * @param p_objectDetectorBackEnds The supported backends for the object detector to run on. CUDA is significantly more
         * performant.
         * @param p_objectDetectorBlobSizes The supported blob sizes for the object detector to run with. The larger the blob
         * size the more performant the detector at the cost of performance.
         */
        void SetProperties(const int32_t& p_inputVideoCamera,
                           const std::string& p_yoloFolderPath,
                           const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                           const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                           const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes);

        /**
         * @brief Set the properties of the %VideoManager object.
         *
         * @param p_inputVideoFilePath The file path of the video file to open using the OpenCV `VideoCapture` class.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_objectDetectorTypes The type of object detector to use with an option to disable object detection. The tiny
         * version is more performant at the cost of accuracy.
         * @param p_objectDetectorBackEnds The supported backends for the object detector to run on. CUDA is significantly more
         * performant.
         * @param p_objectDetectorBlobSizes The supported blob sizes for the object detector to run with. The larger the blob
         * size the more performant the detector at the cost of performance.
         */
        void SetProperties(const std::string& p_inputVideoFilePath,
                           const std::string& p_yoloFolderPath,
                           const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                           const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                           const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes);

        /**
         * @brief Orchestration function which contains the main program loop.
         */
        void RunLaneAndObjectDetector();

        /**
         * @brief Get the times to compute each and every frame.Globals
         *
         * @return `std::vector<uint32_t>` A list of the time to compute each frame.
         */
        std::vector<uint32_t> GetFrameTimes();

        /**
         * @brief Disable constructing a new %VideoManager object using copy constructor.
         *
         * @param p_videoManager The %VideoManager to copy.
         */
        VideoManager(const VideoManager& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new %VideoManager object using move constructor.
         *
         * @param p_videoManager The %VideoManager to copy.
         */
        VideoManager(const VideoManager&& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new %VideoManager object using copy assignment operator.
         *
         * @param p_videoManager The %VideoManager to copy.
         */
        VideoManager& operator=(const VideoManager& p_videoManager) = delete;

        /**
         * @brief Disable constructing a new %VideoManager object using move assignment operator.
         *
         * @param p_videoManager The %VideoManager to copy.
         */
        VideoManager& operator=(const VideoManager&& p_videoManager) = delete;

    private:
        /**
         * @brief Toggles the debug mode and whether to destroy the debug frames.
         */
        void ToggleDebugMode();

        /**
         * @brief Toggles whether the output video stream is saved locally. If toggled on, it creates a new file in the format
         * `YYYY-MM-DD-HH:mm:ss-output.mp4`, and if toggled off, it closes the output file.
         */
        void ToggleSaveOutput();

        /**
         * @brief Destroys all windows and releases all input/output videos.
         */
        void Quit();

        /**
         * @brief Represents the input video stream.
         */
        cv::VideoCapture m_inputVideo;

        /**
         * @brief Represents the output video stream.
         */
        cv::VideoWriter m_outputVideo;

        /**
         * @brief Represents the current frame being processed.
         */
        cv::Mat m_currentFrame;

        /**
         * @brief The lane detector.
         */
        LaneDetector m_laneDetector;

        /**
         * @brief The object detector.
         */
        ObjectDetector m_objectDetector;

        /**
         * @brief The frame time and current/average frames per second performance calculator.
         */
        Performance m_performance;

        /**
         * @brief The VideoManagerInformation struct containing all video manager-related information.
         */
        VideoManagerInformation m_videoManagerInformation;

        /**
         * @brief The time that the output started to be saved locally.
         */
        std::chrono::time_point<std::chrono::high_resolution_clock> m_saveOutputStartTime;
    };
}
