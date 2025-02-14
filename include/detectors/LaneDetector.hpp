// NOLINTBEGIN

#pragma once

#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "helpers/RollingAverage.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
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
        LaneDetector() = default;

        /**
         * @brief TODO
         *
         */
        void RunDetector(const cv::Mat& p_frame, const std::vector<cv::Rect>& p_boundingBoxes);

        /**
         * @brief TODO
         *
         */
        void PrintToFrame(cv::Mat& p_frame);

    private:
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
        void AnalyseHoughLines(const std::vector<cv::Rect>& p_boundingBoxes);

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
        ///@{ Get_Hough_Lines()
        cv::Mat m_blankFrame, m_roiFrame, m_cannyFrame;
        std::vector<cv::Vec4i> m_houghLines;

        const uint32_t m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080;
        const uint32_t m_ROI_TOP_HEIGHT = 660, m_ROI_BOTTOM_HEIGHT = 840, m_ROI_TOP_WIDTH = 200, m_ROI_BOTTOM_WIDTH = 900;
        const std::vector<cv::Point> m_MASK_DIMENSIONS = {
            cv::Point((m_VIDEO_WIDTH / 2) - (m_ROI_TOP_WIDTH / 2), m_ROI_TOP_HEIGHT),
            cv::Point((m_VIDEO_WIDTH / 2) + (m_ROI_TOP_WIDTH / 2), m_ROI_TOP_HEIGHT),
            cv::Point((m_VIDEO_WIDTH / 2) + (m_ROI_BOTTOM_WIDTH / 2), m_ROI_BOTTOM_HEIGHT),
            cv::Point((m_VIDEO_WIDTH / 2) - (m_ROI_BOTTOM_WIDTH / 2), m_ROI_BOTTOM_HEIGHT)};
        const uint32_t m_CANNY_LOWER_THRESHOLD = 128, m_CANNY_UPPER_THRESHOLD = 255;
        const uint32_t m_HOUGH_THRESHOLD = 32, m_HOUGH_MIN_LINE_LENGTH = 16, m_HOUGH_MAX_LINE_GAP = 8;

        // Analyse_Hough_Lines()
        std::vector<cv::Vec4i> m_leftLines, m_middleLines, m_rightLines;
        double m_leftLineAverageSize {}, m_middleLineAverageSize {}, m_rightLineAverageSize {};
        bool m_giveWayWarning {};

        const double m_LEFT_EDGE_OF_MASK_M = (static_cast<double>(m_MASK_DIMENSIONS[0].y) - static_cast<double>(m_MASK_DIMENSIONS[3].y)) / (static_cast<double>(m_MASK_DIMENSIONS[0].x) - static_cast<double>(m_MASK_DIMENSIONS[3].x));
        const double m_LEFT_EDGE_OF_MASK_C = m_MASK_DIMENSIONS[0].y - (m_LEFT_EDGE_OF_MASK_M * m_MASK_DIMENSIONS[0].x);
        const double m_RIGHT_EDGE_OF_MASK_M = (static_cast<double>(m_MASK_DIMENSIONS[1].y) - static_cast<double>(m_MASK_DIMENSIONS[2].y)) / (static_cast<double>(m_MASK_DIMENSIONS[1].x) - static_cast<double>(m_MASK_DIMENSIONS[2].x));
        const double m_RIGHT_EDGE_OF_MASK_C = m_MASK_DIMENSIONS[1].y - (m_RIGHT_EDGE_OF_MASK_M * m_MASK_DIMENSIONS[1].x);

        const double m_TOP_MID_POINT = m_MASK_DIMENSIONS[0].x + ((static_cast<double>(m_MASK_DIMENSIONS[1].x) - static_cast<double>(m_MASK_DIMENSIONS[0].x)) / 2.);
        const double m_BOTTOM_ONE_THIRD = m_MASK_DIMENSIONS[3].x + ((static_cast<double>(m_MASK_DIMENSIONS[2].x) - static_cast<double>(m_MASK_DIMENSIONS[3].x)) / 3.);
        const double m_BOTTOM_TWO_THIRD = m_MASK_DIMENSIONS[3].x + (2. * (static_cast<double>(m_MASK_DIMENSIONS[2].x) - static_cast<double>(m_MASK_DIMENSIONS[3].x)) / 3.);

        const double m_LEFT_EDGE_THRESHOLD_M = (static_cast<double>(m_ROI_TOP_HEIGHT) - static_cast<double>(m_ROI_BOTTOM_HEIGHT)) / (m_TOP_MID_POINT - m_BOTTOM_ONE_THIRD);
        const double m_LEFT_EDGE_THRESHOLD_C = m_ROI_TOP_HEIGHT - (m_LEFT_EDGE_THRESHOLD_M * m_TOP_MID_POINT);
        const double m_RIGHT_EDGE_THRESHOLD_M = (static_cast<double>(m_ROI_TOP_HEIGHT) - static_cast<double>(m_ROI_BOTTOM_HEIGHT)) / (m_TOP_MID_POINT - m_BOTTOM_TWO_THIRD);
        const double m_RIGHT_EDGE_THRESHOLD_C = m_ROI_TOP_HEIGHT - (m_RIGHT_EDGE_THRESHOLD_M * m_TOP_MID_POINT);

        const double m_HORIZONTAL_GRADIENT_THRESHOLD = 0.15;
        const uint32_t m_HORIZONTAL_LENGTH_THRESHOLD = 50, m_HORIZONTAL_COUNT_THRESHOLD = 10;

        // Get_Driving_State()
        std::deque<uint32_t> m_leftLineTypesForDisplay = std::deque<uint32_t>(5, 0), m_middleLineTypesForDisplay = std::deque<uint32_t>(5, 0), m_rightLineTypesForDisplay = std::deque<uint32_t>(5, 0);
        uint32_t m_drivingState {};

        RollingAverage m_horizontalLineStateRollingAverage = RollingAverage(10, 2);
        RollingAverage m_leftLineTypeRollingAverage = RollingAverage(10, 3);
        RollingAverage m_middleLineTypeRollingAverage = RollingAverage(10, 3);
        RollingAverage m_rightLineTypeRollingAverage = RollingAverage(10, 3);
        RollingAverage m_drivingStateRollingAverage = RollingAverage(10, 5);

        const uint32_t m_SOLID_LINE_LENGTH_THRESHOLD = 75;

        // Execute_Driving_State()
        std::vector<cv::Point> m_lanePoints = std::vector<cv::Point>(4);
        std::string m_titleText, m_rightInfoTitleText, m_currentTurningState, m_turningRequiredToReturnToCenter;
        double m_changingLanesPreviousDifference = 0;
        uint32_t m_changingLanesFrameCount = 0, m_turningRequired {};
        bool m_printLaneOverlay {};

        const uint32_t m_FRAME_COUNT_THRESHOLD = 10;

        // Print_To_Frame()
        const cv::Rect m_RIGHT_INFO_RECT = cv::Rect(1495, 25, 400, 360);
        const uint32_t m_FONT_FACE = cv::FONT_HERSHEY_DUPLEX, m_FONT_SCALE = 1, m_FONT_THICKNESS = 1;
        ///@}
    };
}

// NOLINTEND
