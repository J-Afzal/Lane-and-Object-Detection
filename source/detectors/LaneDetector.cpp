#include <algorithm>
#include <cmath>
#include <cstdint>
#include <deque>
#include <format>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/matx.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "detectors/LaneDetector.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

namespace LaneAndObjectDetection
{
    LaneDetector::LaneDetector() :
        m_changingLanesPreviousDistanceDifference(0),
        m_leftLineAverageLength(0),
        m_middleLineAverageLength(0),
        m_rightLineAverageLength(0),
        m_changingLanesFrameCount(0),
        m_changingLanesFirstFrame(false)
    {
        m_laneDetectionInformation.m_leftLineTypesForDisplay.insert(m_laneDetectionInformation.m_leftLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LANE_LINES_TO_DISPLAY, 0);
        m_laneDetectionInformation.m_middleLineTypesForDisplay.insert(m_laneDetectionInformation.m_middleLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LANE_LINES_TO_DISPLAY, 0);
        m_laneDetectionInformation.m_rightLineTypesForDisplay.insert(m_laneDetectionInformation.m_rightLineTypesForDisplay.begin(), Globals::G_NUMBER_OF_LANE_LINES_TO_DISPLAY, 0);
    }

    void LaneDetector::RunLaneDetector(const cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        // Get region of interest (ROI) frame by applying a mask on to the frame
        cv::Mat blankFrame = cv::Mat::zeros(Globals::G_VIDEO_INPUT_HEIGHT, Globals::G_VIDEO_INPUT_WIDTH, p_frame.type());
        cv::fillConvexPoly(blankFrame, Globals::G_ROI_MASK_POINTS, Globals::G_COLOUR_WHITE);
        cv::bitwise_and(blankFrame, p_frame, m_laneDetectionInformation.m_roiFrame);
        cv::cvtColor(m_laneDetectionInformation.m_roiFrame, m_laneDetectionInformation.m_roiFrame, cv::COLOR_BGR2GRAY);

        // Get edges using Canny Algorithm on the ROI Frame
        cv::Canny(m_laneDetectionInformation.m_roiFrame, m_laneDetectionInformation.m_cannyFrame, Globals::G_CANNY_ALGORITHM_LOWER_THRESHOLD, Globals::G_CANNY_ALGORITHM_UPPER_THRESHOLD);

        m_laneDetectionInformation.m_houghLinesFrame = cv::Mat::zeros(Globals::G_VIDEO_INPUT_HEIGHT, Globals::G_VIDEO_INPUT_WIDTH, p_frame.type());

        // Get straight lines using the Probabilistic Hough Transform (PHT) on the output of Canny Algorithm
        std::vector<cv::Vec4i> houghLines;
        cv::HoughLinesP(m_laneDetectionInformation.m_cannyFrame, houghLines, Globals::G_HOUGH_RHO, Globals::G_HOUGH_THETA, Globals::G_HOUGH_THRESHOLD, Globals::G_HOUGH_MIN_LINE_LENGTH, Globals::G_HOUGH_MAX_LINE_GAP);

        AnalyseHoughLines(houghLines, p_objectDetectionInformation);

        UpdateLineTypes();

        UpdateDrivingState();

        // Set titles based on driving state
        m_laneDetectionInformation.m_drivingStateTitle = Globals::G_DRIVING_STATE_TITLES.find(m_laneDetectionInformation.m_drivingState)->second;
        m_laneDetectionInformation.m_laneInformationTitle = m_laneDetectionInformation.m_drivingState == Globals::DrivingState::NO_LANE_MARKINGS_DETECTED ? Globals::G_LANE_INFORMATION_TITLE_LANE_NOT_DETECTED :
                                                                                                                                                            Globals::G_LANE_INFORMATION_TITLE_LANE_DETECTED;
        switch (m_laneDetectionInformation.m_drivingState)
        {
        case Globals::DrivingState::WITHIN_LANE:
            CalculateLanePosition();
            break;

        case Globals::DrivingState::CHANGING_LANES:
            CalculateTurningDirection();
            break;

        case Globals::DrivingState::ONLY_LEFT_LANE_MARKING_DETECTED:
        case Globals::DrivingState::ONLY_RIGHT_LANE_MARKING_DETECTED:
        case Globals::DrivingState::NO_LANE_MARKINGS_DETECTED:
            break;

        default:
            throw Globals::Exceptions::NotImplementedError();
        }
    }

    LaneDetectionInformation LaneDetector::GetInformation()
    {
        return m_laneDetectionInformation;
    }

    template<typename T>
    LaneDetector::RollingAverage<T>::RollingAverage()
    {
        m_rollingAverageArray.insert(m_rollingAverageArray.begin(), Globals::G_DEFAULT_ROLLING_AVERAGE_SIZE, static_cast<T>(0));
    }

    template<typename T>
    T LaneDetector::RollingAverage<T>::CalculateRollingAverage(const T& p_nextInput)
    {
        m_rollingAverageArray.pop_back();
        m_rollingAverageArray.push_front(p_nextInput);

        std::unordered_map<T, uint32_t> elementCount;
        for (const T& element : m_rollingAverageArray)
        {
            elementCount[element]++;
        }

        T mostFrequentElement = static_cast<T>(0);
        uint32_t mostFrequentElementCount = 0;

        for (const std::pair<T, uint32_t> element : elementCount) // NOLINT(readability-identifier-naming)
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
        m_leftLaneLines.clear();
        m_middleLaneLines.clear();
        m_rightLaneLines.clear();
        m_leftLineAverageLength = 0;
        m_middleLineAverageLength = 0;
        m_rightLineAverageLength = 0;

        for (const cv::Vec4i& houghLine : p_houghLines)
        {
            const cv::Point POINT_ONE = cv::Point(houghLine[Globals::G_VEC4_X1_INDEX], houghLine[Globals::G_VEC4_Y1_INDEX]);
            const cv::Point POINT_TWO = cv::Point(houghLine[Globals::G_VEC4_X2_INDEX], houghLine[Globals::G_VEC4_Y2_INDEX]);

            const double CHANGE_IN_X = houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX];
            const double CHANGE_IN_Y = houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX];

            // Filter out vertical lines
            if (CHANGE_IN_X == 0)
            {
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_WHITE, cv::FILLED, cv::LINE_AA);
                continue;
            }

            const double GRADIENT = CHANGE_IN_Y / CHANGE_IN_X;

            // Filter out horizontal lines
            if (std::fabs(GRADIENT) < Globals::G_HOUGH_LINE_HORIZONTAL_GRADIENT_THRESHOLD)
            {
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_GREY, cv::FILLED, cv::LINE_AA);
                continue;
            }

            // The bounding boxes are created by the object detector which cannot detect road markings. Thus, if the line is in
            // a bounding box, it cannot be a road marking and is ignored.
            if (IsLineWithinObjectBoundingBoxes(houghLine, p_objectDetectionInformation))
            {
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_YELLOW, cv::FILLED, cv::LINE_AA);
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
            const double LEFT_EDGE_Y1 = (Globals::G_LEFT_EDGE_OF_MASK_M * houghLine[Globals::G_VEC4_X1_INDEX]) + Globals::G_LEFT_EDGE_OF_MASK_C;
            const double LEFT_EDGE_Y2 = (Globals::G_LEFT_EDGE_OF_MASK_M * houghLine[Globals::G_VEC4_X2_INDEX]) + Globals::G_LEFT_EDGE_OF_MASK_C;
            if ((houghLine[Globals::G_VEC4_Y1_INDEX] <= LEFT_EDGE_Y1 + 1) && (houghLine[Globals::G_VEC4_Y2_INDEX] <= LEFT_EDGE_Y2 + 1)) // +1 for extra buffer
            {
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_LIGHT_RED, cv::FILLED, cv::LINE_AA);
                continue;
            }

            // If within left threshold and has a negative gradient (meaning the line follows the expected path shown in the
            // diagram above), then it is a left line
            const double LEFT_THRESHOLD_Y1 = (Globals::G_LEFT_LINE_THRESHOLD_M * houghLine[Globals::G_VEC4_X1_INDEX]) + Globals::G_LEFT_LINE_THRESHOLD_C;
            const double LEFT_THRESHOLD_Y2 = (Globals::G_LEFT_LINE_THRESHOLD_M * houghLine[Globals::G_VEC4_X2_INDEX]) + Globals::G_LEFT_LINE_THRESHOLD_C;
            if ((houghLine[Globals::G_VEC4_Y1_INDEX] < LEFT_THRESHOLD_Y1) && (houghLine[Globals::G_VEC4_Y2_INDEX] < LEFT_THRESHOLD_Y2) && GRADIENT < 0)
            {
                m_leftLaneLines.push_back(houghLine);
                m_leftLineAverageLength += std::sqrt(
                    ((houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX]) * (houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX])) +
                    ((houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX]) * (houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX])));
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_RED, cv::FILLED, cv::LINE_AA);
                continue;
            }

            // Remove the right edge of the ROI frame
            const double RIGHT_EDGE_Y1 = (Globals::G_RIGHT_EDGE_OF_MASK_M * houghLine[Globals::G_VEC4_X1_INDEX]) + Globals::G_RIGHT_EDGE_OF_MASK_C;
            const double RIGHT_EDGE_Y2 = (Globals::G_RIGHT_EDGE_OF_MASK_M * houghLine[Globals::G_VEC4_X2_INDEX]) + Globals::G_RIGHT_EDGE_OF_MASK_C;
            if ((houghLine[Globals::G_VEC4_Y1_INDEX] <= RIGHT_EDGE_Y1 + 1) && (houghLine[Globals::G_VEC4_Y2_INDEX] <= RIGHT_EDGE_Y2 + 1)) // +1 for extra buffer
            {
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_LIGHT_BLUE, cv::FILLED, cv::LINE_AA);
                continue;
            }

            // If within right threshold and has a positive gradient (meaning the line follows the expected path shown in the
            // diagram above), then it is a right line
            const double RIGHT_THRESHOLD_Y1 = (Globals::G_RIGHT_LINE_THRESHOLD_M * houghLine[Globals::G_VEC4_X1_INDEX]) + Globals::G_RIGHT_LINE_THRESHOLD_C;
            const double RIGHT_THRESHOLD_Y2 = (Globals::G_RIGHT_LINE_THRESHOLD_M * houghLine[Globals::G_VEC4_X2_INDEX]) + Globals::G_RIGHT_LINE_THRESHOLD_C;
            if ((houghLine[Globals::G_VEC4_Y1_INDEX] < RIGHT_THRESHOLD_Y1) && (houghLine[Globals::G_VEC4_Y2_INDEX] < RIGHT_THRESHOLD_Y2) && GRADIENT > 0)
            {
                m_rightLaneLines.push_back(houghLine);
                m_rightLineAverageLength += std::sqrt(
                    ((houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX]) * (houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX])) +
                    ((houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX]) * (houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX])));
                cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_BLUE, cv::FILLED, cv::LINE_AA);

                continue;
            }

            // Otherwise the line is be a middle line
            m_middleLaneLines.push_back(houghLine);
            m_middleLineAverageLength += std::sqrt(
                ((houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX]) * (houghLine[Globals::G_VEC4_X1_INDEX] - houghLine[Globals::G_VEC4_X2_INDEX])) +
                ((houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX]) * (houghLine[Globals::G_VEC4_Y1_INDEX] - houghLine[Globals::G_VEC4_Y2_INDEX])));
            cv::line(m_laneDetectionInformation.m_houghLinesFrame, POINT_ONE, POINT_TWO, Globals::G_COLOUR_GREEN, cv::FILLED, cv::LINE_AA);
        }

        m_leftLineAverageLength /= static_cast<double>(m_leftLaneLines.size());
        m_middleLineAverageLength /= static_cast<double>(m_middleLaneLines.size());
        m_rightLineAverageLength /= static_cast<double>(m_rightLaneLines.size());
    }

    bool LaneDetector::IsLineWithinObjectBoundingBoxes(const cv::Vec4i& p_houghLine, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        for (const ObjectDetectionInformation::DetectedObjectInformation& detectedObjectInformation : p_objectDetectionInformation.m_objectInformation)
        {
            const int32_t LOWER_X = detectedObjectInformation.m_boundingBox.x;
            const int32_t UPPER_X = detectedObjectInformation.m_boundingBox.x + detectedObjectInformation.m_boundingBox.width;
            const int32_t LOWER_Y = detectedObjectInformation.m_boundingBox.y;
            const int32_t UPPER_Y = detectedObjectInformation.m_boundingBox.y + detectedObjectInformation.m_boundingBox.height;

            // The OR operator means that only whole line does not need to be within the bounding box for the line to be
            // considered within the bounding boxes
            if ((p_houghLine[Globals::G_VEC4_X1_INDEX] >= LOWER_X && p_houghLine[Globals::G_VEC4_X1_INDEX] <= UPPER_X &&
                 p_houghLine[Globals::G_VEC4_Y1_INDEX] >= LOWER_Y && p_houghLine[Globals::G_VEC4_Y1_INDEX] <= UPPER_Y) ||
                (p_houghLine[Globals::G_VEC4_X2_INDEX] >= LOWER_X && p_houghLine[Globals::G_VEC4_X2_INDEX] <= UPPER_X &&
                 p_houghLine[Globals::G_VEC4_Y2_INDEX] >= LOWER_Y && p_houghLine[Globals::G_VEC4_Y2_INDEX] <= UPPER_Y))
            {
                return true;
            }
        }

        return false;
    }

    void LaneDetector::UpdateLineTypes()
    {
        Globals::LaneLineType leftLineType = Globals::LaneLineType::SOLID;
        if (m_leftLaneLines.empty())
        {
            leftLineType = Globals::LaneLineType::EMPTY;
        }

        else if (m_leftLineAverageLength < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            leftLineType = Globals::LaneLineType::DASHED;
        }

        Globals::LaneLineType middleLineType = Globals::LaneLineType::SOLID;
        if (m_middleLaneLines.empty())
        {
            middleLineType = Globals::LaneLineType::EMPTY;
        }

        else if (m_middleLineAverageLength < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            middleLineType = Globals::LaneLineType::DASHED;
        }

        Globals::LaneLineType rightLineType = Globals::LaneLineType::SOLID;
        if (m_rightLaneLines.empty())
        {
            rightLineType = Globals::LaneLineType::EMPTY;
        }

        else if (m_rightLineAverageLength < Globals::G_SOLID_LINE_LENGTH_THRESHOLD)
        {
            rightLineType = Globals::LaneLineType::DASHED;
        }

        m_laneDetectionInformation.m_leftLineTypesForDisplay.push_front(m_leftLaneLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(leftLineType)));
        m_laneDetectionInformation.m_leftLineTypesForDisplay.pop_back();

        m_laneDetectionInformation.m_middleLineTypesForDisplay.push_front(m_middleLaneLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(middleLineType)));
        m_laneDetectionInformation.m_middleLineTypesForDisplay.pop_back();

        m_laneDetectionInformation.m_rightLineTypesForDisplay.push_front(m_rightLaneLineTypeRollingAverage.CalculateRollingAverage(static_cast<uint32_t>(rightLineType)));
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

        const bool LEFT_LINES_PRESENT = !m_leftLaneLines.empty();
        const bool MIDDLE_LINES_PRESENT = !m_middleLaneLines.empty();
        const bool RIGHT_LINES_PRESNET = !m_rightLaneLines.empty();

        Globals::DrivingState currentDrivingState = Globals::DrivingState::NO_LANE_MARKINGS_DETECTED;

        if ((LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET) ||
            (LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET))
        {
            currentDrivingState = Globals::DrivingState::WITHIN_LANE;
        }

        else if ((!LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET) ||
                 (LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET) ||
                 (!LEFT_LINES_PRESENT && MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET))
        {
            currentDrivingState = Globals::DrivingState::CHANGING_LANES;
        }

        else if (LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && !RIGHT_LINES_PRESNET)
        {
            currentDrivingState = Globals::DrivingState::ONLY_LEFT_LANE_MARKING_DETECTED;
        }

        else if (!LEFT_LINES_PRESENT && !MIDDLE_LINES_PRESENT && RIGHT_LINES_PRESNET)
        {
            currentDrivingState = Globals::DrivingState::ONLY_RIGHT_LANE_MARKING_DETECTED;
        }

        const Globals::DrivingState NEXT_DRIVING_STATE = m_drivingStateRollingAverage.CalculateRollingAverage(currentDrivingState);

        // Check if the driving state is changing from or will change to CHANGING_LANES and if so reset some variables
        if (NEXT_DRIVING_STATE != m_laneDetectionInformation.m_drivingState)
        {
            // Reset turning state title is no longer in CHANGING_LANES state
            if (m_laneDetectionInformation.m_drivingState == Globals::DrivingState::CHANGING_LANES)
            {
                m_laneDetectionInformation.m_turningStateTitle = "";
            }

            // Reset CHANGING_LANES variables for upcoming changing lanes calculations
            if (NEXT_DRIVING_STATE == Globals::DrivingState::CHANGING_LANES)
            {
                m_changingLanesFrameCount = 0;
                m_changingLanesPreviousDistanceDifference = 0;
                m_changingLanesFirstFrame = true;
            }
        }

        m_laneDetectionInformation.m_drivingState = NEXT_DRIVING_STATE;
    }

    void LaneDetector::CalculateLanePosition()
    {
        m_laneDetectionInformation.m_laneOverlayCorners = {
            {0, 0},
            {0, 0},
            {0, 0},
            {0, 0}
        };

        // Skip if either no left or right lane lines detected
        if (m_leftLaneLines.empty() || m_rightLaneLines.empty())
        {
            return;
        }

        double averageDistanceFromLeft = 0;
        double leftLaneLineC = 0;
        double leftLaneLineM = 0;
        double leftLaneLineMinimumY = std::numeric_limits<double>::max();

        for (const cv::Vec4i& leftLaneLine : m_leftLaneLines)
        {
            // Determine X along the left/right edge of the mask using the Y co-ordinates of leftLaneLine and then use this X
            // value to compare with the actual X co-ordinates of leftLaneLine to determine the distance.

            const double LEFT_EDGE_X1 = (leftLaneLine[Globals::G_VEC4_Y1_INDEX] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;
            const double LEFT_EDGE_X2 = (leftLaneLine[Globals::G_VEC4_Y2_INDEX] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;

            averageDistanceFromLeft += std::fabs(leftLaneLine[Globals::G_VEC4_X1_INDEX] - LEFT_EDGE_X1);
            averageDistanceFromLeft += std::fabs(leftLaneLine[Globals::G_VEC4_X2_INDEX] - LEFT_EDGE_X2);

            // Find the minimum height of a left lane line
            leftLaneLineMinimumY = std::min({static_cast<double>(leftLaneLine[Globals::G_VEC4_Y1_INDEX]),
                                             static_cast<double>(leftLaneLine[Globals::G_VEC4_Y2_INDEX]),
                                             leftLaneLineMinimumY});

            // Find average gradient using m = dy/dx
            leftLaneLineM += (leftLaneLine[Globals::G_VEC4_Y1_INDEX] - leftLaneLine[Globals::G_VEC4_Y2_INDEX]) / static_cast<double>(leftLaneLine[Globals::G_VEC4_X1_INDEX] - leftLaneLine[Globals::G_VEC4_X2_INDEX]);
            // Find average c using c = y - mx and just x1,y1 co-ordinates
            leftLaneLineC += leftLaneLine[Globals::G_VEC4_Y1_INDEX] - ((leftLaneLine[Globals::G_VEC4_Y1_INDEX] - leftLaneLine[Globals::G_VEC4_Y2_INDEX]) / static_cast<double>(leftLaneLine[Globals::G_VEC4_X1_INDEX] - leftLaneLine[Globals::G_VEC4_X2_INDEX]) * leftLaneLine[Globals::G_VEC4_X1_INDEX]);
        }

        if (!m_leftLaneLines.empty())
        {
            averageDistanceFromLeft /= static_cast<double>(m_leftLaneLines.size() * 2);
            leftLaneLineM /= static_cast<double>(m_leftLaneLines.size());
            leftLaneLineC /= static_cast<double>(m_leftLaneLines.size());
        }

        // The same logic as above but for m_rightLaneLines
        double averageDistanceFromRight = 0;
        double rightLaneLineC = 0;
        double rightLaneLineM = 0;
        double rightLaneLineMinimumY = std::numeric_limits<double>::max();

        for (const cv::Vec4i& rightLaneLine : m_rightLaneLines)
        {
            // Determine X along the right/right edge of the mask using the Y co-ordinates of rightLaneLine and then use this X
            // value to compare with the actual X co-ordinates of rightLaneLine to determine the distance.

            const double RIGHT_EDGE_X1 = (rightLaneLine[Globals::G_VEC4_Y1_INDEX] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;
            const double RIGHT_EDGE_X2 = (rightLaneLine[Globals::G_VEC4_Y2_INDEX] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;

            averageDistanceFromRight += std::fabs(rightLaneLine[Globals::G_VEC4_X1_INDEX] - RIGHT_EDGE_X1);
            averageDistanceFromRight += std::fabs(rightLaneLine[Globals::G_VEC4_X2_INDEX] - RIGHT_EDGE_X2);

            // Find the minimum height of a right lane line
            rightLaneLineMinimumY = std::min({static_cast<double>(rightLaneLine[Globals::G_VEC4_Y1_INDEX]),
                                              static_cast<double>(rightLaneLine[Globals::G_VEC4_Y2_INDEX]),
                                              rightLaneLineMinimumY});

            // Find average gradient using m = dy/dx
            rightLaneLineM += (rightLaneLine[Globals::G_VEC4_Y1_INDEX] - rightLaneLine[Globals::G_VEC4_Y2_INDEX]) / static_cast<double>(rightLaneLine[Globals::G_VEC4_X1_INDEX] - rightLaneLine[Globals::G_VEC4_X2_INDEX]);
            // Find average c using c = y - mx and just x1,y1 co-ordinates
            rightLaneLineC += rightLaneLine[Globals::G_VEC4_Y1_INDEX] - ((rightLaneLine[Globals::G_VEC4_Y1_INDEX] - rightLaneLine[Globals::G_VEC4_Y2_INDEX]) / static_cast<double>(rightLaneLine[Globals::G_VEC4_X1_INDEX] - rightLaneLine[Globals::G_VEC4_X2_INDEX]) * rightLaneLine[Globals::G_VEC4_X1_INDEX]);
        }

        if (!m_rightLaneLines.empty())
        {
            averageDistanceFromRight /= static_cast<double>(m_rightLaneLines.size() * 2);
            rightLaneLineM /= static_cast<double>(m_rightLaneLines.size());
            rightLaneLineC /= static_cast<double>(m_rightLaneLines.size());
        }

        // Determine the position of the vehicle as a value from G_WITHIN_LANE_MINIMUM_CLAMP_DIFFERENCE_DISTANCE to
        // G_WITHIN_LANE_MAXIMUM_CLAMP_DIFFERENCE_DISTANCE and thus the turning required to return to the center of the current
        // lane.
        const double TURNING_REQUIRED = std::clamp(averageDistanceFromLeft - averageDistanceFromRight,
                                                   Globals::G_WITHIN_LANE_MINIMUM_CLAMP_DIFFERENCE_DISTANCE,
                                                   Globals::G_WITHIN_LANE_MAXIMUM_CLAMP_DIFFERENCE_DISTANCE);

        // Round the turning required to return to the center of the current lane down to the nearest
        // G_WITHIN_LANE_TURNING_REQUIRED_ROUNDING percent
        m_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage = static_cast<int32_t>(floor(TURNING_REQUIRED) - fmod(floor(TURNING_REQUIRED), Globals::G_WITHIN_LANE_TURNING_REQUIRED_ROUNDING));

        // Get the turning required to return to the center of the current lane in text
        if (m_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage > 0)
        {
            m_laneDetectionInformation.m_turningRequiredToReturnToCenterText = std::format("Turn Right {} %", m_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage);
        }

        else if (m_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage < 0)
        {
            m_laneDetectionInformation.m_turningRequiredToReturnToCenterText = std::format("Turn Left {} %", -m_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage);
        }

        else
        {
            m_laneDetectionInformation.m_turningRequiredToReturnToCenterText = "In Centre";
        }

        // Find the minimum lane line height - due to OpenCV origin being located at the top left of the frame, visually this
        // represents the maximum y value
        const double LANE_LINE_MINIMUM_Y = std::min(leftLaneLineMinimumY, rightLaneLineMinimumY);

        // Calculate the value y0 value for the intersection point of the left and right lane lines
        //
        //                   \  /
        //                    \/  (x0, y0)
        //                    /\
        //                   /  \
        //                  /    \
        //                 /      \
        //                /        \
        //               /          \
        //              ^            ^
        // y0 = m1x0 + c1            y0 = m2x0 + c2
        // x0 = (y0 - c1) / m1       x0 = (y0 - c2) / m2
        //
        //      (y0 - c1) / m1 = (y0 - c2) / m2
        //         m2y0 - m2c1 = m1y0 - m1c2
        //         m2y0 - m1y0 = m2c1 - m1c2
        //         y0(m2 - m1) = m2c1 - m1c2
        //                  y0 = (m2c1 - m1c2) / (m2 - m1)

        const int32_t INTERSECTION_Y0 = static_cast<int32_t>((rightLaneLineM * leftLaneLineC - leftLaneLineM * rightLaneLineC) / (rightLaneLineM - leftLaneLineM));

        // Only draw the lane overlay if the left and right lane lines intersect beyond the visible overlay. Otherwise they
        // would intersect within the visible section and the overlay would turn in to an hourglass.
        if (INTERSECTION_Y0 < LANE_LINE_MINIMUM_Y)
        {
            m_laneDetectionInformation.m_laneOverlayCorners[0] = cv::Point(static_cast<int32_t>((LANE_LINE_MINIMUM_Y - leftLaneLineC) / leftLaneLineM), static_cast<int32_t>(LANE_LINE_MINIMUM_Y));   // Top left
            m_laneDetectionInformation.m_laneOverlayCorners[1] = cv::Point(static_cast<int32_t>((LANE_LINE_MINIMUM_Y - rightLaneLineC) / rightLaneLineM), static_cast<int32_t>(LANE_LINE_MINIMUM_Y)); // Top right
            m_laneDetectionInformation.m_laneOverlayCorners[2] = cv::Point(static_cast<int32_t>((Globals::G_ROI_BOTTOM_HEIGHT - rightLaneLineC) / rightLaneLineM), Globals::G_ROI_BOTTOM_HEIGHT);     // Bottom right
            m_laneDetectionInformation.m_laneOverlayCorners[3] = cv::Point(static_cast<int32_t>((Globals::G_ROI_BOTTOM_HEIGHT - leftLaneLineC) / leftLaneLineM), Globals::G_ROI_BOTTOM_HEIGHT);       // Bottom left
        }
    }

    void LaneDetector::CalculateTurningDirection()
    {
        if (m_middleLaneLines.empty())
        {
            return;
        }

        double averageDistanceFromLeft = 0;
        double averageDistanceFromRight = 0;

        for (const cv::Vec4i& middleLaneLine : m_middleLaneLines)
        {
            // Determine X along the left/right edge of the mask using the Y co-ordinates of middleLaneLine and then use this X
            // value to compare with the actual X co-ordinates of middleLaneLine to determine the distance.

            const double LEFT_EDGE_X1 = (middleLaneLine[Globals::G_VEC4_Y1_INDEX] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;
            const double LEFT_EDGE_X2 = (middleLaneLine[Globals::G_VEC4_Y2_INDEX] - Globals::G_LEFT_EDGE_OF_MASK_C) / Globals::G_LEFT_EDGE_OF_MASK_M;

            averageDistanceFromLeft += std::fabs(middleLaneLine[Globals::G_VEC4_X1_INDEX] - LEFT_EDGE_X1);
            averageDistanceFromLeft += std::fabs(middleLaneLine[Globals::G_VEC4_X2_INDEX] - LEFT_EDGE_X2);

            const double RIGHT_EDGE_X1 = (middleLaneLine[Globals::G_VEC4_Y1_INDEX] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;
            const double RIGHT_EDGE_X2 = (middleLaneLine[Globals::G_VEC4_Y2_INDEX] - Globals::G_RIGHT_EDGE_OF_MASK_C) / Globals::G_RIGHT_EDGE_OF_MASK_M;

            averageDistanceFromRight += std::fabs(middleLaneLine[Globals::G_VEC4_X1_INDEX] - RIGHT_EDGE_X1);
            averageDistanceFromRight += std::fabs(middleLaneLine[Globals::G_VEC4_X2_INDEX] - RIGHT_EDGE_X2);
        }

        averageDistanceFromLeft /= static_cast<double>(m_middleLaneLines.size() * 2);
        averageDistanceFromRight /= static_cast<double>(m_middleLaneLines.size() * 2);

        const double CURRENT_DISTANCE_DIFFERENCE = averageDistanceFromLeft - averageDistanceFromRight;

        // There is no previous distance difference to compare to on the first frame
        if (m_changingLanesFirstFrame)
        {
            m_changingLanesPreviousDistanceDifference = CURRENT_DISTANCE_DIFFERENCE;
            m_changingLanesFirstFrame = false;
            return;
        }

        // Allow for enough frames to pass to accurately determine a difference
        if (m_changingLanesFrameCount == Globals::G_CHANGING_LANES_DISTANCE_DIFFERENCE_FRAME_COUNT_THRESHOLD)
        {
            const double CHANGE_IN_DISTANCE_DIFFERENCE = m_changingLanesPreviousDistanceDifference - CURRENT_DISTANCE_DIFFERENCE;

            if (CHANGE_IN_DISTANCE_DIFFERENCE < 0)
            {
                m_laneDetectionInformation.m_turningStateTitle = "(Currently Turning Left)";
            }

            else if (CHANGE_IN_DISTANCE_DIFFERENCE > 0)
            {
                m_laneDetectionInformation.m_turningStateTitle = "(Currently Turning Right)";
            }

            else
            {
                m_laneDetectionInformation.m_turningStateTitle = "(Currently Not Turning)";
            }

            m_changingLanesPreviousDistanceDifference = CURRENT_DISTANCE_DIFFERENCE;

            m_changingLanesFrameCount = 0;
        }

        m_changingLanesFrameCount++;
    }
}
