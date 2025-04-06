#pragma once

#include <string>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class FrameBuilder
     * @brief Builds OpenCV frames to be displayed to the user.
     */
    class FrameBuilder
    {
    public:
        /**
         * @brief Disable constructing a new %FrameBuilder object.
         */
        explicit FrameBuilder() = delete;

        /**
         * @brief Updates `p_frame` with object detection, lane detection, performance and video manager information.
         *
         * @param p_frame The frame to update.
         * @param p_objectDetectionInformation The information needed by %FrameBuilder to update frame with object detection
         * information.
         * @param p_laneDetectionInformation The information needed by %FrameBuilder to update frame with lane detection
         * information.
         * @param p_performanceInformation The information needed by %FrameBuilder to update frame with performance information.
         * @param p_videoManagerInformation The information needed by %FrameBuilder to update frame with video manager
         * information.
         */
        static void UpdateFrame(cv::Mat& p_frame,
                                const ObjectDetectionInformation& p_objectDetectionInformation,
                                const LaneDetectionInformation& p_laneDetectionInformation,
                                const PerformanceInformation& p_performanceInformation,
                                const VideoManagerInformation& p_videoManagerInformation);

    private:
        /**
         * @brief Adds object detection information to `p_frame`.
         *
         * @param p_frame The frame to update.
         * @param p_objectDetectionInformation The information needed by %FrameBuilder to update frame with object detection
         * information.
         * @param p_debugMode Whether in debug mode.
         */
        static void AddObjectDetectorInformation(cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation, const bool& p_debugMode);

        /**
         * @brief Adds lane detection information to `p_frame`.
         *
         * @param p_frame The frame to update.
         * @param p_laneDetectionInformation The information needed by %FrameBuilder to update frame with lane detection
         * information.
         * @param p_debugMode Whether in debug mode.
         */
        static void AddLaneDetectorInformation(cv::Mat& p_frame, const LaneDetectionInformation& p_laneDetectionInformation, const bool& p_debugMode);

        /**
         * @brief Adds performance information to `p_frame`.
         *
         * @param p_frame The frame to update.
         * @param p_performanceInformation The information needed by %FrameBuilder to update frame with performance information.
         * @param p_debugMode Whether in debug mode.
         */
        static void AddPerformanceInformation(cv::Mat& p_frame, const PerformanceInformation& p_performanceInformation, const bool& p_debugMode);

        /**
         * @brief Adds video manager information to `p_frame`.
         *
         * @param p_frame The frame to update.
         * @param p_videoManagerInformation The information needed by %FrameBuilder to update frame with video manager
         * information.
         */
        static void AddVideoManagerInformation(cv::Mat& p_frame, const VideoManagerInformation& p_videoManagerInformation);

        /**
         * @brief Adds `p_backgroundRect` to `p_frame` and centers `p_text` within `p_backgroundRect`.
         *
         * @param p_frame The frame to update.
         * @param p_backgroundRect The text background rect to add to `p_frame`.
         * @param p_text The text to add to `p_frame`.
         * @param p_fontScale The font scale of p_text.
         */
        static void AddBackgroundRectAndCentredText(cv::Mat& p_frame, const cv::Rect& p_backgroundRect, const std::string& p_text, const double& p_fontScale = Globals::G_FONT_DEFAULT_SCALE);
    };
}
