#include <algorithm>
#include <cmath>
#include <cstdint>
#include <format>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "helpers/FrameBuilder.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

namespace LaneAndObjectDetection
{

    void FrameBuilder::UpdateFrame(cv::Mat& p_frame,
                                   const ObjectDetectionInformation& p_objectDetectionInformation,
                                   const LaneDetectionInformation& p_laneDetectionInformation,
                                   const PerformanceInformation& p_performanceInformation,
                                   const VideoManagerInformation& p_videoManagerInformation)
    {
        AddObjectDetectorInformation(p_frame, p_objectDetectionInformation);

        AddLaneDetectorInformation(p_frame, p_laneDetectionInformation);

        AddPerformanceInformation(p_frame, p_performanceInformation);

        AddVideoManagerInformation(p_frame, p_videoManagerInformation);
    }

    void FrameBuilder::AddObjectDetectorInformation(cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        for (const ObjectDetectionInformation::DetectedObjectInformation& objectInformation : p_objectDetectionInformation.m_objectInformation)
        {
            // Draw the bounding box
            cv::rectangle(p_frame,
                          objectInformation.m_boundingBox,
                          objectInformation.m_boundingBoxColour,
                          Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_BORDER_THICKNESS,
                          cv::LINE_AA);

            // Draw the bounding box header
            cv::rectangle(p_frame,
                          cv::Rect(objectInformation.m_boundingBox.x,
                                   objectInformation.m_boundingBox.y - Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_HEADER_HEIGHT,
                                   std::max(objectInformation.m_boundingBox.width, static_cast<int32_t>(objectInformation.m_objectName.size() * Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_CHARACTER_WIDTH)),
                                   Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_HEADER_HEIGHT),
                          objectInformation.m_boundingBoxColour,
                          cv::FILLED,
                          cv::LINE_AA);

            // Draw the bounding box header text
            cv::putText(p_frame,
                        objectInformation.m_objectName,
                        cv::Point(objectInformation.m_boundingBox.x, objectInformation.m_boundingBox.y - Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_TEXT_HEIGHT_OFFSET),
                        Globals::G_DEFAULT_FONT_FACE,
                        Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_FONT_SCALE,
                        Globals::G_COLOUR_BLACK,
                        Globals::G_DEFAULT_FONT_THICKNESS,
                        cv::LINE_AA);
        }
    }

    void FrameBuilder::AddLaneDetectorInformation(cv::Mat& p_frame, const LaneDetectionInformation& p_laneDetectionInformation)
    {
        // Add driving state
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_DRIVING_STATE_RECT, p_laneDetectionInformation.m_drivingStateTitle);

        // Add turning state
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_TURNING_STATE_RECT, p_laneDetectionInformation.m_currentTurningState);

        // Add lane information title
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_LANE_INFORMATION_TITLE_RECT, p_laneDetectionInformation.m_laneInformationTitle);

        // Add lane information lane lines states
        cv::rectangle(p_frame, Globals::G_UI_LANE_INFORMATION_RECT, Globals::G_COLOUR_BLACK, cv::FILLED, cv::LINE_AA);

        // Left line states
        for (int32_t i = 0; i < p_laneDetectionInformation.m_leftLineTypesForDisplay.size(); i++)
        {
            cv::rectangle(p_frame,
                          cv::Rect(Globals::G_LANE_INFORMATION_LEFT_LANE_STATE_X_LOCATION,
                                   Globals::G_LANE_INFORMATION_LANE_STATE_Y_START_LOCATION + (i * Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * 2),
                                   Globals::G_LANE_INFORMATION_LANE_STATE_WIDTH,
                                   static_cast<int>(Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * p_laneDetectionInformation.m_leftLineTypesForDisplay[i])),
                          Globals::G_COLOUR_WHITE,
                          cv::FILLED,
                          cv::LINE_AA);
        }

        // Middle line states
        for (uint32_t i = 0; i < p_laneDetectionInformation.m_middleLineTypesForDisplay.size(); i++)
        {
            cv::rectangle(p_frame,
                          cv::Rect(Globals::G_LANE_INFORMATION_MIDDLE_LANE_STATE_X_LOCATION,
                                   Globals::G_LANE_INFORMATION_LANE_STATE_Y_START_LOCATION + static_cast<int>(i * Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * 2),
                                   Globals::G_LANE_INFORMATION_LANE_STATE_WIDTH,
                                   static_cast<int>(Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * p_laneDetectionInformation.m_middleLineTypesForDisplay[i])),
                          Globals::G_COLOUR_WHITE,
                          cv::FILLED,
                          cv::LINE_AA);
        }

        // Right line states
        for (int32_t i = 0; i < p_laneDetectionInformation.m_rightLineTypesForDisplay.size(); i++)
        {
            cv::rectangle(p_frame,
                          cv::Rect(Globals::G_LANE_INFORMATION_RIGHT_LANE_STATE_X_LOCATION,
                                   Globals::G_LANE_INFORMATION_LANE_STATE_Y_START_LOCATION + (i * Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * 2),
                                   Globals::G_LANE_INFORMATION_LANE_STATE_WIDTH,
                                   static_cast<int>(Globals::G_LANE_INFORMATION_LANE_STATE_HEIGHT * p_laneDetectionInformation.m_rightLineTypesForDisplay[i])),
                          Globals::G_COLOUR_WHITE,
                          cv::FILLED,
                          cv::LINE_AA);
        }

        // Add turning required to return to center of the lane
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT, p_laneDetectionInformation.m_turningRequiredToReturnToCenter);

        // Used for the below overlays
        cv::Mat blankFrame = cv::Mat::zeros(Globals::G_INPUT_VIDEO_HEIGHT, Globals::G_INPUT_VIDEO_WIDTH, p_frame.type());

        if (p_laneDetectionInformation.m_drivingState == 0)
        {
            // Add the box overlay signifying vehicle location within the lane
            cv::rectangle(blankFrame,
                          cv::Rect(Globals::G_LANE_INFORMATION_VEHICLE_POSITION_X_MIDDLE_LOCATION - p_laneDetectionInformation.m_turningRequired,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_Y_LOCATION,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_WIDTH,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_HEIGHT),
                          Globals::G_COLOUR_YELLOW,
                          cv::FILLED,
                          cv::LINE_AA);
        }

        // Draw the green lane overlay to signify the area of the road which is considered the 'current lane'
        cv::fillConvexPoly(blankFrame,
                           p_laneDetectionInformation.m_lanePoints,
                           Globals::G_LANE_INFORMATION_LANE_OVERLAY_COLOUR,
                           cv::LINE_AA);

        // Using add to give a transparent overlay
        cv::add(p_frame, blankFrame, p_frame);
    }

    void FrameBuilder::AddPerformanceInformation(cv::Mat& p_frame, const PerformanceInformation& p_performanceInformation)
    {
        // Round frame per second values to two decimal places
        const double DIVISOR = 100;
        const std::string AVERAGE_FPS = std::format("Average FPS: {}", std::round(p_performanceInformation.m_averageFramesPerSecond * DIVISOR) / DIVISOR);
        const std::string CURRENT_FPS = std::format("Current FPS: {}", std::round(p_performanceInformation.m_currentFramesPerSecond * DIVISOR) / DIVISOR);

        AddBackgroundRectAndCentredText(p_frame, Globals::G_PERFORMANCE_AVERAGE_FPS_BACKGROUND_RECT, AVERAGE_FPS);
        AddBackgroundRectAndCentredText(p_frame, Globals::G_PERFORMANCE_CURRENT_FPS_BACKGROUND_RECT, CURRENT_FPS);
    }

    void FrameBuilder::AddVideoManagerInformation(cv::Mat& p_frame, const VideoManagerInformation& p_videoManagerInformation)
    {
        AddBackgroundRectAndCentredText(p_frame,
                                        Globals::G_RECORDING_STATUS_RECT,
                                        p_videoManagerInformation.m_saveOutput ? Globals::G_UI_TEXT_RECORDING : Globals::G_UI_TEXT_NOT_RECORDING);
    }

    void FrameBuilder::AddBackgroundRectAndCentredText(cv::Mat& p_frame, const cv::Rect& p_backgroundRect, const std::string& p_text, const double& p_fontScale)
    {
        if (p_text.empty())
        {
            return;
        }

        cv::rectangle(p_frame, p_backgroundRect, Globals::G_COLOUR_BLACK, cv::FILLED, cv::LINE_AA);

        AddCentredText(p_frame, p_backgroundRect, p_text, p_fontScale);
    }

    void FrameBuilder::AddCentredText(cv::Mat& p_frame, const cv::Rect& p_backgroundRect, const std::string& p_text, const double& p_fontScale)
    {
        const double DIVISOR = 2;
        const double FONT_DECREMENT = 0.1;
        const int32_t HORIZONTAL_PADDING = 10;
        const int32_t VERTICAL_PADDING = 15;

        double currentFontScale = p_fontScale;

        cv::Size textSize = cv::getTextSize(p_text, Globals::G_DEFAULT_FONT_FACE, currentFontScale, Globals::G_DEFAULT_FONT_THICKNESS, nullptr);

        while ((((textSize.width + HORIZONTAL_PADDING) > p_backgroundRect.width) || ((textSize.height + VERTICAL_PADDING) > p_backgroundRect.height)) && (currentFontScale > FONT_DECREMENT))
        {
            currentFontScale -= FONT_DECREMENT;
            textSize = cv::getTextSize(p_text, Globals::G_DEFAULT_FONT_FACE, currentFontScale, Globals::G_DEFAULT_FONT_THICKNESS, nullptr);
        }

        const int32_t X_PAD = std::ceil((p_backgroundRect.width - textSize.width) / DIVISOR);
        const int32_t Y_PAD = std::ceil((p_backgroundRect.height - textSize.height) / DIVISOR);

        cv::putText(p_frame,
                    p_text,
                    {p_backgroundRect.x + X_PAD, p_backgroundRect.y + textSize.height + Y_PAD},
                    Globals::G_DEFAULT_FONT_FACE,
                    currentFontScale,
                    Globals::G_COLOUR_WHITE,
                    Globals::G_DEFAULT_FONT_THICKNESS,
                    cv::LINE_AA);
    }
}
