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
         * @class RollingAverage
         * @brief TODO
         */
        class RollingAverage
        {
        public:
            /**
             * @brief TODO
             */
            explicit RollingAverage() = default;

            /**
             * @brief TODO
             */
            explicit RollingAverage(const uint32_t& p_sizeOfRollingAverage, const uint32_t& p_numberOfStates);

            /**
             * @brief TODO
             */
            uint32_t CalculateRollingAverage(const uint32_t& p_nextInput);

        private:
            /**
             * @brief TODO
             */
            std::deque<uint32_t> m_rollingAverageArray;

            /**
             * @brief TODO
             */
            std::vector<uint32_t> m_occurrenceOfEachState;
        };

        /**
         * @brief TODO
         */
        void Setup();

        /**
         * @brief TODO
         */
        void GetHoughLines(const cv::Mat& p_frame);

        /**
         * @brief TODO
         */
        void AnalyseHoughLines(const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief TODO
         */
        void GetDrivingState();

        /**
         * @brief TODO
         */
        void ExecuteDrivingState();

        /**
         * @brief TODO
         */
        ///@{
        LaneDetectionInformation m_laneDetectionInformation;
        RollingAverage m_horizontalLineStateRollingAverage; // TODO: better var names?
        RollingAverage m_leftLineTypeRollingAverage;
        RollingAverage m_middleLineTypeRollingAverage;
        RollingAverage m_rightLineTypeRollingAverage;
        RollingAverage m_drivingStateRollingAverage;
        std::deque<uint32_t> m_leftLineTypesForDisplay;
        std::deque<uint32_t> m_middleLineTypesForDisplay;
        std::deque<uint32_t> m_rightLineTypesForDisplay;
        std::vector<cv::Point> m_lanePoints;
        std::vector<cv::Vec4i> m_houghLines;
        std::vector<cv::Vec4i> m_leftLines;
        std::vector<cv::Vec4i> m_middleLines;
        std::vector<cv::Vec4i> m_rightLines;
        cv::Mat m_blankFrame;
        cv::Mat m_cannyFrame;
        cv::Mat m_roiFrame;
        std::string m_currentTurningState;
        std::string m_laneInformationTitle;
        std::string m_drivingStateTitle;
        std::string m_turningRequiredToReturnToCenter;
        uint32_t m_changingLanesFrameCount;
        uint32_t m_drivingState;
        uint32_t m_turningRequired;
        double m_changingLanesPreviousDifference;
        double m_leftLineAverageSize;
        double m_middleLineAverageSize;
        double m_rightLineAverageSize;
        bool m_giveWayWarning;
        ///@}


    };
}
