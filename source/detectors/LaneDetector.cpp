/**
 * @file LaneDetector.cpp
 * @author Junaid Afzal
 * @brief Implementation of LaneDetector.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "detectors/LaneDetector.hpp"

void LaneDetector::Run_Detector(const cv::Mat &Frame, const std::vector<cv::Rect> &BoundingBoxes)
{
    Setup();

    Get_Hough_Lines(Frame);

    Analyse_Hough_Lines(BoundingBoxes);

    Get_Driving_State();

    Execute_Driving_State();
}

void LaneDetector::Setup()
{
    m_HoughLines.clear();
    m_LeftLines.clear();
    m_MiddleLines.clear();
    m_RightLines.clear();
    m_LeftLineAverageSize = m_MiddleLineAverageSize = m_RightLineAverageSize = m_ChangingLanesPreviousDifference;
    m_PrintLaneOverlay = false;
}

void LaneDetector::Get_Hough_Lines(const cv::Mat &Frame)
{
    m_BlankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, Frame.type());
    cv::fillConvexPoly(m_BlankFrame, m_MASK_DIMENSIONS, cv::Scalar(255, 255, 255), cv::LINE_AA, 0);
    cv::bitwise_and(m_BlankFrame, Frame, m_ROIFrame);

    cv::cvtColor(m_ROIFrame, m_ROIFrame, cv::COLOR_BGR2GRAY);

    cv::Canny(m_ROIFrame, m_CannyFrame, m_CANNY_LOWER_THRESHOLD, m_CANNY_UPPER_THRESHOLD, 3, true);

    cv::HoughLinesP(m_CannyFrame, m_HoughLines, 1, CV_PI / 180.0, m_HOUGH_THRESHOLD, m_HOUGH_MIN_LINE_LENGTH, m_HOUGH_MAX_LINE_GAP);
}

void LaneDetector::Analyse_Hough_Lines(const std::vector<cv::Rect> &BoundingBoxes)
{
    double dx, dy, Gradient, LeftY1, LeftY2, RightY1, RightY2;
    int LowerX, UpperX, LowerY, UpperY, HorizontalCount = 0;
    bool LineIsInBoundingBox;
    for (const auto &HoughLine : m_HoughLines)
    {
        LineIsInBoundingBox = false;

        for (const auto &BoundingBox : BoundingBoxes)
        {
            LowerX = BoundingBox.x;
            UpperX = BoundingBox.x + BoundingBox.width;
            LowerY = BoundingBox.y;
            UpperY = BoundingBox.y + BoundingBox.height;

            if ((HoughLine[0] >= LowerX && HoughLine[0] <= UpperX && HoughLine[1] >= LowerY && HoughLine[1] <= UpperY) ||
                (HoughLine[2] >= LowerX && HoughLine[2] <= UpperX && HoughLine[3] >= LowerY && HoughLine[3] <= UpperY))
            {
                LineIsInBoundingBox = true;
                break;
            }
        }

        if (LineIsInBoundingBox)
            continue;

        dx = HoughLine[0] - HoughLine[2];
        dy = HoughLine[1] - HoughLine[3];
        if (dx == 0)
            continue;
        Gradient = dy / dx;

        // Horizontal Lines
        if (std::fabs(Gradient) < m_HORIZONTAL_GRADIENT_THRESHOLD)
        {
            if ((std::sqrt(dy * dy + dx * dx) > m_HORIZONTAL_LENGTH_THRESHOLD) &&
                (((HoughLine[1] >= m_ROI_TOP_HEIGHT + 1) && (HoughLine[3] >= m_ROI_TOP_HEIGHT + 1)) || ((HoughLine[1] <= m_ROI_BOTTOM_HEIGHT - 1) && (HoughLine[3] <= m_ROI_BOTTOM_HEIGHT - 1))))
                HorizontalCount++;
        }

        // Vertical Lines
        else
        {
            // Remove left edge of mask
            LeftY1 = m_LEFT_EDGE_OF_MASK_M * HoughLine[0] + m_LEFT_EDGE_OF_MASK_C;
            LeftY2 = m_LEFT_EDGE_OF_MASK_M * HoughLine[2] + m_LEFT_EDGE_OF_MASK_C;
            if ((HoughLine[1] <= LeftY1 + 1) && (HoughLine[3] <= LeftY2 + 1))
                continue;

            // left threshold
            LeftY1 = m_LEFT_EDGE_THRESHOLD_M * HoughLine[0] + m_LEFT_EDGE_THRESHOLD_C;
            LeftY2 = m_LEFT_EDGE_THRESHOLD_M * HoughLine[2] + m_LEFT_EDGE_THRESHOLD_C;

            if ((HoughLine[1] < LeftY1) && (HoughLine[3] < LeftY2) && Gradient < 0)
            {
                m_LeftLines.push_back(HoughLine);
                m_LeftLineAverageSize += std::sqrt((HoughLine[0] - HoughLine[2]) * (HoughLine[0] - HoughLine[2]) + (HoughLine[1] - HoughLine[3]) * (HoughLine[1] - HoughLine[3]));
                continue;
            }

            // Remove right edge of mask
            RightY1 = m_RIGHT_EDGE_OF_MASK_M * HoughLine[0] + m_RIGHT_EDGE_OF_MASK_C;
            RightY2 = m_RIGHT_EDGE_OF_MASK_M * HoughLine[2] + m_RIGHT_EDGE_OF_MASK_C;
            if ((HoughLine[1] <= RightY1 + 1) && (HoughLine[3] <= RightY2 + 1))
                continue;

            // right threshold
            RightY1 = m_RIGHT_EDGE_THRESHOLD_M * HoughLine[0] + m_RIGHT_EDGE_THRESHOLD_C;
            RightY2 = m_RIGHT_EDGE_THRESHOLD_M * HoughLine[2] + m_RIGHT_EDGE_THRESHOLD_C;

            if ((HoughLine[1] < RightY1) && (HoughLine[3] < RightY2) && Gradient > 0)
            {
                m_RightLines.push_back(HoughLine);
                m_RightLineAverageSize += std::sqrt((HoughLine[0] - HoughLine[2]) * (HoughLine[0] - HoughLine[2]) + (HoughLine[1] - HoughLine[3]) * (HoughLine[1] - HoughLine[3]));
                continue;
            }

            // else must be in middle
            m_MiddleLines.push_back(HoughLine);
            m_MiddleLineAverageSize += std::sqrt((HoughLine[0] - HoughLine[2]) * (HoughLine[0] - HoughLine[2]) + (HoughLine[1] - HoughLine[3]) * (HoughLine[1] - HoughLine[3]));
        }
    }

    if (HorizontalCount > m_HORIZONTAL_COUNT_THRESHOLD)
        m_Give_Way_Warning = m_HorizontalLineStateRollingAverage.calculateRollingAverage(1);
    else
        m_Give_Way_Warning = m_HorizontalLineStateRollingAverage.calculateRollingAverage(0);

    m_LeftLineAverageSize /= (double)m_LeftLines.size();
    m_MiddleLineAverageSize /= (double)m_MiddleLines.size();
    m_RightLineAverageSize /= (double)m_RightLines.size();
}

void LaneDetector::Get_Driving_State()
{
    int LeftLineType, MiddleLineType, RightLineType;

    if (m_LeftLines.empty())
        LeftLineType = m_LeftLineTypeRollingAverage.calculateRollingAverage(0);
    else if (m_LeftLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        LeftLineType = m_LeftLineTypeRollingAverage.calculateRollingAverage(1);
    else
        LeftLineType = m_LeftLineTypeRollingAverage.calculateRollingAverage(2);

    if (m_MiddleLines.empty())
        MiddleLineType = m_MiddleLineTypeRollingAverage.calculateRollingAverage(0);
    else if (m_MiddleLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        MiddleLineType = m_MiddleLineTypeRollingAverage.calculateRollingAverage(1);
    else
        MiddleLineType = m_MiddleLineTypeRollingAverage.calculateRollingAverage(2);

    if (m_RightLines.empty())
        RightLineType = m_RightLineTypeRollingAverage.calculateRollingAverage(0);
    else if (m_RightLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        RightLineType = m_RightLineTypeRollingAverage.calculateRollingAverage(1);
    else
        RightLineType = m_RightLineTypeRollingAverage.calculateRollingAverage(2);

    m_LeftLineTypesForDisplay.push_front(LeftLineType);
    m_LeftLineTypesForDisplay.pop_back();

    m_MiddleLineTypesForDisplay.push_front(MiddleLineType);
    m_MiddleLineTypesForDisplay.pop_back();

    m_RightLineTypesForDisplay.push_front(RightLineType);
    m_RightLineTypesForDisplay.pop_back();

    // Within Lanes
    if ((!m_LeftLines.empty() && !m_MiddleLines.empty() && !m_RightLines.empty()) ||
        (!m_LeftLines.empty() && m_MiddleLines.empty() && !m_RightLines.empty()))
        m_DrivingState = m_DrivingStateRollingAverage.calculateRollingAverage(0);

    // Changing Lanes
    else if ((m_LeftLines.empty() && !m_MiddleLines.empty() && m_RightLines.empty()) ||
             (!m_LeftLines.empty() && !m_MiddleLines.empty() && m_RightLines.empty()) ||
             (m_LeftLines.empty() && !m_MiddleLines.empty() && !m_RightLines.empty()))
        m_DrivingState = m_DrivingStateRollingAverage.calculateRollingAverage(1);

    // Only left road marking detected
    else if (!m_LeftLines.empty() && m_MiddleLines.empty() && m_RightLines.empty())
        m_DrivingState = m_DrivingStateRollingAverage.calculateRollingAverage(2);

    // Only right road marking detected
    else if (m_LeftLines.empty() && m_MiddleLines.empty() && !m_RightLines.empty())
        m_DrivingState = m_DrivingStateRollingAverage.calculateRollingAverage(3);

    // No road marking detected
    else
        m_DrivingState = m_DrivingStateRollingAverage.calculateRollingAverage(4);
}

void LaneDetector::Execute_Driving_State()
{
    switch (m_DrivingState)
    {
    case 0: // Within lanes
    {
        // Calculate the average distance to left edge, minimum y,
        // and average LeftLaneEdgeM and LeftLaneEdgeC
        double LeftLaneEdgeM = 0, LeftLaneEdgeC = 0, RightLaneEdgeM = 0, RightLaneEdgeC = 0;
        double AverageDistanceFromLeft = 0, AverageDistanceFromRight = 0;
        double LeftMinY = m_LeftLines[0][1], RightMinY = m_ROI_BOTTOM_HEIGHT, MinY;

        if (!m_LeftLines.empty())
        {
            double LeftX1, LeftX2;
            for (const auto &LeftLine : m_LeftLines)
            {
                LeftX1 = (LeftLine[1] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;
                LeftX2 = (LeftLine[3] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;

                AverageDistanceFromLeft += std::fabs(LeftLine[0] - LeftX1);
                AverageDistanceFromLeft += std::fabs(LeftLine[2] - LeftX2);

                if (LeftLine[1] < LeftMinY)
                    LeftMinY = LeftLine[1];

                if (LeftLine[3] < LeftMinY)
                    LeftMinY = LeftLine[3];

                // Find average m and c values for left lane
                LeftLaneEdgeM += (LeftLine[1] - LeftLine[3]) / (double)(LeftLine[0] - LeftLine[2]);
                LeftLaneEdgeC += LeftLine[1] - ((LeftLine[1] - LeftLine[3]) / (double)(LeftLine[0] - LeftLine[2])) * LeftLine[0];
            }

            AverageDistanceFromLeft /= (double)(m_LeftLines.size() * 2);
            LeftLaneEdgeM /= (double)(m_LeftLines.size());
            LeftLaneEdgeC /= (double)(m_LeftLines.size());
        }

        // Calculate the average distance to right edge, minimum y,
        // and average RightLaneEdgeM and RightLaneEdgeC
        if (!m_RightLines.empty())
        {
            double RightX1, RightX2;
            for (auto &RightLine : m_RightLines)
            {
                RightX1 = (RightLine[1] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;
                RightX2 = (RightLine[3] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;

                AverageDistanceFromRight += std::fabs(RightLine[0] - RightX1);
                AverageDistanceFromRight += std::fabs(RightLine[2] - RightX2);

                if (RightLine[1] < RightMinY)
                    RightMinY = RightLine[1];

                if (RightLine[3] < RightMinY)
                    RightMinY = RightLine[3];

                // Find average m and c values for right lane
                RightLaneEdgeM += (RightLine[1] - RightLine[3]) / (double)(RightLine[0] - RightLine[2]);
                RightLaneEdgeC += RightLine[1] - ((RightLine[1] - RightLine[3]) / (double)(RightLine[0] - RightLine[2])) * RightLine[0];
            }

            AverageDistanceFromRight /= (double)(m_RightLines.size() * 2);
            RightLaneEdgeM /= (double)(m_RightLines.size());
            RightLaneEdgeC /= (double)(m_RightLines.size());
        }

        // Next determine position of car using distances from left and right lane to the left and right edge
        double WithinLaneCurrentDifference;
        if ((AverageDistanceFromLeft - AverageDistanceFromRight) > 200)
            WithinLaneCurrentDifference = 1;
        else if ((AverageDistanceFromLeft - AverageDistanceFromRight) < -200)
            WithinLaneCurrentDifference = -1;
        else
            WithinLaneCurrentDifference = (AverageDistanceFromLeft - AverageDistanceFromRight) / 200.0;

        // Calculate the turning needed to return to centre to the nearest 10%
        m_TurningRequired = (int)(WithinLaneCurrentDifference * 100 - floor(WithinLaneCurrentDifference) * 100) % 10;

        // Calculate the direction of turning needed
        if (m_TurningRequired == 0)
            m_TurningRequiredToReturnToCenter = "In Centre";
        else if (m_TurningRequired < 0)
            m_TurningRequiredToReturnToCenter = "Turn Left " + std::to_string(-m_TurningRequired) + "%";
        else
            m_TurningRequiredToReturnToCenter = "Turn Right " + std::to_string(m_TurningRequired) + "%";

        // Draw the lane overlay
        if ((LeftLaneEdgeM != 0) && (RightLaneEdgeM != 0))
        {
            // Then plot line from ROI_BOTTOM_HEIGHT to the lowest MinY
            if (LeftMinY < RightMinY)
                MinY = LeftMinY;
            else
                MinY = RightMinY;

            // To prevent hour glass error, detect y value that lines intersect and if within overlay
            // region then skip printing overlay to screen as is error. This done by the following equation:
            //
            // y = (m2*c1 - m1*c2) / (m2-m1)
            //
            // where m1 and c1 are left lane edge and m2 and c2 are right lane edge
            int intersectionY = (RightLaneEdgeM * LeftLaneEdgeC - LeftLaneEdgeM * RightLaneEdgeC) / (RightLaneEdgeM - LeftLaneEdgeM);

            if (intersectionY < MinY)
            {
                // Add the four points of the quadrangle
                m_LanePoints[0] = cv::Point((MinY - LeftLaneEdgeC) / LeftLaneEdgeM, MinY);
                m_LanePoints[1] = cv::Point((MinY - RightLaneEdgeC) / RightLaneEdgeM, MinY);
                m_LanePoints[2] = cv::Point((m_ROI_BOTTOM_HEIGHT - RightLaneEdgeC) / RightLaneEdgeM, m_ROI_BOTTOM_HEIGHT);
                m_LanePoints[3] = cv::Point((m_ROI_BOTTOM_HEIGHT - LeftLaneEdgeC) / LeftLaneEdgeM, m_ROI_BOTTOM_HEIGHT);
                m_PrintLaneOverlay = true;
            }
            else
                m_PrintLaneOverlay = false;
        }
        else
            m_PrintLaneOverlay = false;

        m_TitleText = "Within Detected Lanes";
        m_RightInfoTitleText = "Detected Lanes";
        // Reset these to prevent errors
        m_ChangingLanesPreviousDifference = 0;
        m_ChangingLanesFrameCount = 0;
        m_CurrentTurningState.clear();
        break;
    }

    case 1: // Changing lanes
    {
        // Check to prevent divide by zero error
        if (!m_MiddleLines.empty())
        {
            double AverageDistanceFromLeft = 0, AverageDistanceFromRight = 0, ChangingLanesCurrentDifference;
            double LeftY1, LeftY2, RightY1, RightY2;

            // Calculate the average distance to the left and right edge of the middle lane
            for (const auto &MiddleLine : m_MiddleLines)
            {
                LeftY1 = (MiddleLine[1] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;
                LeftY2 = (MiddleLine[3] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;

                AverageDistanceFromLeft += std::fabs(MiddleLine[0] - LeftY1);
                AverageDistanceFromLeft += std::fabs(MiddleLine[2] - LeftY2);

                RightY1 = (MiddleLine[1] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;
                RightY2 = (MiddleLine[3] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;

                AverageDistanceFromRight += std::fabs(MiddleLine[0] - RightY1);
                AverageDistanceFromRight += std::fabs(MiddleLine[2] - RightY2);
            }

            AverageDistanceFromLeft /= (double)(m_MiddleLines.size() * 2);
            AverageDistanceFromRight /= (double)(m_MiddleLines.size() * 2);

            ChangingLanesCurrentDifference = AverageDistanceFromLeft - AverageDistanceFromRight;

            // To determine the direction the car is moving, multiple frames that are many frames apart need to be compared
            // to see a difference in lane position; thus, a frame count is used

            // Increment frame count and then check if the threshold met. If so, the current turning state is compared to the previous
            // turning state - which occurred FRAME_COUNT_THRESHOLD number of frames ago - and then determine the car's turning state and
            // update the previous difference and reset the counter.

            // this for if coming from a different driving state
            if (m_ChangingLanesFrameCount == 0)
                m_ChangingLanesPreviousDifference = ChangingLanesCurrentDifference;

            m_ChangingLanesFrameCount++;

            if (m_ChangingLanesFrameCount == m_FRAME_COUNT_THRESHOLD)
            {
                // Returns whether the car is turning left, right, or not turning based on
                // a current and previous difference, which is a value that represents the
                // difference between the distances from the left and right edge with respect
                // to the left and right road markings. The threshold value defines how big of
                // a difference between current and previous for the car to be detected as turning
                if ((m_ChangingLanesPreviousDifference - ChangingLanesCurrentDifference) < 0)
                    m_CurrentTurningState = "(Currently Turning Left)";
                else if ((m_ChangingLanesPreviousDifference - ChangingLanesCurrentDifference) > 0)
                    m_CurrentTurningState = "(Currently Turning Right)";
                else
                    m_CurrentTurningState = "(Currently Not Turning)";

                if (ChangingLanesCurrentDifference != 0)
                    m_ChangingLanesPreviousDifference = ChangingLanesCurrentDifference;

                m_ChangingLanesFrameCount = 0;
            }
        }

        m_TitleText = "WARNING: Changing lanes";
        m_RightInfoTitleText = "Detected Lanes";
        break;
    }

    case 2: // Only left road marking detected
    {
        m_TitleText = "WARNING: Only left road marking detected";
        m_RightInfoTitleText = "Detected Lanes";
        // Reset these to prevent errors
        m_ChangingLanesPreviousDifference = 0;
        m_ChangingLanesFrameCount = 0;
        m_CurrentTurningState.clear();
        break;
    }

    case 3: // Only right road marking detected
    {
        m_TitleText = "WARNING: Only right road marking detected";
        m_RightInfoTitleText = "Detected Lanes";
        // Reset these to prevent errors
        m_ChangingLanesPreviousDifference = 0;
        m_ChangingLanesFrameCount = 0;
        m_CurrentTurningState.clear();
        break;
    }

    case 4: // No road markings detected
    {
        m_TitleText = "WARNING: No road markings detected";
        m_RightInfoTitleText = "No Lanes Detected";
        // Reset these to prevent errors
        m_ChangingLanesPreviousDifference = 0;
        m_ChangingLanesFrameCount = 0;
        m_CurrentTurningState.clear();
        break;
    }

    default:
        std::cout << "\ncurrentDrivingState switch statement error: " << m_DrivingState;
        break;
    }
}

void LaneDetector::Print_To_Frame(cv::Mat &Frame)
{
    // title
    int Baseline = 0;
    cv::Size TextSize = cv::getTextSize(m_TitleText, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &Baseline);
    Baseline += FONT_THICKNESS;
    cv::Point TextOrg = cv::Point((m_VIDEO_WIDTH - TextSize.width) / 2.0, 25 + Baseline + TextSize.height);
    cv::rectangle(Frame, TextOrg + cv::Point(0, Baseline), TextOrg + cv::Point(TextSize.width, -TextSize.height - Baseline), cv::Scalar(0), cv::FILLED);
    cv::putText(Frame, m_TitleText, TextOrg, FONT_FACE, FONT_SCALE, cv::Scalar::all(255), FONT_THICKNESS, cv::LINE_AA);

    // Give way warning
    if (m_Give_Way_Warning)
    {
        std::string giveWayWarningText = "WARNING: Giveway ahead";
        Baseline = 0;
        TextSize = cv::getTextSize(giveWayWarningText, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &Baseline);
        Baseline += FONT_THICKNESS;

        TextOrg = cv::Point((m_VIDEO_WIDTH - TextSize.width) / 2.0, 225 + Baseline + TextSize.height);
        cv::rectangle(Frame, TextOrg + cv::Point(0, Baseline), TextOrg + cv::Point(TextSize.width, -TextSize.height - Baseline), cv::Scalar(0), cv::FILLED);
        cv::putText(Frame, giveWayWarningText, TextOrg, FONT_FACE, FONT_SCALE, cv::Scalar::all(255), FONT_THICKNESS, cv::LINE_AA);
    }

    // Right info box
    cv::rectangle(Frame, m_RIGHT_INFO_RECT, cv::Scalar(0), cv::FILLED, cv::LINE_AA, 0);

    // Right info box title
    Baseline = 0;
    TextSize = cv::getTextSize(m_RightInfoTitleText, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &Baseline);
    Baseline += FONT_THICKNESS;
    TextOrg = cv::Point((m_RIGHT_INFO_RECT.x + m_RIGHT_INFO_RECT.width / 2.0) - TextSize.width / 2.0, m_RIGHT_INFO_RECT.y + Baseline + TextSize.height);
    cv::putText(Frame, m_RightInfoTitleText, TextOrg, FONT_FACE, FONT_SCALE, cv::Scalar::all(255), FONT_THICKNESS, cv::LINE_AA);

    // Left line states
    for (int i = 0; i < m_LeftLineTypesForDisplay.size(); i++)
        cv::rectangle(Frame, cv::Rect(1595, 80 + i * 50, 4, 25 * m_LeftLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);
    // Right line states
    for (int i = 0; i < m_RightLineTypesForDisplay.size(); i++)
        cv::rectangle(Frame, cv::Rect(1795, 80 + i * 50, 4, 25 * m_RightLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);

    if (m_DrivingState == 0)
    {
        // Draw the yellow box that signifies the position of car with respect to the lanes detected
        m_BlankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, Frame.type());
        cv::rectangle(m_BlankFrame, cv::Rect(1695 - m_TurningRequired - 75, 205 - 100, 150, 200), cv::Scalar(0, 200, 200), cv::FILLED, cv::LINE_AA);
        cv::add(Frame, m_BlankFrame, Frame);

        // Write the turning needed to the screen
        Baseline = 0;
        TextSize = cv::getTextSize(m_TurningRequiredToReturnToCenter, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &Baseline);
        Baseline += FONT_THICKNESS;
        // center the text
        TextOrg = cv::Point(m_RIGHT_INFO_RECT.x + m_RIGHT_INFO_RECT.width / 2.0 - TextSize.width / 2.0, m_RIGHT_INFO_RECT.y + m_RIGHT_INFO_RECT.height + Baseline - TextSize.height);
        // then put the text itself
        cv::putText(Frame, m_TurningRequiredToReturnToCenter, TextOrg, FONT_FACE, FONT_SCALE, cv::Scalar::all(255), FONT_THICKNESS, cv::LINE_AA);

        if (m_PrintLaneOverlay)
        {
            // Make blank frame a blank black frame
            m_BlankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, Frame.type());

            cv::fillConvexPoly(m_BlankFrame, m_LanePoints, cv::Scalar(0, 64, 0), cv::LINE_AA, 0);

            // Can simply add the two images as the background in m_BlankFrame
            // is black (0,0,0) and so will not affect the frame image
            // while still being able to see tarmac
            cv::add(Frame, m_BlankFrame, Frame);
        }
    }

    else if (m_DrivingState == 1)
    {
        // Middle line type on RHS information box
        for (int i = 0; i < m_MiddleLineTypesForDisplay.size(); i++)
            cv::rectangle(Frame, cv::Rect(1695, 80 + i * 50, 4, 25 * m_MiddleLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);

        if (!m_CurrentTurningState.empty())
        {
            // Write the current turning state to screen
            Baseline = 0;
            TextSize = cv::getTextSize(m_CurrentTurningState, FONT_FACE, FONT_SCALE, FONT_THICKNESS, &Baseline);
            Baseline += FONT_THICKNESS;
            // center the text
            TextOrg = cv::Point((m_VIDEO_WIDTH - TextSize.width) / 2.0, 125 + Baseline + TextSize.height);
            // draw the box
            cv::rectangle(Frame, TextOrg + cv::Point(0, Baseline), TextOrg + cv::Point(TextSize.width, -TextSize.height - Baseline), cv::Scalar(0), cv::FILLED);
            // then put the text itself
            cv::putText(Frame, m_CurrentTurningState, TextOrg, FONT_FACE, FONT_SCALE, cv::Scalar::all(255), FONT_THICKNESS, cv::LINE_AA);
        }
    }
}
