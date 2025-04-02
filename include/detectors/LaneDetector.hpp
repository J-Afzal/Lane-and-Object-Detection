#pragma once

#include <cstdint>
#include <deque>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>

#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class LaneDetector
     * @brief Detects the lane the vehicle is in and other lane-related metrics.
     */
    class LaneDetector
    {
    public:
        /**
         * @brief Construct a new %LaneDetector object.
         */
        explicit LaneDetector();

        /**
         * @brief Runs the lane detector against `p_frame`.
         *
         * @param p_frame The frame to run the lane detector against.
         * @param p_objectDetectionInformation The ObjectDetectionInformation struct containing all object detection-related
         * information.
         */
        void RunLaneDetector(const cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief Get the LaneDetectionInformation struct.
         *
         * @return LaneDetectionInformation The LaneDetectionInformation struct.
         */
        LaneDetectionInformation GetInformation();

    private:
        /**
         * @brief Determines whether the detected hough lines are left, middle or right lane lines. The majority of the noise
         * removal occurs here.
         *
         * @param p_houghLines The hough lines that have been detected.
         * @param p_objectDetectionInformation The ObjectDetectionInformation struct containing the object bounding boxes.
         */
        void AnalyseHoughLines(const std::vector<cv::Vec4i>& p_houghLines, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief Determines whether `p_houghLine` is within any one of the `p_objectDetectionInformation` object bounding boxes.
         *
         * @param p_houghLine The line to check.
         * @param p_objectDetectionInformation The ObjectDetectionInformation struct containing the object bounding boxes.
         * @return true If `p_houghLine` is within the object bounding boxes found within `p_objectDetectionInformation`.
         * @return false If `p_houghLine` is NOT within the object bounding boxes found within `p_objectDetectionInformation`.
         */
        static bool IsLineWithinObjectBoundingBoxes(const cv::Vec4i& p_houghLine, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief Determines whether the left, middle, and right lane lines detected are solid, dashed or empty.
         */
        void UpdateLineTypes();

        /**
         * @brief Determines the current driving state depending upon the absence/presence of left, right and middle lane lines.
         */
        void UpdateDrivingState();

        /**
         * @brief Determines the vehicles position within the current lane, the turning required to return to the centre of the
         * lane and the lane overlay.
         */
        void CalculateLanePosition();

        /**
         * @brief Determines the direction (left, right or neither) that the vehicle is turning while changing lanes.
         */
        void CalculateTurningDirection();

        /**
         * @class RollingAverage
         * @brief Calculates a rolling average to provide a smoothed view of state trends over time.
         * @tparam T The type of value to be stored in the rolling average buffer.
         */
        template<typename T>
        class RollingAverage
        {
        public:
            /**
             * @brief Constructs a new %RollingAverage object.
             */
            explicit RollingAverage();

            /**
             * @brief Adds `p_nextInput` to the rolling average buffer and returns the most frequent value within the rolling
             * average buffer.
             *
             * @param p_nextInput The value to add to the rolling average buffer.
             * @return T The most frequent value within the rolling average buffer.
             */
            T CalculateRollingAverage(const T& p_nextInput);

        private:
            /**
             * @brief A double-ended queue representing the states stored in the rolling average.
             */
            std::deque<T> m_rollingAverageArray;
        };

        /**
         * @brief The LaneDetectionInformation struct containing all lane detection-related information.
         */
        LaneDetectionInformation m_laneDetectionInformation;

        /**
         * @brief The rolling averages to determine the line types (solid, dashed or empty) for the left, middle and right lane
         * lines.
         */
        ///@{
        RollingAverage<uint32_t> m_leftLaneLineTypeRollingAverage;
        RollingAverage<uint32_t> m_middleLaneLineTypeRollingAverage;
        RollingAverage<uint32_t> m_rightLaneLineTypeRollingAverage;
        ///@}

        /**
         * @brief The rolling average to determine the driving state.
         */
        RollingAverage<Globals::DrivingState> m_drivingStateRollingAverage;

        /**
         * @brief The left, middle and right lane lines that have been detected for the current frame.
         */
        ///@{
        std::vector<cv::Vec4i> m_leftLaneLines;
        std::vector<cv::Vec4i> m_middleLaneLines;
        std::vector<cv::Vec4i> m_rightLaneLines;
        ///@}

        /**
         * @brief The distance difference that will be compared to in order to determine the direction the vehicle is moving
         * while changing lanes.
         */
        double m_changingLanesPreviousDistanceDifference;

        /**
         * @brief The average left, middle and right lane line lengths for the current frame.
         */
        ///@{
        double m_leftLineAverageLength;
        double m_middleLineAverageLength;
        double m_rightLineAverageLength;
        ///@}

        /**
         * @brief The number of frames passed since a distance difference was calculated while the vehicle is changing lanes.
         */
        uint32_t m_changingLanesFrameCount;

        /**
         * @brief Whether the current frame is the frame in which the driving state changed to CHANGING_LANES.
         */
        bool m_changingLanesFirstFrame;
    };
}
