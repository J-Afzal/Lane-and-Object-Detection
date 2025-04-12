#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <format>

#include <opencv2/core.hpp>
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
        AddObjectDetectorInformation(p_frame, p_objectDetectionInformation, p_videoManagerInformation.m_debugMode);

        AddLaneDetectorInformation(p_frame, p_laneDetectionInformation, p_videoManagerInformation.m_debugMode);

        AddPerformanceInformation(p_frame, p_performanceInformation, p_videoManagerInformation.m_debugMode);

        AddVideoManagerInformation(p_frame, p_videoManagerInformation);
    }

    void FrameBuilder::AddObjectDetectorInformation(cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation, const bool& p_debugMode)
    {
        if (!p_debugMode)
        {
            return;
        }

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
                        Globals::G_FONT_DEFAULT_FACE,
                        Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_FONT_SCALE,
                        Globals::G_COLOUR_BLACK,
                        Globals::G_FONT_DEFAULT_THICKNESS,
                        cv::LINE_AA);
        }
    }

    void FrameBuilder::AddLaneDetectorInformation(cv::Mat& p_frame, const LaneDetectionInformation& p_laneDetectionInformation, const bool& p_debugMode)
    {
        // Add driving state
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_DRIVING_STATE_RECT, p_laneDetectionInformation.m_drivingStateTitle, Globals::G_UI_H1_FONT_SCALE);

        // Add turning state
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_TURNING_STATE_RECT, p_laneDetectionInformation.m_turningStateTitle, Globals::G_UI_H2_FONT_SCALE);

        if (!p_debugMode)
        {
            return;
        }

        // TODO(Main): add roi frame, canny frame and hough lines frame

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
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT, p_laneDetectionInformation.m_turningRequiredToReturnToCenterText);

        // Used for the below overlays
        cv::Mat blankFrame = cv::Mat::zeros(Globals::G_VIDEO_INPUT_HEIGHT, Globals::G_VIDEO_INPUT_WIDTH, p_frame.type());

        if (p_laneDetectionInformation.m_drivingState == Globals::DrivingState::WITHIN_LANE)
        {
            // Add the box overlay signifying vehicle location within the lane
            cv::rectangle(blankFrame,
                          cv::Rect(Globals::G_LANE_INFORMATION_VEHICLE_POSITION_X_MIDDLE_LOCATION - p_laneDetectionInformation.m_turningRequiredToReturnToCenterPercentage,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_Y_LOCATION,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_WIDTH,
                                   Globals::G_LANE_INFORMATION_VEHICLE_POSITION_HEIGHT),
                          Globals::G_COLOUR_YELLOW,
                          cv::FILLED,
                          cv::LINE_AA);
        }

        // Draw the green lane overlay to signify the area of the road which is considered the 'current lane'
        cv::fillConvexPoly(blankFrame, p_laneDetectionInformation.m_laneOverlayCorners, Globals::G_LANE_OVERLAY_COLOUR, cv::LINE_AA);

        // Using add to give a transparent overlay
        cv::add(p_frame, blankFrame, p_frame);
    }

    void FrameBuilder::AddPerformanceInformation(cv::Mat& p_frame, const PerformanceInformation& p_performanceInformation, const bool& p_debugMode)
    {
        if (!p_debugMode)
        {
            return;
        }

        // Round frame per second values to two decimal places
        const std::string CURRENT_FPS = std::format("{} Current FPS ({} Average FPS)",
                                                    std::round(p_performanceInformation.m_currentFramesPerSecond * 100.0) / 100.0,
                                                    std::round(p_performanceInformation.m_averageFramesPerSecond));

        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_FPS_RECT, CURRENT_FPS, Globals::G_UI_H2_FONT_SCALE);
    }

    void FrameBuilder::AddVideoManagerInformation(cv::Mat& p_frame, const VideoManagerInformation& p_videoManagerInformation)
    {
        // UTC timestamp E.g. Thursday 01 January 1970 10:11:03
        const std::string TIMESTAMP = std::format("{:%A %d %B %Y} ", std::chrono::system_clock::now()) + std::format("{:%H:%M:%S}", std::chrono::system_clock::now()).substr(0, 8);
        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_TIMESTAMP_RECT, TIMESTAMP, Globals::G_UI_H2_FONT_SCALE);

        AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_DEBUG_MODE_STATUS_RECT, p_videoManagerInformation.m_debugModeText, Globals::G_UI_H2_FONT_SCALE);

        if (p_videoManagerInformation.m_saveOutput)
        {
            AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_RECORDING_STATUS_RECT, p_videoManagerInformation.m_saveOutputText, Globals::G_UI_H2_FONT_SCALE);
            AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_RECORDING_ELAPSED_TIME_RECT, p_videoManagerInformation.m_saveOutputElapsedTime, Globals::G_UI_H2_FONT_SCALE);

            // Add flashing recording dot and time spent recording
            const uint32_t SECONDS_SINCE_EPOCH = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            if (static_cast<bool>(SECONDS_SINCE_EPOCH % 2))
            {
                cv::circle(p_frame, Globals::G_UI_RECORDING_DOT_POINT, Globals::G_UI_RECORDING_DOT_RADIUS, Globals::G_COLOUR_RED, cv::FILLED, cv::LINE_AA);
            }
        }

        else
        {
            AddBackgroundRectAndCentredText(p_frame, Globals::G_UI_NOT_RECORDING_STATUS_RECT, p_videoManagerInformation.m_saveOutputText, Globals::G_UI_H2_FONT_SCALE);
        }
    }

    void FrameBuilder::AddBackgroundRectAndCentredText(cv::Mat& p_frame, const cv::Rect& p_backgroundRect, const std::string& p_text, const double& p_fontScale)
    {
        cv::rectangle(p_frame, p_backgroundRect, Globals::G_COLOUR_BLACK, cv::FILLED, cv::LINE_AA);

        double currentFontScale = p_fontScale;

        cv::Size textSize = cv::getTextSize(p_text, Globals::G_FONT_DEFAULT_FACE, currentFontScale, Globals::G_FONT_DEFAULT_THICKNESS, nullptr);

        while ((((textSize.width + Globals::G_FONT_DEFAULT_HORIZONTAL_PADDING) > p_backgroundRect.width) ||
                ((textSize.height + Globals::G_FONT_DEFAULT_VERTICAL_PADDING) > p_backgroundRect.height)) &&
               (currentFontScale > Globals::G_FONT_DEFAULT_DECREMENT))
        {
            currentFontScale -= Globals::G_FONT_DEFAULT_DECREMENT;
            textSize = cv::getTextSize(p_text, Globals::G_FONT_DEFAULT_FACE, currentFontScale, Globals::G_FONT_DEFAULT_THICKNESS, nullptr);
        }

        const int32_t X_PAD = std::ceil((p_backgroundRect.width - textSize.width) / Globals::G_DIVIDE_BY_TWO);
        const int32_t Y_PAD = std::ceil((p_backgroundRect.height - textSize.height) / Globals::G_DIVIDE_BY_TWO);

        cv::putText(p_frame,
                    p_text,
                    {p_backgroundRect.x + X_PAD, p_backgroundRect.y + textSize.height + Y_PAD},
                    Globals::G_FONT_DEFAULT_FACE,
                    currentFontScale,
                    Globals::G_COLOUR_WHITE,
                    Globals::G_FONT_DEFAULT_THICKNESS,
                    cv::LINE_AA);
    }
}
