/**
 * @file LaneDetector.hpp
 * @author Junaid Afzal
 * @brief This class performs lane detection using OpenCV,
 * Canny Algorithm and the Probabilistic Hough Transform (PHT).
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"
#include "helpers/RollingAverage.hpp"

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
    ~LaneDetector() = default;

    /**
     * @brief TODO
     *
     */
    void Run_Detector(const cv::Mat &Frame, const std::vector<cv::Rect> &BoundingBoxes);

    /**
     * @brief TODO
     *
     */
    void Print_To_Frame(cv::Mat &Frame);

private:

    /**
     * @brief TODO
     */
    ///@{ Get_Hough_Lines()
    cv::Mat m_BlankFrame, m_ROIFrame, m_CannyFrame;
    std::vector<cv::Vec4i> m_HoughLines;

    const int m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080;
    const int m_ROI_TOP_HEIGHT = 660, m_ROI_BOTTOM_HEIGHT = 840, m_ROI_TOP_WIDTH = 200, m_ROI_BOTTOM_WIDTH = 900;
    const std::vector<cv::Point> m_MASK_DIMENSIONS = {
            cv::Point(m_VIDEO_WIDTH / 2 - m_ROI_TOP_WIDTH / 2, m_ROI_TOP_HEIGHT),
            cv::Point(m_VIDEO_WIDTH / 2 + m_ROI_TOP_WIDTH / 2, m_ROI_TOP_HEIGHT),
            cv::Point(m_VIDEO_WIDTH / 2 + m_ROI_BOTTOM_WIDTH / 2, m_ROI_BOTTOM_HEIGHT),
            cv::Point(m_VIDEO_WIDTH / 2 - m_ROI_BOTTOM_WIDTH / 2, m_ROI_BOTTOM_HEIGHT)
    };
    const int m_CANNY_LOWER_THRESHOLD = 128, m_CANNY_UPPER_THRESHOLD = 255;
    const int m_HOUGH_THRESHOLD = 32, m_HOUGH_MIN_LINE_LENGTH = 16, m_HOUGH_MAX_LINE_GAP = 8;

    // Analyse_Hough_Lines()
    std::vector<cv::Vec4i> m_LeftLines, m_MiddleLines, m_RightLines;
    double m_LeftLineAverageSize{}, m_MiddleLineAverageSize{}, m_RightLineAverageSize{};
    bool m_Give_Way_Warning{};

    const double m_LEFT_EDGE_OF_MASK_M = ((double)m_MASK_DIMENSIONS[0].y - (double)m_MASK_DIMENSIONS[3].y) / (double)((double)m_MASK_DIMENSIONS[0].x - (double)m_MASK_DIMENSIONS[3].x);
    const double m_LEFT_EDGE_OF_MASK_C = m_MASK_DIMENSIONS[0].y - m_LEFT_EDGE_OF_MASK_M * m_MASK_DIMENSIONS[0].x;
    const double m_RIGHT_EDGE_OF_MASK_M = ((double)m_MASK_DIMENSIONS[1].y - (double)m_MASK_DIMENSIONS[2].y) / (double)((double)m_MASK_DIMENSIONS[1].x - (double)m_MASK_DIMENSIONS[2].x);
    const double m_RIGHT_EDGE_OF_MASK_C = m_MASK_DIMENSIONS[1].y - m_RIGHT_EDGE_OF_MASK_M * m_MASK_DIMENSIONS[1].x;

    const double m_TOP_MID_POINT = m_MASK_DIMENSIONS[0].x + ((double)m_MASK_DIMENSIONS[1].x - (double)m_MASK_DIMENSIONS[0].x) / 2.;
    const double m_BOTTOM_ONE_THIRD = m_MASK_DIMENSIONS[3].x + ((double)m_MASK_DIMENSIONS[2].x - (double)m_MASK_DIMENSIONS[3].x) / 3.;
    const double m_BOTTOM_TWO_THIRD = m_MASK_DIMENSIONS[3].x + 2. * ((double)m_MASK_DIMENSIONS[2].x - (double)m_MASK_DIMENSIONS[3].x) / 3.;

    const double m_LEFT_EDGE_THRESHOLD_M = ((double)m_ROI_TOP_HEIGHT - (double)m_ROI_BOTTOM_HEIGHT) / (m_TOP_MID_POINT - m_BOTTOM_ONE_THIRD);
    const double m_LEFT_EDGE_THRESHOLD_C = m_ROI_TOP_HEIGHT - m_LEFT_EDGE_THRESHOLD_M * m_TOP_MID_POINT;
    const double m_RIGHT_EDGE_THRESHOLD_M = ((double)m_ROI_TOP_HEIGHT - (double)m_ROI_BOTTOM_HEIGHT) / (m_TOP_MID_POINT - m_BOTTOM_TWO_THIRD);
    const double m_RIGHT_EDGE_THRESHOLD_C = m_ROI_TOP_HEIGHT - m_RIGHT_EDGE_THRESHOLD_M * m_TOP_MID_POINT;

    const double m_HORIZONTAL_GRADIENT_THRESHOLD = 0.15;
    const int m_HORIZONTAL_LENGTH_THRESHOLD = 50, m_HORIZONTAL_COUNT_THRESHOLD = 10;

    // Get_Driving_State()
    std::deque<int> m_LeftLineTypesForDisplay = std::deque<int>(5, 0), m_MiddleLineTypesForDisplay = std::deque<int>(5, 0), m_RightLineTypesForDisplay = std::deque<int>(5, 0);
    int m_DrivingState{};

    RollingAverage m_HorizontalLineStateRollingAverage = RollingAverage(10, 2);
    RollingAverage m_LeftLineTypeRollingAverage = RollingAverage(10, 3);
    RollingAverage m_MiddleLineTypeRollingAverage = RollingAverage(10, 3);
    RollingAverage m_RightLineTypeRollingAverage = RollingAverage(10, 3);
    RollingAverage m_DrivingStateRollingAverage = RollingAverage(10, 5);

    const int m_SOLID_LINE_LENGTH_THRESHOLD = 75;

    // Execute_Driving_State()
    std::vector<cv::Point> m_LanePoints = std::vector<cv::Point>(4);
    std::string m_TitleText, m_RightInfoTitleText, m_CurrentTurningState, m_TurningRequiredToReturnToCenter;
    double m_ChangingLanesPreviousDifference = 0;
    int m_ChangingLanesFrameCount = 0, m_TurningRequired{};
    bool m_PrintLaneOverlay{};

    const int m_FRAME_COUNT_THRESHOLD = 10;

    // Print_To_Frame()
    const cv::Rect m_RIGHT_INFO_RECT = cv::Rect(1495, 25, 400, 360);
    const int FONT_FACE = cv::FONT_HERSHEY_DUPLEX, FONT_SCALE = 1, FONT_THICKNESS = 1;
    ///@}

    /**
     * @brief TODO
     */
    void Setup();

    /**
     * @brief TODO
     */
    void Get_Hough_Lines(const cv::Mat &Frame);

    /**
     * @brief TODO
     */
    void Analyse_Hough_Lines(const std::vector<cv::Rect> &BoundingBoxes);

    /**
     * @brief TODO
     */
    void Get_Driving_State();

    /**
     * @brief TODO
     */
    void Execute_Driving_State();
};
