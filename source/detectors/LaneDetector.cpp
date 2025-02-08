// NOLINTBEGIN

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/cvdef.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "detectors/LaneDetector.hpp"

namespace LaneAndObjectDetection
{
    void LaneDetector::RunDetector(const cv::Mat& p_frame, const std::vector<cv::Rect>& p_boundingBoxes)
    {
        Setup();

        GetHoughLines(p_frame);

        AnalyseHoughLines(p_boundingBoxes);

        GetDrivingState();

        ExecuteDrivingState();
    }

    void LaneDetector::Setup()
    {
        m_houghLines.clear();
        m_leftLines.clear();
        m_middleLines.clear();
        m_rightLines.clear();
        m_leftLineAverageSize = m_middleLineAverageSize = m_rightLineAverageSize = 0;
        m_printLaneOverlay = false;
    }

    void LaneDetector::GetHoughLines(const cv::Mat& p_frame)
    {
        // Get region of interest (ROI) frame by applying a mask on to the frame
        m_blankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, p_frame.type());
        cv::fillConvexPoly(m_blankFrame, m_MASK_DIMENSIONS, cv::Scalar(255, 255, 255), cv::LINE_AA, 0);
        cv::bitwise_and(m_blankFrame, p_frame, m_roiFrame);
        // Convert ROI frame to B&W
        cv::cvtColor(m_roiFrame, m_roiFrame, cv::COLOR_BGR2GRAY);
        // Get edges using Canny Algorithm on the ROI Frame
        cv::Canny(m_roiFrame, m_cannyFrame, m_CANNY_LOWER_THRESHOLD, m_CANNY_UPPER_THRESHOLD, 3, true);
        // Get straight lines using the Probabilistic Hough Transform (PHT) on the output of Canny Algorithm
        cv::HoughLinesP(m_cannyFrame, m_houghLines, 1, CV_PI / 180.0, m_HOUGH_THRESHOLD, m_HOUGH_MIN_LINE_LENGTH, m_HOUGH_MAX_LINE_GAP);
    }

    void LaneDetector::AnalyseHoughLines(const std::vector<cv::Rect>& p_boundingBoxes)
    {
        /**
         * The straight lines received from the PHT contain lines that are not a part of any
         * road markings (i.e. noise) and so this for loop tries to remove as much as them whilst
         * preserving the useful lines.
         *
         * For example, the ROI frame is the same size as the original frame (1920x1080) but is blank
         * expect for the locations within the area defined by m_MASK_DIMENSIONS (region of interest/ROI).
         * This means an edge between the black pixels and the contents within the ROI will be
         * picked up by the Canny Algorithm on the ROI Frame, which will be picked up by the
         * Probabilistic Hough Transform (PHT). Thus these lines (mask edge lines) must be removed.
         */
        double dx = NAN;
        double dy = NAN;
        double gradient = NAN;
        double leftY1 = NAN;
        double leftY2 = NAN;
        double rightY1 = NAN;
        double rightY2 = NAN;
        int lowerX = 0;
        int upperX = 0;
        int lowerY = 0;
        int upperY = 0;
        int horizontalCount = 0;
        bool lineIsInBoundingBox = false;
        for (const auto& houghLine : m_houghLines)
        {
            lineIsInBoundingBox = false;

            for (const auto& boundingBox : p_boundingBoxes)
            {
                lowerX = boundingBox.x;
                upperX = boundingBox.x + boundingBox.width;
                lowerY = boundingBox.y;
                upperY = boundingBox.y + boundingBox.height;

                if ((houghLine[0] >= lowerX && houghLine[0] <= upperX && houghLine[1] >= lowerY && houghLine[1] <= upperY) ||
                    (houghLine[2] >= lowerX && houghLine[2] <= upperX && houghLine[3] >= lowerY && houghLine[3] <= upperY))
                {
                    lineIsInBoundingBox = true;
                    break;
                }
            }

            /**
             * The bounding boxes are created by the object detector, which
             * cannot detect road markings, thus if the line is in a bounding
             * box, it cannot be a road marking and must be ignored.
             */
            if (lineIsInBoundingBox)
            {
                continue;
            }

            dx = houghLine[0] - houghLine[2];
            dy = houghLine[1] - houghLine[3];
            if (dx == 0)
            {
                continue;
            }
            gradient = dy / dx;

            /**
             * Lines are considered horizontal lines if they have:
             *    an absolute gradient below a certain gradient threshold
             *    a length above a threshold
             *    are not the top or bottom edge of the ROI frame
             */
            if (std::fabs(gradient) < m_HORIZONTAL_GRADIENT_THRESHOLD)
            {
                if ((std::sqrt((dy * dy) + (dx * dx)) > m_HORIZONTAL_LENGTH_THRESHOLD) &&
                    (((houghLine[1] >= m_ROI_TOP_HEIGHT + 1) && (houghLine[3] >= m_ROI_TOP_HEIGHT + 1)) || ((houghLine[1] <= m_ROI_BOTTOM_HEIGHT - 1) && (houghLine[3] <= m_ROI_BOTTOM_HEIGHT - 1))))
                {
                    horizontalCount++;
                }
            }

            /**
             * All lines with an absolute gradient above the gradient
             * threshold are considered for vertical lines
             */
            else
            {
                /**
                 *    Top edge of mask -->   ____________________
                 *                          /         / \        \
                 * Left edge of mask -->   /         /   \        \
                 *                        /         /     \        \
                 *                       /    #    /   @   \    &   \
                 *                      /         /         \        \   <-- Right edge of mask
                 *                     /         /           \        \
                 *                     ---------- ----------- ----------   <-- Bottom edge of mask
                 *                               ^           ^
                 *                            Left line    Right line
                 *                            threshold    threshold
                 *
                 * Any lines within:
                 *      '#' region will be considered left lines
                 *      '@' region will be considered middle lines
                 *      '&' region will be considered right lines
                 *
                 * Please note that, for OpenCV, the origin is located at the top left of the frame.
                 *
                 * Thus, left lines should have negative gradients and right positive gradients.
                 *
                 * Equations are used to determine the locations of two lines with respect to each other
                 */

                // Remove the left edge of the ROI frame
                leftY1 = m_LEFT_EDGE_OF_MASK_M * houghLine[0] + m_LEFT_EDGE_OF_MASK_C;
                leftY2 = m_LEFT_EDGE_OF_MASK_M * houghLine[2] + m_LEFT_EDGE_OF_MASK_C;
                if ((houghLine[1] <= leftY1 + 1) && (houghLine[3] <= leftY2 + 1))
                {
                    continue;
                }

                // If within left threshold and has a negative gradient, then it is a left line
                leftY1 = m_LEFT_EDGE_THRESHOLD_M * houghLine[0] + m_LEFT_EDGE_THRESHOLD_C;
                leftY2 = m_LEFT_EDGE_THRESHOLD_M * houghLine[2] + m_LEFT_EDGE_THRESHOLD_C;
                if ((houghLine[1] < leftY1) && (houghLine[3] < leftY2) && gradient < 0)
                {
                    m_leftLines.push_back(houghLine);
                    m_leftLineAverageSize += std::sqrt(((houghLine[0] - houghLine[2]) * (houghLine[0] - houghLine[2])) + ((houghLine[1] - houghLine[3]) * (houghLine[1] - houghLine[3])));
                    continue;
                }

                // Remove the right edge of the ROI frame
                rightY1 = m_RIGHT_EDGE_OF_MASK_M * houghLine[0] + m_RIGHT_EDGE_OF_MASK_C;
                rightY2 = m_RIGHT_EDGE_OF_MASK_M * houghLine[2] + m_RIGHT_EDGE_OF_MASK_C;
                if ((houghLine[1] <= rightY1 + 1) && (houghLine[3] <= rightY2 + 1))
                {
                    continue;
                }

                // If within right threshold and has a positive gradient, then it is a right line
                rightY1 = m_RIGHT_EDGE_THRESHOLD_M * houghLine[0] + m_RIGHT_EDGE_THRESHOLD_C;
                rightY2 = m_RIGHT_EDGE_THRESHOLD_M * houghLine[2] + m_RIGHT_EDGE_THRESHOLD_C;
                if ((houghLine[1] < rightY1) && (houghLine[3] < rightY2) && gradient > 0)
                {
                    m_rightLines.push_back(houghLine);
                    m_rightLineAverageSize += std::sqrt(((houghLine[0] - houghLine[2]) * (houghLine[0] - houghLine[2])) + ((houghLine[1] - houghLine[3]) * (houghLine[1] - houghLine[3])));
                    continue;
                }

                // Thus, the line must be a middle line
                m_middleLines.push_back(houghLine);
                m_middleLineAverageSize += std::sqrt(((houghLine[0] - houghLine[2]) * (houghLine[0] - houghLine[2])) + ((houghLine[1] - houghLine[3]) * (houghLine[1] - houghLine[3])));
            }
        }

        if (horizontalCount > m_HORIZONTAL_COUNT_THRESHOLD)
        {
            m_giveWayWarning = (m_horizontalLineStateRollingAverage.CalculateRollingAverage(1) != 0);
        }
        else
        {
            m_giveWayWarning = (m_horizontalLineStateRollingAverage.CalculateRollingAverage(0) != 0);
        }

        m_leftLineAverageSize /= static_cast<double>(m_leftLines.size());
        m_middleLineAverageSize /= static_cast<double>(m_middleLines.size());
        m_rightLineAverageSize /= static_cast<double>(m_rightLines.size());
    }

    void LaneDetector::GetDrivingState()
    {
        int leftLineType = 0;
        int middleLineType = 0;
        int rightLineType = 0;

        if (m_leftLines.empty())
        {
            leftLineType = m_leftLineTypeRollingAverage.CalculateRollingAverage(0);
        }
        else if (m_leftLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        {
            leftLineType = m_leftLineTypeRollingAverage.CalculateRollingAverage(1);
        }
        else
        {
            leftLineType = m_leftLineTypeRollingAverage.CalculateRollingAverage(2);
        }

        if (m_middleLines.empty())
        {
            middleLineType = m_middleLineTypeRollingAverage.CalculateRollingAverage(0);
        }
        else if (m_middleLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        {
            middleLineType = m_middleLineTypeRollingAverage.CalculateRollingAverage(1);
        }
        else
        {
            middleLineType = m_middleLineTypeRollingAverage.CalculateRollingAverage(2);
        }

        if (m_rightLines.empty())
        {
            rightLineType = m_rightLineTypeRollingAverage.CalculateRollingAverage(0);
        }
        else if (m_rightLineAverageSize < m_SOLID_LINE_LENGTH_THRESHOLD)
        {
            rightLineType = m_rightLineTypeRollingAverage.CalculateRollingAverage(1);
        }
        else
        {
            rightLineType = m_rightLineTypeRollingAverage.CalculateRollingAverage(2);
        }

        m_leftLineTypesForDisplay.push_front(leftLineType);
        m_leftLineTypesForDisplay.pop_back();

        m_middleLineTypesForDisplay.push_front(middleLineType);
        m_middleLineTypesForDisplay.pop_back();

        m_rightLineTypesForDisplay.push_front(rightLineType);
        m_rightLineTypesForDisplay.pop_back();

        /**
         * For left lanes lines, middle lane lines, and right lane lines '1' represents that
         * one or more of those lines has been detected and a '0' means no of those lines has
         * been detected. The driving state is used in the Execute_Driving_State() switch
         * statement.
         *
         * |----------------------------------------------------------------------------------------|
         * |  Left Lane  | Middle Lane | Right Lane | Driving State |      Driving State Name       |
         * |     Lines   |    Lines    |    Lines   |     Value     |                               |
         * |-------------|-------------|------------|---------------|-------------------------------|
         * |      1      |      1      |     1      |       0       |     Within Detected Lanes     |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      1      |      0      |     1      |       0       |     Within Detected Lanes     |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      0      |      1      |     0      |       1       |        Changing Lanes         |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      1      |      1      |     0      |       1       |        Changing Lanes         |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      0      |      1      |     1      |       1       |        Changing Lanes         |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      1      |      0      |     0      |       2       | Only left lane line detected  |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      0      |      0      |     1      |       3       | Only right lane line detected |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         * |      0      |      0      |     0      |       4       |    No lane lines detected     |
         * |-------------|---------- --|------------|---------------|-------------------------------|
         */

        // Within Lanes
        if ((!m_leftLines.empty() && !m_middleLines.empty() && !m_rightLines.empty()) ||
            (!m_leftLines.empty() && m_middleLines.empty() && !m_rightLines.empty()))
        {
            m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(0);

            // Changing Lanes
        }
        else if ((m_leftLines.empty() && !m_middleLines.empty() && m_rightLines.empty()) ||
                 (!m_leftLines.empty() && !m_middleLines.empty() && m_rightLines.empty()) ||
                 (m_leftLines.empty() && !m_middleLines.empty() && !m_rightLines.empty()))
        {
            m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(1);

            // Only left road marking detected
        }
        else if (!m_leftLines.empty() && m_middleLines.empty() && m_rightLines.empty())
        {
            m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(2);

            // Only right road marking detected
        }
        else if (m_leftLines.empty() && m_middleLines.empty() && !m_rightLines.empty())
        {
            m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(3);

            // No road marking detected
        }
        else
        {
            m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(4);
        }
    }

    void LaneDetector::ExecuteDrivingState()
    {
        switch (m_drivingState)
        {
        case 0: // Within lanes
        {
            // Calculate the average distance to left edge, minimum y,
            // and average LeftLaneEdgeM and LeftLaneEdgeC
            double leftLaneEdgeM = 0;
            double leftLaneEdgeC = 0;
            double rightLaneEdgeM = 0;
            double rightLaneEdgeC = 0;
            double averageDistanceFromLeft = 0;
            double averageDistanceFromRight = 0;
            double leftMinY = NAN;
            double rightMinY = m_ROI_BOTTOM_HEIGHT;
            double minY = NAN;

            if (!m_leftLines.empty())
            {
                leftMinY = m_leftLines[0][1]; // Bug - Initially was before if. IDK why.
                double leftX1 = NAN;
                double leftX2 = NAN;
                for (const auto& leftLine : m_leftLines)
                {
                    leftX1 = (leftLine[1] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;
                    leftX2 = (leftLine[3] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;

                    averageDistanceFromLeft += std::fabs(leftLine[0] - leftX1);
                    averageDistanceFromLeft += std::fabs(leftLine[2] - leftX2);

                    leftMinY = std::min<double>(leftLine[1], leftMinY);

                    leftMinY = std::min<double>(leftLine[3], leftMinY);

                    // Find average m and c values for left lane
                    leftLaneEdgeM += (leftLine[1] - leftLine[3]) / static_cast<double>(leftLine[0] - leftLine[2]);
                    leftLaneEdgeC += leftLine[1] - ((leftLine[1] - leftLine[3]) / static_cast<double>(leftLine[0] - leftLine[2])) * leftLine[0];
                }

                averageDistanceFromLeft /= static_cast<double>(m_leftLines.size() * 2);
                leftLaneEdgeM /= static_cast<double>(m_leftLines.size());
                leftLaneEdgeC /= static_cast<double>(m_leftLines.size());
            }

            // Calculate the average distance to right edge, minimum y,
            // and average RightLaneEdgeM and RightLaneEdgeC
            if (!m_rightLines.empty())
            {
                double rightX1 = NAN;
                double rightX2 = NAN;
                for (auto& rightLine : m_rightLines)
                {
                    rightX1 = (rightLine[1] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;
                    rightX2 = (rightLine[3] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;

                    averageDistanceFromRight += std::fabs(rightLine[0] - rightX1);
                    averageDistanceFromRight += std::fabs(rightLine[2] - rightX2);

                    rightMinY = std::min<double>(rightLine[1], rightMinY);

                    rightMinY = std::min<double>(rightLine[3], rightMinY);

                    // Find average m and c values for right lane
                    rightLaneEdgeM += (rightLine[1] - rightLine[3]) / static_cast<double>(rightLine[0] - rightLine[2]);
                    rightLaneEdgeC += rightLine[1] - ((rightLine[1] - rightLine[3]) / static_cast<double>(rightLine[0] - rightLine[2])) * rightLine[0];
                }

                averageDistanceFromRight /= static_cast<double>(m_rightLines.size() * 2);
                rightLaneEdgeM /= static_cast<double>(m_rightLines.size());
                rightLaneEdgeC /= static_cast<double>(m_rightLines.size());
            }

            // Next determine position of car using distances from left and right lane to the left and right edge
            double withinLaneCurrentDifference = NAN;
            if ((averageDistanceFromLeft - averageDistanceFromRight) > 200)
            {
                withinLaneCurrentDifference = 1;
            }
            else if ((averageDistanceFromLeft - averageDistanceFromRight) < -200)
            {
                withinLaneCurrentDifference = -1;
            }
            else
            {
                withinLaneCurrentDifference = (averageDistanceFromLeft - averageDistanceFromRight) / 200.0;
            }

            // Calculate the turning needed to return to centre to the nearest 10%
            m_turningRequired = static_cast<int>((withinLaneCurrentDifference * 100) - (floor(withinLaneCurrentDifference) * 100)) % 10;

            // Calculate the direction of turning needed
            if (m_turningRequired == 0)
            {
                m_turningRequiredToReturnToCenter = "In Centre";
            }
            else if (m_turningRequired < 0)
            {
                m_turningRequiredToReturnToCenter = "Turn Left " + std::to_string(-m_turningRequired) + "%";
            }
            else
            {
                m_turningRequiredToReturnToCenter = "Turn Right " + std::to_string(m_turningRequired) + "%";
            }

            // Draw the lane overlay
            if ((leftLaneEdgeM != 0) && (rightLaneEdgeM != 0))
            {
                // Then plot line from ROI_BOTTOM_HEIGHT to the lowest MinY
                if (leftMinY < rightMinY)
                {
                    minY = leftMinY;
                }
                else
                {
                    minY = rightMinY;
                }

                // To prevent hour glass error, detect y value that lines intersect and if within overlay
                // region then skip printing overlay to screen as is error. This done by the following equation:
                //
                // y = (m2*c1 - m1*c2) / (m2-m1)
                //
                // where m1 and c1 are left lane edge and m2 and c2 are right lane edge
                const int INTERSECTION_Y = (rightLaneEdgeM * leftLaneEdgeC - leftLaneEdgeM * rightLaneEdgeC) / (rightLaneEdgeM - leftLaneEdgeM);

                if (INTERSECTION_Y < minY)
                {
                    // Add the four points of the quadrangle
                    m_lanePoints[0] = cv::Point((minY - leftLaneEdgeC) / leftLaneEdgeM, minY);
                    m_lanePoints[1] = cv::Point((minY - rightLaneEdgeC) / rightLaneEdgeM, minY);
                    m_lanePoints[2] = cv::Point((m_ROI_BOTTOM_HEIGHT - rightLaneEdgeC) / rightLaneEdgeM, m_ROI_BOTTOM_HEIGHT);
                    m_lanePoints[3] = cv::Point((m_ROI_BOTTOM_HEIGHT - leftLaneEdgeC) / leftLaneEdgeM, m_ROI_BOTTOM_HEIGHT);
                    m_printLaneOverlay = true;
                }
                else
                {
                    m_printLaneOverlay = false;
                }
            }
            else
            {
                m_printLaneOverlay = false;
            }

            m_titleText = "Within Detected Lanes";
            m_rightInfoTitleText = "Detected Lanes";
            // Reset these to prevent errors
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        case 1: // Changing lanes
        {
            // Check to prevent divide by zero error
            if (!m_middleLines.empty())
            {
                double averageDistanceFromLeft = 0;
                double averageDistanceFromRight = 0;
                double changingLanesCurrentDifference = NAN;
                double leftY1 = NAN;
                double leftY2 = NAN;
                double rightY1 = NAN;
                double rightY2 = NAN;

                // Calculate the average distance to the left and right edge of the middle lane
                for (const auto& middleLine : m_middleLines)
                {
                    leftY1 = (middleLine[1] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;
                    leftY2 = (middleLine[3] - m_LEFT_EDGE_OF_MASK_C) / m_LEFT_EDGE_OF_MASK_M;

                    averageDistanceFromLeft += std::fabs(middleLine[0] - leftY1);
                    averageDistanceFromLeft += std::fabs(middleLine[2] - leftY2);

                    rightY1 = (middleLine[1] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;
                    rightY2 = (middleLine[3] - m_RIGHT_EDGE_OF_MASK_C) / m_RIGHT_EDGE_OF_MASK_M;

                    averageDistanceFromRight += std::fabs(middleLine[0] - rightY1);
                    averageDistanceFromRight += std::fabs(middleLine[2] - rightY2);
                }

                averageDistanceFromLeft /= static_cast<double>(m_middleLines.size() * 2);
                averageDistanceFromRight /= static_cast<double>(m_middleLines.size() * 2);

                changingLanesCurrentDifference = averageDistanceFromLeft - averageDistanceFromRight;

                // To determine the direction the car is moving, multiple frames that are many frames apart need to be compared
                // to see a difference in lane position; thus, a frame count is used

                // Increment frame count and then check if the threshold met. If so, the current turning state is compared to the previous
                // turning state - which occurred FRAME_COUNT_THRESHOLD number of frames ago - and then determine the car's turning state and
                // update the previous difference and reset the counter.

                // this for if coming from a different driving state
                if (m_changingLanesFrameCount == 0)
                {
                    m_changingLanesPreviousDifference = changingLanesCurrentDifference;
                }

                m_changingLanesFrameCount++;

                if (m_changingLanesFrameCount == m_FRAME_COUNT_THRESHOLD)
                {
                    // Returns whether the car is turning left, right, or not turning based on
                    // a current and previous difference, which is a value that represents the
                    // difference between the distances from the left and right edge with respect
                    // to the left and right road markings. The threshold value defines how big of
                    // a difference between current and previous for the car to be detected as turning
                    if ((m_changingLanesPreviousDifference - changingLanesCurrentDifference) < 0)
                    {
                        m_currentTurningState = "(Currently Turning Left)";
                    }
                    else if ((m_changingLanesPreviousDifference - changingLanesCurrentDifference) > 0)
                    {
                        m_currentTurningState = "(Currently Turning Right)";
                    }
                    else
                    {
                        m_currentTurningState = "(Currently Not Turning)";
                    }

                    if (changingLanesCurrentDifference != 0)
                    {
                        m_changingLanesPreviousDifference = changingLanesCurrentDifference;
                    }

                    m_changingLanesFrameCount = 0;
                }
            }

            m_titleText = "WARNING: Changing lanes";
            m_rightInfoTitleText = "Detected Lanes";
            break;
        }

        case 2: // Only left road marking detected
        {
            m_titleText = "WARNING: Only left road marking detected";
            m_rightInfoTitleText = "Detected Lanes";
            // Reset these to prevent errors
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        case 3: // Only right road marking detected
        {
            m_titleText = "WARNING: Only right road marking detected";
            m_rightInfoTitleText = "Detected Lanes";
            // Reset these to prevent errors
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        case 4: // No road markings detected
        {
            m_titleText = "WARNING: No road markings detected";
            m_rightInfoTitleText = "No Lanes Detected";
            // Reset these to prevent errors
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        default:
            std::cout << "\ncurrentDrivingState switch statement error: " << m_drivingState;
            break;
        }
    }

    void LaneDetector::PrintToFrame(cv::Mat& p_frame)
    {
        // Center Title
        // The next four lines are used to center the text horizontally and vertically
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(m_titleText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        cv::Point textOrg = cv::Point((m_VIDEO_WIDTH - textSize.width) / 2.0, 25 + baseline + textSize.height);
        cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), cv::Scalar(0), cv::FILLED);
        cv::putText(p_frame, m_titleText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);

        // Giveway warning
        if (m_giveWayWarning)
        {
            const std::string GIVE_WAY_WARNING_TEXT = "WARNING: Giveway ahead";
            // The next four lines are used to center the text horizontally and vertically
            baseline = 0;
            textSize = cv::getTextSize(GIVE_WAY_WARNING_TEXT, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
            baseline += m_FONT_THICKNESS;
            textOrg = cv::Point((m_VIDEO_WIDTH - textSize.width) / 2.0, 225 + baseline + textSize.height);
            cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), cv::Scalar(0), cv::FILLED);
            cv::putText(p_frame, GIVE_WAY_WARNING_TEXT, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
        }

        // Right-hand side info box and title
        cv::rectangle(p_frame, m_RIGHT_INFO_RECT, cv::Scalar(0), cv::FILLED, cv::LINE_AA, 0);
        // The next four lines are used to center the text horizontally and vertically
        baseline = 0;
        textSize = cv::getTextSize(m_rightInfoTitleText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        textOrg = cv::Point((m_RIGHT_INFO_RECT.x + m_RIGHT_INFO_RECT.width / 2.0) - (textSize.width / 2.0), m_RIGHT_INFO_RECT.y + baseline + textSize.height);
        cv::putText(p_frame, m_rightInfoTitleText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);

        // Left line states
        for (uint32_t i = 0; i < m_leftLineTypesForDisplay.size(); i++)
        {
            cv::rectangle(p_frame, cv::Rect(1595, 80 + (i * 50), 4, 25 * m_leftLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);
        }
        // Right line states
        for (uint32_t i = 0; i < m_rightLineTypesForDisplay.size(); i++)
        {
            cv::rectangle(p_frame, cv::Rect(1795, 80 + (i * 50), 4, 25 * m_rightLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);
        }

        if (m_drivingState == 0)
        {
            // Draw the yellow box that signifies the position of car with respect to the lanes detected
            m_blankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, p_frame.type());
            cv::rectangle(m_blankFrame, cv::Rect(1695 - m_turningRequired - 75, 205 - 100, 150, 200), cv::Scalar(0, 200, 200), cv::FILLED, cv::LINE_AA);
            cv::add(p_frame, m_blankFrame, p_frame);

            // Write the turning needed to the screen
            // The next four lines are used to center the text horizontally and vertically
            baseline = 0;
            textSize = cv::getTextSize(m_turningRequiredToReturnToCenter, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
            baseline += m_FONT_THICKNESS;
            textOrg = cv::Point(m_RIGHT_INFO_RECT.x + (m_RIGHT_INFO_RECT.width / 2.0) - (textSize.width / 2.0), m_RIGHT_INFO_RECT.y + m_RIGHT_INFO_RECT.height + baseline - textSize.height);
            cv::putText(p_frame, m_turningRequiredToReturnToCenter, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);

            if (m_printLaneOverlay)
            {
                // Make blank frame a blank black frame
                m_blankFrame = cv::Mat::zeros(m_VIDEO_HEIGHT, m_VIDEO_WIDTH, p_frame.type());

                cv::fillConvexPoly(m_blankFrame, m_lanePoints, cv::Scalar(0, 64, 0), cv::LINE_AA, 0);

                // Can simply add the two images as the background in m_blankFrame
                // is black (0,0,0) and so will not affect the frame image
                // while still being able to see tarmac
                cv::add(p_frame, m_blankFrame, p_frame);
            }
        }

        else if (m_drivingState == 1)
        {
            // Middle line type on RHS information box
            for (uint32_t i = 0; i < m_middleLineTypesForDisplay.size(); i++)
            {
                cv::rectangle(p_frame, cv::Rect(1695, 80 + (i * 50), 4, 25 * m_middleLineTypesForDisplay[i]), cv::Scalar(255, 255, 255), cv::FILLED, cv::LINE_AA);
            }

            if (!m_currentTurningState.empty())
            {
                // Write the current turning state to screen
                // The next four lines are used to center the text horizontally and vertically
                baseline = 0;
                textSize = cv::getTextSize(m_currentTurningState, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
                baseline += m_FONT_THICKNESS;
                textOrg = cv::Point((m_VIDEO_WIDTH - textSize.width) / 2.0, 125 + baseline + textSize.height);
                cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), cv::Scalar(0), cv::FILLED);
                cv::putText(p_frame, m_currentTurningState, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
            }
        }
    }
}

// NOLINTEND
