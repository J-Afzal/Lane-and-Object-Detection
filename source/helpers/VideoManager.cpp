#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <vector>

#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "detectors/LaneDetector.hpp"
#include "detectors/ObjectDetector.hpp"
#include "helpers/FrameBuilder.hpp"
#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"
#include "helpers/Performance.hpp"

#include "helpers/VideoManager.hpp"

namespace LaneAndObjectDetection
{
    VideoManager::VideoManager()
    {
        m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_NOT_RECORDING;
        m_videoManagerInformation.m_debugModeText = Globals::G_UI_TEXT_NOT_DEBUG_MODE;
        m_videoManagerInformation.m_debugMode = false;
        m_videoManagerInformation.m_saveOutput = false;
    }

    VideoManager::VideoManager(const int32_t& p_inputVideoCamera,
                               const std::string& p_yoloFolderPath,
                               const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                               const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                               const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_NOT_RECORDING;
        m_videoManagerInformation.m_debugModeText = Globals::G_UI_TEXT_NOT_DEBUG_MODE;
        m_videoManagerInformation.m_debugMode = false;
        m_videoManagerInformation.m_saveOutput = false;

        SetProperties(p_inputVideoCamera, p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);
    }

    VideoManager::VideoManager(const std::string& p_inputVideoFilePath,
                               const std::string& p_yoloFolderPath,
                               const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                               const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                               const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_NOT_RECORDING;
        m_videoManagerInformation.m_debugModeText = Globals::G_UI_TEXT_NOT_DEBUG_MODE;
        m_videoManagerInformation.m_debugMode = false;
        m_videoManagerInformation.m_saveOutput = false;

        SetProperties(p_inputVideoFilePath, p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);
    }

    VideoManager::VideoManager(const std::vector<std::string>& p_commandLineArguments) // NOLINT(readability-function-cognitive-complexity)
    {
        m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_NOT_RECORDING;
        m_videoManagerInformation.m_debugModeText = Globals::G_UI_TEXT_NOT_DEBUG_MODE;
        m_videoManagerInformation.m_debugMode = false;
        m_videoManagerInformation.m_saveOutput = false;

        std::string parsedInputVideoFilePath;
        std::string parsedYoloFolderPath;
        Globals::ObjectDetectorTypes parsedObjectDetectorTypes = Globals::ObjectDetectorTypes::NONE;
        Globals::ObjectDetectorBackEnds parsedObjectDetectorBackEnds = Globals::ObjectDetectorBackEnds::NONE;
        Globals::ObjectDetectorBlobSizes parsedObjectDetectorBlobSizes = Globals::ObjectDetectorBlobSizes::NONE;

        uint32_t index = 0;

        for (const std::string& argument : p_commandLineArguments)
        {
            if (argument == "-h" || argument == "--help")
            {
                std::cout << Globals::G_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            try
            {
                if (argument == "-i" || argument == "--input")
                {
                    parsedInputVideoFilePath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-y" || argument == "--yolo-folder-path")
                {
                    parsedYoloFolderPath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-o" || argument == "--object-detector-type")
                {
                    if (p_commandLineArguments.at(index + 1) == "none")
                    {
                        parsedObjectDetectorTypes = Globals::ObjectDetectorTypes::NONE;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "standard")
                    {
                        parsedObjectDetectorTypes = Globals::ObjectDetectorTypes::STANDARD;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "tiny")
                    {
                        parsedObjectDetectorTypes = Globals::ObjectDetectorTypes::TINY;
                    }

                    else
                    {
                        std::cout << Globals::G_CLI_HELP_MESSAGE;
                        std::exit(1);
                    }
                }

                if (argument == "-b" || argument == "--object-detector-backend")
                {
                    if (p_commandLineArguments.at(index + 1) == "cpu")
                    {
                        parsedObjectDetectorBackEnds = Globals::ObjectDetectorBackEnds::CPU;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "gpu")
                    {
                        parsedObjectDetectorBackEnds = Globals::ObjectDetectorBackEnds::GPU;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "cuda")
                    {
                        parsedObjectDetectorBackEnds = Globals::ObjectDetectorBackEnds::CUDA;
                    }

                    else
                    {
                        std::cout << Globals::G_CLI_HELP_MESSAGE;
                        std::exit(1);
                    }
                }

                if (argument == "-s" || argument == "--object-detector-blob-size")
                {
                    parsedObjectDetectorBlobSizes = static_cast<Globals::ObjectDetectorBlobSizes>(std::stoi(p_commandLineArguments.at(index + 1)));
                }
            }

            catch (...)
            {
                std::cout << Globals::G_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            index++;
        }

        // Check that the required arguments have been provided
        if (parsedInputVideoFilePath.empty() ||
            parsedYoloFolderPath.empty() ||
            (parsedObjectDetectorTypes != Globals::ObjectDetectorTypes::NONE && (parsedObjectDetectorBackEnds == Globals::ObjectDetectorBackEnds::NONE || parsedObjectDetectorBlobSizes == Globals::ObjectDetectorBlobSizes::NONE)))
        {
            std::cout << Globals::G_CLI_HELP_MESSAGE;
            std::exit(1);
        }

        SetProperties(parsedInputVideoFilePath, parsedYoloFolderPath, parsedObjectDetectorTypes, parsedObjectDetectorBackEnds, parsedObjectDetectorBlobSizes);
    }

    VideoManager::~VideoManager()
    {
        Quit();
    }

    void VideoManager::SetProperties(const int32_t& p_inputVideoCamera,
                                     const std::string& p_yoloFolderPath,
                                     const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                     const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                     const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_inputVideo.open(p_inputVideoCamera);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video camera '" + std::to_string(p_inputVideoCamera) + "' cannot be found!\n";
            std::exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_VIDEO_INPUT_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_VIDEO_INPUT_HEIGHT);

        m_objectDetector.SetProperties(p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);

        m_videoManagerInformation.m_saveOutput = false;
    }

    void VideoManager::SetProperties(const std::string& p_inputVideoFilePath,
                                     const std::string& p_yoloFolderPath,
                                     const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                     const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                     const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_inputVideo.open(p_inputVideoFilePath);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video file path '" + p_inputVideoFilePath + "' cannot be found!\n";
            std::exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_VIDEO_INPUT_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_VIDEO_INPUT_HEIGHT);

        m_objectDetector.SetProperties(p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);

        m_videoManagerInformation.m_saveOutput = false;
    }

    void VideoManager::RunLaneAndObjectDetector()
    {
        while (true)
        {
            m_performance.StartTimer();

            if (!m_inputVideo.read(m_currentFrame))
            {
                break;
            }

            m_objectDetector.RunObjectDetector(m_currentFrame);

            m_laneDetector.RunLaneDetector(m_currentFrame, m_objectDetector.GetInformation(), m_videoManagerInformation.m_debugMode);

            FrameBuilder::UpdateFrame(m_currentFrame, m_objectDetector.GetInformation(), m_laneDetector.GetInformation(), m_performance.GetInformation(), m_videoManagerInformation);

            if (m_videoManagerInformation.m_saveOutput)
            {
                m_outputVideo.write(m_currentFrame);
                m_videoManagerInformation.m_saveOutputElapsedTime = Globals::GetTimeElapsed(m_saveOutputStartTime);
            }

            cv::imshow("currentFrame", m_currentFrame);

            if (m_videoManagerInformation.m_debugMode)
            {
                cv::imshow("roiFrame", m_laneDetector.GetInformation().m_roiFrame);
                cv::imshow("cannyFrame", m_laneDetector.GetInformation().m_cannyFrame);
                cv::imshow("houghLinesFrame", m_laneDetector.GetInformation().m_houghLinesFrame);
            }

            switch (cv::waitKey(1))
            {
            case Globals::G_KEY_DEBUG_MODE:
                ToggleDebugMode();
                break;

            case Globals::G_KEY_TOGGLE_SAVE_OUTPUT:
                ToggleSaveOutput();
                break;

            case Globals::G_KEY_QUIT:
                Quit();
                return;

            default:
                break;
            }

            m_performance.EndTimer();
        }
    }

    Performance VideoManager::GetPerformance()
    {
        return m_performance;
    }

    void VideoManager::ToggleDebugMode()
    {
        m_videoManagerInformation.m_debugMode = !m_videoManagerInformation.m_debugMode;
        m_videoManagerInformation.m_debugModeText = m_videoManagerInformation.m_debugMode ? Globals::G_UI_TEXT_DEBUG_MODE : Globals::G_UI_TEXT_NOT_DEBUG_MODE;

        if (!m_videoManagerInformation.m_debugMode)
        {
            cv::destroyWindow("roiFrame");
            cv::destroyWindow("cannyFrame");
            cv::destroyWindow("houghLinesFrame");
        }
    }

    void VideoManager::ToggleSaveOutput()
    {
        m_videoManagerInformation.m_saveOutput = !m_videoManagerInformation.m_saveOutput;

        if (m_videoManagerInformation.m_saveOutput)
        {
            m_saveOutputStartTime = std::chrono::high_resolution_clock::now();

            const std::string OUTPUT_FILE_NAME = std::format("{:%Y-%m-%d-%H-%M-%S}-output.mp4", std::chrono::system_clock::now());

            m_outputVideo.open(OUTPUT_FILE_NAME,
                               cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                               Globals::G_VIDEO_OUTPUT_FPS,
                               cv::Size(Globals::G_VIDEO_OUTPUT_WIDTH, Globals::G_VIDEO_OUTPUT_HEIGHT));

            if (!m_outputVideo.isOpened())
            {
                std::cout << std::format("\nERROR: Output video file '{}' could not be opened! Recording stopped!\n", OUTPUT_FILE_NAME);
                m_videoManagerInformation.m_saveOutput = false;
            }

            else
            {
                m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_RECORDING;
            }
        }

        else
        {
            m_outputVideo.release();
            m_videoManagerInformation.m_saveOutputText = Globals::G_UI_TEXT_NOT_RECORDING;
        }
    }

    void VideoManager::Quit()
    {
        m_inputVideo.release();
        m_outputVideo.release();
        cv::destroyAllWindows();
    }
}
