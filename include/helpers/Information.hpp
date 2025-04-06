#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

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
         * @brief TODO
         */
        cv::Mat m_roiFrame;

        /**
         * @brief TODO
         */
        cv::Mat m_cannyFrame;

        /**
         * @brief TODO
         */
        cv::Mat m_houghLinesFrame;

        /**
         * @brief The co-ordinate points that outline the current lane.
         */
        std::vector<cv::Point> m_laneOverlayCorners;

        /**
         * @brief The left-hand side road marking types to display.
         */
        std::deque<uint32_t> m_leftLineTypesForDisplay;

        /**
         * @brief The middle road marking types to display.
         */
        std::deque<uint32_t> m_middleLineTypesForDisplay;

        /**
         * @brief The right-hand side road marking types to display.
         */
        std::deque<uint32_t> m_rightLineTypesForDisplay;

        /**
         * @brief The current driving state.
         */
        Globals::DrivingState m_drivingState;

        /**
         * @brief The current driving state title.
         */
        std::string m_drivingStateTitle;

        /**
         * @brief The current lane information title.
         */
        std::string m_laneInformationTitle;

        /**
         * @brief The current turning state title.
         */
        std::string m_turningStateTitle;

        /**
         * @brief The text for the current turning required to return the vehicle to the center of the current lane.
         */
        std::string m_turningRequiredToReturnToCenterText;

        /**
         * @brief The number for the current turning required to return the vehicle to the center of the current lane.
         */
        int32_t m_turningRequiredToReturnToCenterPercentage;
    };

    /**
     * @brief The information needed by FrameBuilder to update frame with performance information.
     */
    struct PerformanceInformation
    {
        /**
         * @brief The frame times that have been measured.
         */
        std::vector<uint32_t> m_frameTimes;

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
         * @brief The time that the output started to be saved locally.
         */
        std::chrono::time_point<std::chrono::high_resolution_clock> m_saveOutputStartTime;

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
