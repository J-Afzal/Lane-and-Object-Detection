#pragma once

#include <cstdint>
#include <deque>
#include <string>

#include <opencv2/core/types.hpp>

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @brief The information needed by %FrameBuilder to update frame with object detection information.
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
     * @brief The information needed by %FrameBuilder to update frame with lane detection information.
     */
    struct LaneDetectionInformation
    {
        /**
         * @brief The co-ordinate points that outline the current lane.
         */
        std::vector<cv::Point> m_lanePoints;

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
         * @brief The current turning state title.
         */
        std::string m_currentTurningState;

        /**
         * @brief The current driving state title.
         */
        std::string m_drivingStateTitle;

        /**
         * @brief The current lane information title.
         */
        std::string m_laneInformationTitle;

        /**
         * @brief The text for the current turning required to return the vehicle to the center of the current lane.
         */
        std::string m_turningRequiredToReturnToCenter;

        /**
         * @brief The current driving state.
         */
        uint32_t m_drivingState;

        /**
         * @brief The number for the current turning required to return the vehicle to the center of the current lane.
         */
        uint32_t m_turningRequired;
    };

    /**
     * @brief The information needed by %FrameBuilder to update frame with performance information.
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
     * @brief The information needed by %FrameBuilder to update frame with video manager information.
     */
    struct VideoManagerInformation
    {
        /**
         * @brief Whether the output video stream should be saved locally.
         */
        bool m_saveOutput;
    };
}
