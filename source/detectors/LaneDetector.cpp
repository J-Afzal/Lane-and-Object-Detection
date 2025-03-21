#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "detectors/LaneDetector.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

namespace LaneAndObjectDetection
{
    LaneDetector::LaneDetector() :
        m_changingLanesFrameCount(0),
        m_drivingState(0),
        m_turningRequired(0),
        m_changingLanesPreviousDifference(0),
        m_leftLineAverageSize(0),
        m_middleLineAverageSize(0),
        m_rightLineAverageSize(0)
    {
        m_laneDetectionInformation.m_leftLineTypesForDisplay.insert(m_laneDetectionInformation.m_leftLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LINES_TO_DISPLAY, 0);
        m_laneDetectionInformation.m_middleLineTypesForDisplay.insert(m_laneDetectionInformation.m_middleLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LINES_TO_DISPLAY, 0);
        m_laneDetectionInformation.m_rightLineTypesForDisplay.insert(m_laneDetectionInformation.m_rightLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LINES_TO_DISPLAY, 0);
    }

    void LaneDetector::RunLaneDetector(const cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        cv::Mat cannyFrame;
        cv::Mat roiFrame;
        std::vector<cv::Vec4i> houghLines;

        // Get region of interest (ROI) frame by applying a mask on to the frame
        cv::Mat blankFrame = cv::Mat::zeros(Globals::G_INPUT_VIDEO_HEIGHT, Globals::G_INPUT_VIDEO_WIDTH, p_frame.type());
        cv::fillConvexPoly(blankFrame, Globals::G_ROI_MASK_POINTS, Globals::G_COLOUR_WHITE);
        cv::bitwise_and(blankFrame, p_frame, roiFrame);
        cv::cvtColor(roiFrame, roiFrame, cv::COLOR_BGR2GRAY);

        // Get edges using Canny Algorithm on the ROI Frame
        cv::Canny(roiFrame, cannyFrame, Globals::G_CANNY_ALGORITHM_LOWER_THRESHOLD, Globals::G_CANNY_ALGORITHM_UPPER_THRESHOLD);

        // Get straight lines using the Probabilistic Hough Transform (PHT) on the output of Canny Algorithm
        cv::HoughLinesP(cannyFrame, houghLines, Globals::G_HOUGH_RHO, Globals::G_HOUGH_THETA, Globals::G_HOUGH_THRESHOLD, Globals::G_HOUGH_MIN_LINE_LENGTH, Globals::G_HOUGH_MAX_LINE_GAP);

        AnalyseHoughLines(houghLines, p_objectDetectionInformation);

        UpdateLineTypes();

        UpdateDrivingState();

        ExecuteDrivingState();
    }

    LaneDetectionInformation LaneDetector::GetInformation()
    {
        return m_laneDetectionInformation;
    }

    LaneDetector::RollingAverage::RollingAverage()
    {
        m_rollingAverageArray.insert(m_rollingAverageArray.begin(), Globals::G_DEFAULT_ROLLING_AVERAGE_SIZE, 0);
    }

    uint32_t LaneDetector::RollingAverage::CalculateRollingAverage(const uint32_t& p_nextInput)
    {
        m_rollingAverageArray.pop_back();
        m_rollingAverageArray.push_front(p_nextInput);

        std::unordered_map<uint32_t, uint32_t> elementCount;
        for (const uint32_t& element : m_rollingAverageArray)
        {
            elementCount[element]++;
        }

        uint32_t mostFrequentElement = 0;
        uint32_t mostFrequentElementCount = 0;

        for (const std::pair<uint32_t, uint32_t> element : elementCount)
        {
            if (element.second > mostFrequentElementCount)
            {
                mostFrequentElement = element.first;
                mostFrequentElementCount = element.second;
            }
        }

        return mostFrequentElement;
    }

    void LaneDetector::AnalyseHoughLines(const std::vector<cv::Vec4i>& p_houghLines, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        m_leftLines.clear();
        m_middleLines.clear();
        m_rightLines.clear();
        m_leftLineAverageSize = 0;
        m_middleLineAverageSize = 0;
        m_rightLineAverageSize = 0;

        for (const cv::Vec4i& houghLine : p_houghLines)
        {
            const double CHANGE_IN_X = houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX];
            const double CHANGE_IN_Y = houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX];

            // Filter out vertical lines
            if (CHANGE_IN_X == 0)
            {
                continue;
            }

            const double GRADIENT = CHANGE_IN_Y / CHANGE_IN_X;

            // Filter out horizontal lines
            if (std::fabs(GRADIENT) < Globals::G_HOUGH_LINE_HORIZONTAL_GRADIENT_THRESHOLD)
            {
                continue;
            }

            // The bounding boxes are created by the object detector which cannot detect road markings. Thus, if the line is in
            // a bounding box, it cannot be a road marking and is ignored.
            if (IsLineWithinBoundingBoxes(houghLine, p_objectDetectionInformation))
            {
                continue;
            }

            // ROI frame:
            //
            //    Top edge of mask -->  ____________________
            //                         /         / \        \
            // Left edge of mask -->  /         /   \        \
            //                       /         /     \        \
            //                      /    #    /   @   \    &   \
            //                     /         /         \        \  <-- Right edge of mask
            //                    /         /           \        \
            //                    ---------- ----------- ----------  <-- Bottom edge of mask
            //        Left line threshold  ^             ^ Right line threshold
            //
            // Any lines within:
            //      '#' region will be considered left lines
            //      '@' region will be considered middle lines
            //      '&' region will be considered right lines
            //
            // Within OpenCV, the origin is located at the top left of the frame. Thus, left lines should have negative
            // gradients and right positive gradients.

            // Remove the left edge of the ROI frame
            double y1 = Globals::G_LEFT_EDGE_OF_MASK_M * houghLine[Globals::G_HOUGH_LINE_X1_INDEX] + Globals::G_LEFT_EDGE_OF_MASK_C;
            double y2 = Globals::G_LEFT_EDGE_OF_MASK_M * houghLine[2] + Globals::G_LEFT_EDGE_OF_MASK_C;
            if ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] <= y1 + 1) && (houghLine[Globals::G_HOUGH_LINE_Y2_INDEX] <= y2 + 1)) // TODO: why +1
            {
                continue;
            }

            // If within left threshold and has a negative gradient (meaning the line follows the expected path shown in the
            // diagram above), then it is a left line
            y1 = Globals::G_LEFT_LINE_THRESHOLD_M * houghLine[Globals::G_HOUGH_LINE_X1_INDEX] + Globals::G_LEFT_LINE_THRESHOLD_C;
            y2 = Globals::G_LEFT_LINE_THRESHOLD_M * houghLine[2] + Globals::G_LEFT_LINE_THRESHOLD_C;
            if ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] < y1) && (houghLine[Globals::G_HOUGH_LINE_Y2_INDEX] < y2) && GRADIENT < 0)
            {
                m_leftLines.push_back(houghLine);
                m_leftLineAverageSize += std::sqrt(
                    ((houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX])) +
                    ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX])));
                continue;
            }

            // Remove the right edge of the ROI frame
            y1 = Globals::G_RIGHT_EDGE_OF_MASK_M * houghLine[Globals::G_HOUGH_LINE_X1_INDEX] + Globals::G_RIGHT_EDGE_OF_MASK_C;
            y2 = Globals::G_RIGHT_EDGE_OF_MASK_M * houghLine[Globals::G_HOUGH_LINE_X2_INDEX] + Globals::G_RIGHT_EDGE_OF_MASK_C;
            if ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] <= y1 + 1) && (houghLine[Globals::G_HOUGH_LINE_Y2_INDEX] <= y2 + 1)) // TODO: why +1
            {
                continue;
            }

            // If within right threshold and has a positive gradient (meaning the line follows the expected path shown in the
            // diagram above), then it is a right line
            y1 = Globals::G_RIGHT_LINE_THRESHOLD_M * houghLine[Globals::G_HOUGH_LINE_X1_INDEX] + Globals::G_RIGHT_LINE_THRESHOLD_C;
            y2 = Globals::G_RIGHT_LINE_THRESHOLD_M * houghLine[Globals::G_HOUGH_LINE_X2_INDEX] + Globals::G_RIGHT_LINE_THRESHOLD_C;
            if ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] < y1) && (houghLine[Globals::G_HOUGH_LINE_Y2_INDEX] < y2) && GRADIENT > 0)
            {
                m_rightLines.push_back(houghLine);
                m_rightLineAverageSize += std::sqrt(
                    ((houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX])) +
                    ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX])));
                continue;
            }

            // Otherwise the line is be a middle line
            m_middleLines.push_back(houghLine);
            m_middleLineAverageSize += std::sqrt(
                ((houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_X1_INDEX] - houghLine[Globals::G_HOUGH_LINE_X2_INDEX])) +
                ((houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX]) * (houghLine[Globals::G_HOUGH_LINE_Y1_INDEX] - houghLine[Globals::G_HOUGH_LINE_Y2_INDEX])));
        }

        m_leftLineAverageSize /= static_cast<double>(m_leftLines.size());
        m_middleLineAverageSize /= static_cast<double>(m_middleLines.size());
        m_rightLineAverageSize /= static_cast<double>(m_rightLines.size());
    }

    bool LaneDetector::IsLineWithinBoundingBoxes(const cv::Vec4i& p_houghLine, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        for (const auto& objectInformation : p_objectDetectionInformation.m_objectInformation)
        {
            int32_t lowerX = objectInformation.m_boundingBox.x;
            int32_t upperX = objectInformation.m_boundingBox.x + objectInformation.m_boundingBox.width;
            int32_t lowerY = objectInformation.m_boundingBox.y;
            int32_t upperY = objectInformation.m_boundingBox.y + objectInformation.m_boundingBox.height;

            // The OR operator means that only whole line does not need to be within the bounding box for the line to be
            // considered within the bounding boxes
            if ((p_houghLine[0] >= lowerX && p_houghLine[0] <= upperX && p_houghLine[1] >= lowerY && p_houghLine[1] <= upperY) ||
                (p_houghLine[2] >= lowerX && p_houghLine[2] <= upperX && p_houghLine[3] >= lowerY && p_houghLine[3] <= upperY))
            {
                return true;
            }
        }

        return false;
    }

    void LaneDetector::UpdateLineTypes()
    {
        LineType leftLineType;
        if (m_leftLines.empty())
        {
            leftLineType = LineType::EMPTY;
        }
        else if (m_leftLineAverageSize < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            leftLineType = LineType::DASHED;
        }
        else
        {
            leftLineType = LineType::SOLID;
        }

        LineType middleLineType;
        if (m_middleLines.empty())
        {
            middleLineType = LineType::EMPTY;
        }
        else if (m_middleLineAverageSize < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            middleLineType = LineType::DASHED;
        }
        else
        {
            middleLineType = LineType::SOLID;
        }

        LineType rightLineType;
        if (m_rightLines.empty())
        {
            rightLineType = LineType::EMPTY;
        }
        else if (m_rightLineAverageSize < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            rightLineType = LineType::DASHED;
        }
        else
        {
            rightLineType = LineType::SOLID;
        }

        m_laneDetectionInformation.m_leftLineTypesForDisplay.push_front(m_leftLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(leftLineType)));
        m_laneDetectionInformation.m_leftLineTypesForDisplay.pop_back();

        m_laneDetectionInformation.m_middleLineTypesForDisplay.push_front(m_middleLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(middleLineType)));
        m_laneDetectionInformation.m_middleLineTypesForDisplay.pop_back();

        m_laneDetectionInformation.m_rightLineTypesForDisplay.push_front(m_rightLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(rightLineType)));
        m_laneDetectionInformation.m_rightLineTypesForDisplay.pop_back();
    }

    void LaneDetector::UpdateDrivingState()
    {
        // Driving state mapping:
        //
        // | Left Lane Lines | Middle Lane Lines | Right Lane Lines |           Driving State          |
        // | --------------- |------------------ | ---------------- | -------------------------------- |
        // |    Detected     |      Detected     |  Detected        |            WITHIN_LANE           |
        // |    Detected     |                   |  Detected        |            WITHIN_LANE           |
        // |                 |      Detected     |                  |          CHANGING_LANES          |
        // |    Detected     |      Detected     |                  |          CHANGING_LANES          |
        // |                 |      Detected     |  Detected        |          CHANGING_LANES          |
        // |    Detected     |                   |                  |  ONLY_LEFT_LANE_MARKING_DETECTED |
        // |                 |                   |  Detected        | ONLY_RIGHT_LANE_MARKING_DETECTED |
        // |                 |                   |                  |     NO_LANE_MARKINGS_DETECTED    |

        const bool LEFT_LINES_PRESENT = !m_leftLines.empty();
        const bool MIDDLE_LINES_PRESENT = !m_middleLines.empty();
        const bool RIGHT_LINES_PRESNET = !m_rightLines.empty();

        DrivingState currentDrivingState;

        if ((LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET) ||
            (LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET))
        {
            currentDrivingState = DrivingState::WITHIN_LANE;
        }
        else if ((!LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET) ||
                 (LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET) ||
                 (!LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET))
        {
            currentDrivingState = DrivingState::CHANGING_LANES;
        }
        else if (LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET)
        {
            currentDrivingState = DrivingState::ONLY_LEFT_LANE_MARKING_DETECTED;
        }
        else if (!LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET)
        {
            currentDrivingState = DrivingState::ONLY_RIGHT_LANE_MARKING_DETECTED;
        }
        else
        {
            currentDrivingState = DrivingState::NO_LANE_MARKINGS_DETECTED;
        }

        m_laneDetectionInformation.m_drivingState = m_drivingStateRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(currentDrivingState));


        // TODO: Some way to auto update below. Maybe fixed global array? and index it using driving state. but then don't need these vars
        // as frame builder could also access that?
        // m_drivingStateTitle = "Within Detected Lanes";
        // m_laneInformationTitle = "Detected Lanes";
    }

    void LaneDetector::ExecuteDrivingState()
    {
        // TODO: do lane_points all need clearing before use?

        switch (m_drivingState)
        {
        case 0: // Within lanes
        {
            // Calculate the average distance to left edge, minimum y,
            // and average LeftLaneEdgeM and LeftLaneEdgeC
            double averageDistanceFromLeft = 0;
            double averageDistanceFromRight = 0;
            double leftLaneEdgeM = 0;
            double leftLaneEdgeC = 0;
            double leftMinY = NAN;
            double minY = NAN;
            double rightLaneEdgeM = 0;
            double rightLaneEdgeC = 0;
            double rightMinY = Globals::G_ROI_BOTTOM_HEIGHT;

            if (!m_leftLines.empty())
            {
                leftMinY = m_leftLines[0][1]; // Bug - Initially was before if. IDK why.
                double leftX1 = NAN;
                double leftX2 = NAN;
                for (const auto& leftLine : m_leftLines)
                {
                    leftX1 = (leftLine[1] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;
                    leftX2 = (leftLine[3] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;

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
                    rightX1 = (rightLine[1] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;
                    rightX2 = (rightLine[3] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;

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
            const double MAXIMUM_DIFFERENCE = 200;
            const double MINIMUM_DIFFERENCE = -200;
            double withinLaneCurrentDifference = NAN;
            if ((averageDistanceFromLeft - averageDistanceFromRight) > MAXIMUM_DIFFERENCE)
            {
                withinLaneCurrentDifference = 1;
            }
            else if ((averageDistanceFromLeft - averageDistanceFromRight) < MINIMUM_DIFFERENCE)
            {
                withinLaneCurrentDifference = -1;
            }
            else
            {
                withinLaneCurrentDifference = (averageDistanceFromLeft - averageDistanceFromRight) / MAXIMUM_DIFFERENCE;
            }

            // Calculate the turning needed to return to centre to the nearest 10% // TODO: fix this as currently always small number
            const int32_t CONVERT_TO_PERCENTAGE = 100;
            const int32_t ROUNDING = 10;
            m_turningRequired = static_cast<int32_t>((withinLaneCurrentDifference * CONVERT_TO_PERCENTAGE) - (floor(withinLaneCurrentDifference) * CONVERT_TO_PERCENTAGE)) % ROUNDING;

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
                const int INTERSECTION_Y = static_cast<int32_t>((rightLaneEdgeM * leftLaneEdgeC - leftLaneEdgeM * rightLaneEdgeC) / (rightLaneEdgeM - leftLaneEdgeM));

                if (INTERSECTION_Y < minY)
                {
                    // Add the four points of the quadrangle
                    m_lanePoints[0] = cv::Point(static_cast<int32_t>((minY - leftLaneEdgeC) / leftLaneEdgeM), static_cast<int32_t>(minY));
                    m_lanePoints[1] = cv::Point(static_cast<int32_t>((minY - rightLaneEdgeC) / rightLaneEdgeM), static_cast<int32_t>(minY));
                    m_lanePoints[2] = cv::Point(static_cast<int32_t>((Globals::G_ROI_BOTTOM_HEIGHT - rightLaneEdgeC) / rightLaneEdgeM), Globals::G_ROI_BOTTOM_HEIGHT);
                    m_lanePoints[3] = cv::Point(static_cast<int32_t>((Globals::G_ROI_BOTTOM_HEIGHT - leftLaneEdgeC) / leftLaneEdgeM), Globals::G_ROI_BOTTOM_HEIGHT);
                }
                else
                {
                    m_lanePoints = {
                        {0, 0},
                        {0, 0},
                        {0, 0},
                        {0, 0}
                    };
                }
            }
            else
            {
                m_lanePoints = {
                    {0, 0},
                    {0, 0},
                    {0, 0},
                    {0, 0}
                };
            }

            m_drivingStateTitle = "Within Detected Lanes";
            m_laneInformationTitle = "Detected Lanes";
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
                    leftY1 = (middleLine[1] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;
                    leftY2 = (middleLine[3] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;

                    averageDistanceFromLeft += std::fabs(middleLine[0] - leftY1);
                    averageDistanceFromLeft += std::fabs(middleLine[2] - leftY2);

                    rightY1 = (middleLine[1] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;
                    rightY2 = (middleLine[3] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;

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

                if (m_changingLanesFrameCount == Globals::G_FRAME_COUNT_THRESHOLD)
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

            m_drivingStateTitle = "WARNING: Changing lanes";
            m_laneInformationTitle = "Detected Lanes";
            m_lanePoints = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}
            };
            break;
        }

        case 2: // Only left road marking detected
        {
            m_drivingStateTitle = "WARNING: Only left road marking detected";
            m_laneInformationTitle = "Detected Lanes";
            // Reset these to prevent errors
            m_lanePoints = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}
            };
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        case 3: // Only right road marking detected
        {
            m_drivingStateTitle = "WARNING: Only right road marking detected";
            m_laneInformationTitle = "Detected Lanes";
            // Reset these to prevent errors
            m_lanePoints = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}
            };
            m_changingLanesPreviousDifference = 0;
            m_changingLanesFrameCount = 0;
            m_currentTurningState.clear();
            break;
        }

        case 4: // No road markings detected
        {
            m_drivingStateTitle = "WARNING: No road markings detected";
            m_laneInformationTitle = "No Lanes Detected";
            // Reset these to prevent errors
            m_lanePoints = {
                {0, 0},
                {0, 0},
                {0, 0},
                {0, 0}
            };
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
}
