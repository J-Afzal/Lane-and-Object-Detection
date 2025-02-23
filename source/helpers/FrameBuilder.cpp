#include <algorithm>

#include <opencv2/imgproc.hpp>

#include "helpers/FrameBuilder.hpp"
#include "helpers/Globals.hpp"

namespace LaneAndObjectDetection
{

    void FrameBuilder::UpdateFrame(cv::Mat& p_frame,
                                   const ObjectDetectionInformation& p_objectDetectionInformation,
                                   const LaneDetectionInformation& p_laneDetectionInformation,
                                   const PerformanceInformation& p_performanceInformation)
    {
        AddObjectDetectionInformation(p_frame, p_objectDetectionInformation);
        AddLaneDetectionInformation(p_frame, p_laneDetectionInformation);
        AddPerformanceInformation(p_frame, p_performanceInformation);
    }

    void FrameBuilder::AddObjectDetectionInformation(cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation)
    {
        for (const ObjectDetectionInformation::ObjectInformation& objectInformation : p_objectDetectionInformation.m_objectInformation)
        {
            // Draw the bounding box
            cv::rectangle(p_frame, objectInformation.m_boundingBox, objectInformation.m_boundingBoxColour);

            // Draw the bounding box header
            cv::rectangle(p_frame,
                          cv::Rect(objectInformation.m_boundingBox.x,
                                   objectInformation.m_boundingBox.y - Globals::G_OBJECT_DETECTION_BOUNDING_BOX_HEIGHT,
                                   std::max(objectInformation.m_boundingBox.width, static_cast<int32_t>(objectInformation.m_objectName.size() * Globals::G_OBJECT_DETECTION_BOUNDING_BOX_CHARACTER_WIDTH)),
                                   Globals::G_OBJECT_DETECTION_BOUNDING_BOX_HEIGHT),
                          objectInformation.m_boundingBoxColour,
                          cv::FILLED);

            // Draw the bounding box header text
            cv::putText(p_frame,
                        objectInformation.m_objectName,
                        cv::Point(objectInformation.m_boundingBox.x, objectInformation.m_boundingBox.y - Globals::G_OBJECT_DETECTION_BOUNDING_BOX_TEXT_HEIGHT_OFFSET),
                        Globals::G_DEFAULT_FONT_FACE,
                        Globals::G_OBJECT_DETECTION_BOUNDING_BOX_FONT_SCALE,
                        Globals::G_OPENCV_BLACK);
        }
    }

    void FrameBuilder::AddLaneDetectionInformation(cv::Mat& p_frame, const LaneDetectionInformation& p_laneDetectionInformation)
    {
    }

    void FrameBuilder::AddPerformanceInformation(cv::Mat& p_frame, const PerformanceInformation& p_performanceInformation)
    {
    }
}

    // void Performance::PrintFpsToFrame(cv::Mat& p_frame) const
    // {
    //     const int32_t TEXT_HORIZONTAL_START = 30;
    //     const int32_t TEXT_VERTICAL_START = 25;
    //     const int32_t TEXT_VERTICAL_OFFSET = 50;

    //     // Display the previous average FPS and previous frame FPS
    //     cv::rectangle(p_frame, Globals::G_FPS_RECT, Globals::G_OPENCV_BLACK, cv::FILLED);

    //     std::stringstream ss1;
    //     ss1 << std::fixed << std::setprecision(2) << m_averageFps;
    //     std::string fpsText = "Average FPS: " + ss1.str();
    //     // The next four lines are used to center the text horizontally and vertically
    //     int baseline = 0;
    //     cv::Size textSize = cv::getTextSize(fpsText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //     baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //     cv::Point textOrg = cv::Point(TEXT_HORIZONTAL_START, TEXT_VERTICAL_START + baseline + textSize.height);
    //     cv::putText(p_frame, fpsText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);

    //     std::stringstream ss2;
    //     ss2 << std::fixed << std::setprecision(2) << m_currentFps;
    //     fpsText = "Current FPS: " + ss2.str();
    //     // The next four lines are used to center the text horizontally and vertically
    //     baseline = 0;
    //     textSize = cv::getTextSize(fpsText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //     baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //     textOrg = cv::Point(TEXT_HORIZONTAL_START, TEXT_VERTICAL_OFFSET + TEXT_VERTICAL_START + baseline + textSize.height);
    //     cv::putText(p_frame, fpsText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);
    // }


// void VideoManager::UpdateCurrentFrame()
// {
    // TODO: add info more discretely at the bottom of the frame? press D for debug mode with lane lines and driving state and the different frames at the top and allows for ROI to be adjusted?

    // Need to incorporate these functions in to this functions. Probably need helper functions to get info to print to frame.
    // m_objectDetector.PrintToFrame(m_currentFrame);
    // m_laneDetector.PrintToFrame(m_currentFrame);
    // m_performance.PrintFpsToFrame(m_currentFrame);

    // void LaneDetector::PrintToFrame(cv::Mat& p_frame)
    // {
    //     const double DIVISOR = 2;

    //     const int32_t LEFT_X_POSITION = 1595;
    //     const int32_t MIDDLE_X_POSITION = 1695;
    //     const int32_t RIGHT_X_POSITION = 1795;
    //     const int32_t MINIMUM_Y_POSITION = 80;
    //     const int32_t Y_POSITION_INCREMENT = 50;
    //     const int32_t WIDTH = 4;
    //     const int32_t HEIGHT_UNIT = 25;

    //     const int32_t TITLE_Y_POSITION = 25;
    //     const int32_t GIVEWAY_WARNING_Y_POSITION = 225;

    //     // Center Title
    //     // The next four lines are used to center the text horizontally and vertically
    //     int baseline = 0;
    //     cv::Size textSize = cv::getTextSize(m_titleText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //     baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //     cv::Point textOrg = cv::Point(static_cast<int>((Globals::G_VIDEO_WIDTH - textSize.width) / DIVISOR), TITLE_Y_POSITION + baseline + textSize.height);
    //     cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), Globals::G_OPENCV_BLACK, cv::FILLED);
    //     cv::putText(p_frame, m_titleText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);

    //     // Giveway warning
    //     if (m_giveWayWarning)
    //     {
    //         const std::string GIVE_WAY_WARNING_TEXT = "WARNING: Giveway ahead";
    //         // The next four lines are used to center the text horizontally and vertically
    //         baseline = 0;
    //         textSize = cv::getTextSize(GIVE_WAY_WARNING_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //         baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //         textOrg = cv::Point(static_cast<int>((Globals::G_VIDEO_WIDTH - textSize.width) / DIVISOR), GIVEWAY_WARNING_Y_POSITION + baseline + textSize.height);
    //         cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), Globals::G_OPENCV_BLACK, cv::FILLED);
    //         cv::putText(p_frame, GIVE_WAY_WARNING_TEXT, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);
    //     }

    //     // Right-hand side info box and title
    //     cv::rectangle(p_frame, Globals::G_RIGHT_INFO_RECT, Globals::G_OPENCV_BLACK, cv::FILLED, cv::LINE_AA, 0);
    //     // The next four lines are used to center the text horizontally and vertically
    //     baseline = 0;
    //     textSize = cv::getTextSize(m_rightInfoTitleText, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //     baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //     textOrg = cv::Point(static_cast<int>(Globals::G_RIGHT_INFO_RECT.x + (Globals::G_RIGHT_INFO_RECT.width / DIVISOR)) - static_cast<int>(textSize.width / DIVISOR), Globals::G_RIGHT_INFO_RECT.y + baseline + textSize.height);
    //     cv::putText(p_frame, m_rightInfoTitleText, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);

    //     // Left line states
    //     for (int32_t i = 0; i < m_leftLineTypesForDisplay.size(); i++)
    //     {
    //         cv::rectangle(p_frame, cv::Rect(LEFT_X_POSITION, MINIMUM_Y_POSITION + (i * Y_POSITION_INCREMENT), WIDTH, static_cast<int>(HEIGHT_UNIT * m_leftLineTypesForDisplay[i])), Globals::G_OPENCV_WHITE, cv::FILLED, cv::LINE_AA);
    //     }
    //     // Right line states
    //     for (int32_t i = 0; i < m_rightLineTypesForDisplay.size(); i++)
    //     {
    //         cv::rectangle(p_frame, cv::Rect(RIGHT_X_POSITION, MINIMUM_Y_POSITION + (i * Y_POSITION_INCREMENT), WIDTH, static_cast<int>(HEIGHT_UNIT * m_rightLineTypesForDisplay[i])), Globals::G_OPENCV_WHITE, cv::FILLED, cv::LINE_AA);
    //     }

    //     if (m_drivingState == 0)
    //     {
    //         const int32_t BOX_X_POSITION = static_cast<int>(1695 - m_turningRequired - 75);
    //         const int32_t BOX_Y_POSITION = 205 - 100;
    //         const int32_t BOX_WIDTH = 150;
    //         const int32_t BOX_HEIGHT = 200;

    //         // Draw the yellow box that signifies the position of car with respect to the lanes detected
    //         m_blankFrame = cv::Mat::zeros(Globals::G_VIDEO_HEIGHT, Globals::G_VIDEO_WIDTH, p_frame.type());
    //         cv::rectangle(m_blankFrame, cv::Rect(BOX_X_POSITION, BOX_Y_POSITION, BOX_WIDTH, BOX_HEIGHT), Globals::G_OPENCV_BRIGHTER_YELLOW, cv::FILLED, cv::LINE_AA);
    //         cv::add(p_frame, m_blankFrame, p_frame);

    //         // Write the turning needed to the screen
    //         // The next four lines are used to center the text horizontally and vertically
    //         baseline = 0;
    //         textSize = cv::getTextSize(m_turningRequiredToReturnToCenter, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //         baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //         textOrg = cv::Point(Globals::G_RIGHT_INFO_RECT.x + static_cast<int>(Globals::G_RIGHT_INFO_RECT.width / DIVISOR) - static_cast<int>(textSize.width / DIVISOR), Globals::G_RIGHT_INFO_RECT.y + Globals::G_RIGHT_INFO_RECT.height + baseline - textSize.height);
    //         cv::putText(p_frame, m_turningRequiredToReturnToCenter, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);

    //         if (m_printLaneOverlay)
    //         {
    //             // Make blank frame a blank black frame
    //             m_blankFrame = cv::Mat::zeros(Globals::G_VIDEO_HEIGHT, Globals::G_VIDEO_WIDTH, p_frame.type());

    //             cv::fillConvexPoly(m_blankFrame, m_lanePoints, Globals::G_OPENCV_GREEN, cv::LINE_AA, 0);

    //             // Can simply add the two images as the background in m_blankFrame
    //             // is black (0,0,0) and so will not affect the frame image
    //             // while still being able to see tarmac
    //             cv::add(p_frame, m_blankFrame, p_frame);
    //         }
    //     }

    //     else if (m_drivingState == 1)
    //     {
    //         // Middle line type on RHS information box
    //         for (uint32_t i = 0; i < m_middleLineTypesForDisplay.size(); i++)
    //         {
    //             cv::rectangle(p_frame, cv::Rect(MIDDLE_X_POSITION, MINIMUM_Y_POSITION + static_cast<int>(i * Y_POSITION_INCREMENT), WIDTH, static_cast<int>(HEIGHT_UNIT * m_middleLineTypesForDisplay[i])), Globals::G_OPENCV_WHITE, cv::FILLED, cv::LINE_AA);
    //         }

    //         if (!m_currentTurningState.empty())
    //         {
    //             // Write the current turning state to screen
    //             // The next four lines are used to center the text horizontally and vertically
    //             const int32_t MINIMUM_HEIGHT = 125;
    //             baseline = 0;
    //             textSize = cv::getTextSize(m_currentTurningState, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
    //             baseline += Globals::G_DEFAULT_FONT_THICKNESS;
    //             textOrg = cv::Point(static_cast<int>((Globals::G_VIDEO_WIDTH - textSize.width) / DIVISOR), MINIMUM_HEIGHT + baseline + textSize.height);
    //             cv::rectangle(p_frame, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height - baseline), Globals::G_OPENCV_BLACK, cv::FILLED);
    //             cv::putText(p_frame, m_currentTurningState, textOrg, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_WHITE, Globals::G_DEFAULT_FONT_THICKNESS, cv::LINE_AA);
    //         }
    //     }
    // }

//     const double DIVISOR = 2;

//     cv::rectangle(m_currentFrame, Globals::G_RECORDING_STATUS_RECT, Globals::G_OPENCV_BLACK, cv::FILLED);

//     if (m_saveOutput)
//     {
//         const std::string RECORDING_OUTPUT_TEXT = "Recording Output";

//         // The next four lines are used to center the text horizontally and vertically
//         int baseline = 0;
//         const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
//         baseline += Globals::G_DEFAULT_FONT_THICKNESS;
//         const cv::Point TEXT_ORG = cv::Point(static_cast<int>((Globals::G_RECORDING_STATUS_RECT.x + (Globals::G_RECORDING_STATUS_RECT.width / DIVISOR)) - (TEXT_SIZE.width / DIVISOR)), Globals::G_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height);
//         cv::putText(m_currentFrame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
//     }

//     else
//     {
//         const std::string RECORDING_OUTPUT_TEXT = "Press 'r' to start recording";

//         // The next four lines are used to center the text horizontally and vertically
//         const int32_t HEIGHT_OFFSET = 5;
//         int baseline = 0;
//         const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_SMALL_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
//         baseline += Globals::G_DEFAULT_FONT_THICKNESS;
//         const cv::Point TEXT_ORG = cv::Point(static_cast<int>((Globals::G_RECORDING_STATUS_RECT.x + (Globals::G_RECORDING_STATUS_RECT.width / DIVISOR)) - (TEXT_SIZE.width / DIVISOR)), Globals::G_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height + HEIGHT_OFFSET);
//         cv::putText(m_currentFrame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_SMALL_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
//     }

//     cv::imshow("frame", m_currentFrame); // const these
// }
