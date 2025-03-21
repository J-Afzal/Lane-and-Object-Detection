#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>

#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class LaneDetector
     * @brief TODO
     */
    class LaneDetector
    {
    public:
        /**
         * @brief TODO
         *
         */
        explicit LaneDetector();

        /**
         * @brief TODO
         *
         */
        void RunLaneDetector(const cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief TODO
         */
        LaneDetectionInformation GetInformation();

    private:
        /**
         * @brief TODO
         *
         * The straight lines received from the PHT contain lines that are not a part of any road markings (i.e. noise) and so
         * this for loop tries to remove as much as them whilst preserving the useful lines.
         *
         * @param p_houghLines TODO
         * @param p_objectDetectionInformation TODO
         */
        void AnalyseHoughLines(const std::vector<cv::Vec4i>& p_houghLines, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief TODO
         *
         * @param p_houghLine TODO
         * @param p_objectDetectionInformation TODO
         * @return true TODO
         * @return false TODO
         */
        bool IsLineWithinBoundingBoxes(const cv::Vec4i& p_houghLine, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief TODO
         */
        void UpdateLineTypes();

        /**
         * @brief TODO
         */
        void UpdateDrivingState();

        /**
         * @brief TODO
         */
        void ExecuteDrivingState();

        /**
         * @class RollingAverage
         * @brief Calculates a rolling average to provide a smoothed view of state trends over time.
         */
        class RollingAverage
        {
        public:
            /**
             * @brief Constructs a new %RollingAverage object.
             */
            explicit RollingAverage();

            /**
             * @brief Advances the rolling average with p_nextInput and returns the most frequent state.
             */
            uint32_t CalculateRollingAverage(const uint32_t& p_nextInput);

        private:
            /**
             * @brief A double-ended queue represent the states stored in the rolling average.
             */
            std::deque<uint32_t> m_rollingAverageArray;
        };

        /**
         * @brief TODO
         */
        enum class LineType : std::uint8_t
        {
            EMPTY = 0,
            DASHED,
            SOLID
        };

        /**
         * @brief TODO
         */
        enum class DrivingState : std::uint8_t
        {
            WITHIN_LANE = 0,
            CHANGING_LANES,
            ONLY_LEFT_LANE_MARKING_DETECTED,
            ONLY_RIGHT_LANE_MARKING_DETECTED,
            NO_LANE_MARKINGS_DETECTED
        };

        /**
         * @brief TODO
         */
        ///@{
        LaneDetectionInformation m_laneDetectionInformation;
        RollingAverage m_leftLineTypeRollingAverage;
        RollingAverage m_middleLineTypeRollingAverage;
        RollingAverage m_rightLineTypeRollingAverage;
        RollingAverage m_drivingStateRollingAverage;
        std::vector<cv::Vec4i> m_leftLines;
        std::vector<cv::Vec4i> m_middleLines;
        std::vector<cv::Vec4i> m_rightLines;
        double m_leftLineAverageSize;
        double m_middleLineAverageSize;
        double m_rightLineAverageSize;
        ///@}





        /**
         * @brief TODO
         */
        ///@{

        std::deque<uint32_t> m_leftLineTypesForDisplay;
        std::deque<uint32_t> m_middleLineTypesForDisplay;
        std::deque<uint32_t> m_rightLineTypesForDisplay;
        std::vector<cv::Point> m_lanePoints;

        std::string m_currentTurningState;
        std::string m_laneInformationTitle;
        std::string m_drivingStateTitle;
        std::string m_turningRequiredToReturnToCenter;
        uint32_t m_changingLanesFrameCount;
        uint32_t m_drivingState;
        uint32_t m_turningRequired;
        double m_changingLanesPreviousDifference;
        ///@}
    };
}
