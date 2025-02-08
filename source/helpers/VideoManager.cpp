// NOLINTBEGIN

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "detectors/LaneDetector.hpp"
#include "detectors/ObjectDetector.hpp"
#include "helpers/Performance.hpp"
#include "helpers/VideoManager.hpp"

namespace LaneAndObjectDetection
{
    VideoManager::VideoManager(const std::string& p_inputVideoFilePath,
                               const std::string& p_yoloResourcesFolderPath,
                               const Detector& p_objectDetectorType,
                               const BackEnd& p_backEndType,
                               const BlobSize& p_blobSize)
    {
        // If InputVideoPath is an integer, convert it to one so that OpenCV will use a camera as the input
        if (std::ranges::all_of(p_inputVideoFilePath, [](const char& p_i) { return isdigit(p_i); }))
        {
            m_inputVideo.open(std::stoi(p_inputVideoFilePath, nullptr, 10));
        }

        else
        {
            m_inputVideo.open(p_inputVideoFilePath);
        }

        // Set size to avoid errors in hard coded values
        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nInput video file: " + p_inputVideoFilePath + " cannot be found\n";
            exit(1);
        }

        m_objectDetector = std::make_unique<ObjectDetector>(p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);
        m_laneDetector = std::make_unique<LaneDetector>();
        m_performance = std::make_unique<Performance>();
    }

    VideoManager::~VideoManager()
    {
        m_inputVideo.release();
        m_outputVideo.release();
        cv::destroyAllWindows();
    }

    std::optional<std::vector<uint32_t>> VideoManager::Run()
    {
        while (true)
        {
            m_performance->StartTimer();

            // Generate the next frame
            m_inputVideo >> m_frame;
            if (m_frame.empty())
            {
                break;
            }

            // Run the detectors
            m_objectDetector->RunDetector(m_frame);
            m_laneDetector->RunDetector(m_frame, m_objectDetector->GetBoundingBoxes());

            // Print all info to the frame
            m_objectDetector->PrintToFrame(m_frame);
            m_laneDetector->PrintToFrame(m_frame);
            m_performance->PrintFpsToFrame(m_frame);
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

            m_performance->StopTimer();
        }

        cv::destroyAllWindows();

        return m_performance->GetFrameTimes();
    }

    void VideoManager::PrintRecordingStatusToFrame()
    {
        cv::rectangle(m_frame, m_RECORDING_STATUS_RECT, cv::Scalar(0), cv::FILLED);

        if (m_recordOutput)
        {
            const std::string RECORDING_OUTPUT_TEXT = "Recording Output";
            // The next four lines are used to center the text horizontally and vertically
            int baseline = 0;
            const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
            baseline += m_FONT_THICKNESS;
            const cv::Point TEXT_ORG = cv::Point((m_RECORDING_STATUS_RECT.x + (m_RECORDING_STATUS_RECT.width / 2.0)) - (TEXT_SIZE.width / 2.0), m_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height);
            cv::putText(m_frame, RECORDING_OUTPUT_TEXT, TEXT_ORG, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
        }

        else
        {
            const std::string RECORDING_OUTPUT_TEXT = "Press 'r' to start recording";
            // The next four lines are used to center the text horizontally and vertically
            int baseline = 0;
            const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, m_FONT_FACE, m_FONT_SCALE - 0.2, m_FONT_THICKNESS, &baseline);
            baseline += m_FONT_THICKNESS;
            const cv::Point TEXT_ORG = cv::Point((m_RECORDING_STATUS_RECT.x + m_RECORDING_STATUS_RECT.width / 2.0) - (TEXT_SIZE.width / 2.0), m_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height + 5);
            cv::putText(m_frame, RECORDING_OUTPUT_TEXT, TEXT_ORG, m_FONT_FACE, m_FONT_SCALE - 0.2, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
        }
    }

    void VideoManager::ToggleRecording()
    {
        m_recordOutput = !m_recordOutput;

        if (m_recordOutput)
        {
            // Create a unique file name based on the current date and time
            const time_t NOW = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream ss;
            ss << std::put_time(localtime(&NOW), "%Y-%m-%d %H-%M-%S");
            const std::string CURRENT_TIME = ss.str();

            m_outputVideo.open(CURRENT_TIME + " Frame.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(m_VIDEO_WIDTH, m_VIDEO_HEIGHT), true);

            if (!m_outputVideo.isOpened())
            {
                std::cout << "\nERROR: Output video file could not be opened! Recording stopped!\n";
            }
        }
    }
}

// NOLINTEND
