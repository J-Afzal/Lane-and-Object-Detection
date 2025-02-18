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

    explicit VideoManager::VideoManager(const uint32_t& p_inputVideoCamera,
                                        const std::string& p_yoloResourcesFolderPath,
                                        const Detectors& p_objectDetectorType,
                                        const BackEnds& p_backEndType,
                                        const BlobSizes& p_blobSize)
    {
        SetProperties(p_inputVideoCamera, p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);
    }

    explicit VideoManager::VideoManager(const std::string& p_inputVideoFilePath,
                                        const std::string& p_yoloResourcesFolderPath,
                                        const Detectors& p_objectDetectorType,
                                        const BackEnds& p_backEndType,
                                        const BlobSizes& p_blobSize)
    {
        SetProperties(p_inputVideoFilePath, p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);
    }

    explicit VideoManager::VideoManager(const std::vector<std::string>& p_commandLineArguments)
    {
        // TODO: function to parse commands and set properties
        // TODO: Add help prompt to readme (and probably need to update docs page screenshot)
    }

    VideoManager::~VideoManager()
    {
        m_inputVideo.release();
        m_outputVideo.release();
        cv::destroyAllWindows();
    }

    void VideoManager::SetProperties(const uint32_t& p_inputVideoCamera,
                                     const std::string& p_yoloResourcesFolderPath,
                                     const Detectors& p_objectDetectorType,
                                     const BackEnds& p_backEndType,
                                     const BlobSizes& p_blobSize)
    {
        m_inputVideo.open(p_inputVideoCamera);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video camera '" + std::to_string(p_inputVideoCamera) + "' cannot be found!\n";
            exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_INPUT_VIDEO_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_INPUT_VIDEO_HEIGHT);

        m_objectDetector.SetProperties(p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);

        m_recordOutput = false;
    }

    void VideoManager::SetProperties(const std::string& p_inputVideoFilePath,
                                     const std::string& p_yoloResourcesFolderPath,
                                     const Detectors& p_objectDetectorType,
                                     const BackEnds& p_backEndType,
                                     const BlobSizes& p_blobSize)
    {
        m_inputVideo.open(p_inputVideoFilePath);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video file path '" + p_inputVideoFilePath + "' cannot be found!\n";
            exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_INPUT_VIDEO_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_INPUT_VIDEO_HEIGHT);

        m_objectDetector.SetProperties(p_yoloResourcesFolderPath, p_objectDetectorType, p_backEndType, p_blobSize);

        m_recordOutput = false;
    }

    void VideoManager::Run()
    {
        while (true)
        {
            m_performance.StartTimer();

            if (!m_inputVideo.read(m_currentFrame))
            {
                break;
            }

            m_objectDetector.RunDetector(m_currentFrame);
            m_laneDetector.RunDetector(m_currentFrame, m_objectDetector.GetBoundingBoxes());

            UpdateCurrentFrame();

            if (m_recordOutput)
            {
                m_outputVideo.write(m_currentFrame);
            }

            switch (cv::waitKey(1))
            {
            case 'r': // TODO: const these
                ToggleRecording();
                break;

            case 'q': // TODO: const these
                cv::destroyAllWindows();
                return;

            default:
                break;
            }

            m_performance.StopTimer();
        }
    }

    std::vector<uint32_t> VideoManager::GetFrameTimes()
    {
        return m_performance.GetFrameTimes();
    }

    void VideoManager::UpdateCurrentFrame() // TODO: add info more discretely at the bottom of the frame? press D for debug mode with lane lines and driving state and the different frames at the top and allows for ROI to be adjusted?
    {
        // Need to incorporate these functions in to this functions. Probably need helper functions to get info to print to frame.
        // m_objectDetector.PrintToFrame(m_currentFrame);
        // m_laneDetector.PrintToFrame(m_currentFrame);
        // m_performance.PrintFpsToFrame(m_currentFrame);

        const double DIVISOR = 2;

        cv::rectangle(m_currentFrame, Globals::G_RECORDING_STATUS_RECT, Globals::G_OPENCV_BLACK, cv::FILLED);

        if (m_recordOutput)
        {
            const std::string RECORDING_OUTPUT_TEXT = "Recording Output";

            // The next four lines are used to center the text horizontally and vertically
            int baseline = 0;
            const cv::Size TEXT_SIZE = cv::getTextSize(RECORDING_OUTPUT_TEXT, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_DEFAULT_FONT_THICKNESS, &baseline);
            baseline += Globals::G_DEFAULT_FONT_THICKNESS;
            const cv::Point TEXT_ORG = cv::Point(static_cast<int>((Globals::G_RECORDING_STATUS_RECT.x + (Globals::G_RECORDING_STATUS_RECT.width / DIVISOR)) - (TEXT_SIZE.width / DIVISOR)), Globals::G_RECORDING_STATUS_RECT.y + baseline + TEXT_SIZE.height);
            cv::putText(m_currentFrame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_DEFAULT_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
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
            cv::putText(m_currentFrame, RECORDING_OUTPUT_TEXT, TEXT_ORG, Globals::G_DEFAULT_FONT_FACE, Globals::G_SMALL_FONT_SCALE, Globals::G_OPENCV_BLACK, Globals::G_DEFAULT_FONT_THICKNESS);
        }

        cv::imshow("frame", m_currentFrame); // TODO const these
    }

    void VideoManager::ToggleRecording() // TODO: clean up
    {
        m_recordOutput = !m_recordOutput;

        if (m_recordOutput)
        {
            // Create a unique file name based on the current date and time
            m_outputVideo.open(std::format("{:%F-%T}", std::chrono::system_clock::now()) + " Frame.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), Globals::G_OUTPUT_VIDEO_FPS, cv::Size(Globals::G_OUTPUT_VIDEO_WIDTH, Globals::G_OUTPUT_VIDEO_HEIGHT));

            if (!m_outputVideo.isOpened())
            {
                std::cout << "\nERROR: Output video file could not be opened! Recording stopped!\n";
            }
        }

        else
        {
            m_outputVideo.release();
        }
    }
}
