#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>

#include "helpers/Globals.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @brief The information needed by FrameBuilder to update frame with object detection information.
     */
    struct ObjectDetectionInformation
    {
        /**
         * @brief The properties of a single detected object.
         */
        struct DetectedObjectInformation
        {
            /**
             * @brief The bounding box of the detected object.
             */
            cv::Rect m_boundingBox;

            /**
             * @brief The bounding box colour.
             */
            cv::Scalar m_boundingBoxColour;

            /**
             * @brief The name of the detected object.
             */
            std::string m_objectName;
        };

        /**
         * @brief The list of detected objects for the current frame.
         */
        std::vector<DetectedObjectInformation> m_objectInformation;
    };

    /**
     * @brief The information needed by FrameBuilder to update frame with lane detection information.
     */
    struct LaneDetectionInformation
    {
        /**
         * @brief The initial region-of-interest frame.
         */
        cv::Mat m_roiFrame;

        /**
         * @brief The frame containing the edges detected by the Canny algorithm.
         */
        cv::Mat m_cannyFrame;

        /**
         * @brief The frame contains the lines detected by the Hough transform.
         */
        cv::Mat m_houghLinesFrame;

        /**
         * @brief The co-ordinate points that outline the current lane.
         */
        std::vector<cv::Point> m_laneOverlayCorners;

        /**
         * @brief The current driving state title.
         */
        std::string m_drivingStateTitle;

        /**
         * @brief Either the lane line information or turning state depending upon the current driving state.
         */
        std::string m_drivingStateSubTitle;
    };

    /**
     * @brief The information needed by FrameBuilder to update frame with performance information.
     */
    struct PerformanceInformation
    {
        /**
         * @brief The average number of frames per second.
         */
        double m_averageFramesPerSecond;

        /**
         * @brief The current number of frames per second.
         */
        double m_currentFramesPerSecond;
    };

    /**
     * @brief The information needed by FrameBuilder to update frame with video manager information.
     */
    struct VideoManagerInformation
    {
        /**
         * @brief Text to display whether or not the program is in debug mode.
         */
        std::string m_debugModeText;

        /**
         * @brief The time that has elapsed since starting to save the output video stream locally.
         */
        std::string m_saveOutputElapsedTime;

        /**
         * @brief Text to display whether or not the program is recording the output.
         */
        std::string m_saveOutputText;

        /**
         * @brief Whether in debug mode.
         */
        bool m_debugMode;

        /**
         * @brief Whether the output video stream should be saved locally.
         */
        bool m_saveOutput;
    };
}
