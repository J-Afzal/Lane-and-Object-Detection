#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "detectors/LaneDetector.hpp"
#include "detectors/ObjectDetector.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Performance.hpp"
#include "helpers/VideoManager.hpp"

namespace LaneAndObjectDetection
{
    VideoManager::VideoManager(const std::string& p_inputVideoFilePath,
                               const std::string& p_yoloResourcesFolderPath,
                               const Detectors& p_objectDetectorType,
                               const BackEnds& p_backEndType,
                               const BlobSizes& p_blobSize)
    {
        // If InputVideoPath is an integer, convert it to one so that OpenCV will use a camera as the input
        if (std::ranges::all_of(p_inputVideoFilePath, [](const char& p_i) { return isdigit(p_i); }))
        {
            m_inputVideo.open(std::stoi(p_inputVideoFilePath));
        }

        else
        {
            m_inputVideo.open(p_inputVideoFilePath);
        }

        // Set size to avoid errors in hard coded values
        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_VIDEO_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_VIDEO_HEIGHT);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nInput video file: " + p_inputVideoFilePath + " cannot be found\n";
            exit(1);
        }

        m_objectDetector.SetProperties(p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);

        m_recordOutput = false;
    }

    VideoManager::~VideoManager()
    {
        m_inputVideo.release();
        m_outputVideo.release();
        cv::destroyAllWindows();
    }

    std::vector<uint32_t> VideoManager::Run()
    {
        while (true)
        {
            m_performance.StartTimer();

            // Generate the next frame
            m_inputVideo >> m_frame;
            if (m_frame.empty())
            {
                break;
            }

            // Run the detectors
            m_objectDetector.RunDetector(m_frame);
            m_laneDetector.RunDetector(m_frame, m_objectDetector.GetBoundingBoxes());

            // Print all info to the frame
            m_objectDetector.PrintToFrame(m_frame);
            m_laneDetector.PrintToFrame(m_frame);
            m_performance.PrintFpsToFrame(m_frame);
            PrintRecordingStatusToFrame();

            // If recording, add frame to output file
            if (m_recordOutput)
            {
                m_outputVideo << m_frame;
            }

            cv::imshow("frame", m_frame);

            const uint32_t KEY = cv::waitKey(1);

            if (KEY == 'r')
            {
                ToggleRecording();
            }

            else if (KEY == 'q')
            {
                break;
            }

            m_performance.StopTimer();
        }

        cv::destroyAllWindows();

        return m_performance.GetFrameTimes();
    }

    void VideoManager::PrintRecordingStatusToFrame()
    {
        const double DIVISOR = 2;

        cv::rectangle(m_frame, Globals::G_RECORDING_STATUS_RECT, Globals::G_OPENCV_BLACK, cv::FILLED);

        if (m_recordOutput)
        {
            const std::string RECORDING_OUTPUT_TEXT = "Recording Output";

            // The next four lines are used to center the text horizontally and vertically
            int baseline = 0;
            const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
            baseline += Globals::G_DEFAULT_FONT_THICKNESS;
            const cv::Point TEXT_ORG = cv::Point(static_cast<int>((Globals::G_RECORDING_STATUS_RECT.x + (Globals::G_RECORDING_STATUS_RECT.width / DIVISOR)) - (TEXT_SIZE.width / DIVISOR)), Globals::G_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height);
            cv::putText(m_frame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
        }

        else
        {
            const std::string RECORDING_OUTPUT_TEXT = "Press 'r' to start recording";

            // The next four lines are used to center the text horizontally and vertically
            const int32_t HEIGHT_OFFSET = 5;
            int baseline = 0;
            const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_SMALL_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
            baseline += Globals::G_DEFAULT_FONT_THICKNESS;
            const cv::Point TEXT_ORG = cv::Point(static_cast<int>((Globals::G_RECORDING_STATUS_RECT.x + (Globals::G_RECORDING_STATUS_RECT.width / DIVISOR)) - (TEXT_SIZE.width / DIVISOR)), Globals::G_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height + HEIGHT_OFFSET);
            cv::putText(m_frame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_SMALL_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
        }
    }

    void VideoManager::ToggleRecording()
    {
        m_recordOutput = !m_recordOutput;

        if (m_recordOutput)
        {
            // Create a unique file name based on the current date and time
            m_outputVideo.open(std::format("{:%F-%T}", std::chrono::system_clock::now()) + " Frame.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), Globals::G_OUTPUT_VIDEO_FPS, cv::Size(Globals::G_VIDEO_WIDTH, Globals::G_VIDEO_HEIGHT));

            if (!m_outputVideo.isOpened())
            {
                std::cout << "\nERROR: Output video file could not be opened! Recording stopped!\n";
            }
        }
    }
}
