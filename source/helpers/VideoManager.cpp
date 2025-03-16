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
    VideoManager::VideoManager() :
        m_saveOutput(false) {}

    VideoManager::VideoManager(const int32_t& p_inputVideoCamera,
                               const std::string& p_yoloFolderPath,
                               const ObjectDetectorTypes& p_objectDetectorTypes,
                               const ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                               const ObjectDetectorBlobSizes& p_objectDetectorBlobSizes) :
        m_saveOutput(false)
    {
        SetProperties(p_inputVideoCamera, p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);
    }

    VideoManager::VideoManager(const std::string& p_inputVideoFilePath,
                               const std::string& p_yoloFolderPath,
                               const ObjectDetectorTypes& p_objectDetectorTypes,
                               const ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                               const ObjectDetectorBlobSizes& p_objectDetectorBlobSizes) :
        m_saveOutput(false)
    {
        SetProperties(p_inputVideoFilePath, p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);
    }

    VideoManager::VideoManager(const std::vector<std::string>& p_commandLineArguments) :
        m_saveOutput(false)
    {
        std::string parsedInputVideoFilePath;
        std::string parsedYoloFolderPath;
        ObjectDetectorTypes parsedObjectDetectorTypes = ObjectDetectorTypes::DEFAULT;
        ObjectDetectorBackEnds parsedObjectDetectorBackEnds = ObjectDetectorBackEnds::CPU;
        ObjectDetectorBlobSizes parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::ONE;

        uint32_t index = 0;

        for (const std::string& argument : p_commandLineArguments)
        {
            if (argument == "-h" || argument == "--help")
            {
                std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
                exit(1);
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
                        parsedObjectDetectorTypes = ObjectDetectorTypes::NONE;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "standard")
                    {
                        parsedObjectDetectorTypes = ObjectDetectorTypes::STANDARD;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "tiny")
                    {
                        parsedObjectDetectorTypes = ObjectDetectorTypes::TINY;
                    }

                    else
                    {
                        std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
                        exit(1);
                    }
                }

                if (argument == "-b" || argument == "--object-detector-backend")
                {
                    if (p_commandLineArguments.at(index + 1) == "cpu")
                    {
                        parsedObjectDetectorBackEnds = ObjectDetectorBackEnds::CPU;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "cuda")
                    {
                        parsedObjectDetectorBackEnds = ObjectDetectorBackEnds::CUDA;
                    }

                    else
                    {
                        std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
                        exit(1);
                    }
                }

                if (argument == "-s" || argument == "--object-detector-blob-size")
                {
                    if (p_commandLineArguments.at(index + 1) == "208")
                    {
                        parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::ONE;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "320")
                    {
                        parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::TWO;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "416")
                    {
                        parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::THREE;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "512")
                    {
                        parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::FOUR;
                    }

                    else if (p_commandLineArguments.at(index + 1) == "608")
                    {
                        parsedObjectDetectorBlobSizes = ObjectDetectorBlobSizes::FIVE;
                    }

                    else
                    {
                        std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
                        exit(1);
                    }
                }
            }

            catch (...)
            {
                std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
                exit(1);
            }

            index++;
        }

        // Check that the required arguments have been provided
        if (parsedInputVideoFilePath.empty() || parsedYoloFolderPath.empty() || parsedObjectDetectorTypes == ObjectDetectorTypes::DEFAULT)
        {
            std::cout << Globals::G_VIDEO_MANAGER_CLI_HELP_MESSAGE;
            exit(1);
        }

        SetProperties(parsedInputVideoFilePath, parsedYoloFolderPath, parsedObjectDetectorTypes, parsedObjectDetectorBackEnds, parsedObjectDetectorBlobSizes);
    }

    VideoManager::~VideoManager()
    {
        m_inputVideo.release();
        m_outputVideo.release();
        cv::destroyAllWindows();
    }

    void VideoManager::SetProperties(const int32_t& p_inputVideoCamera,
                                     const std::string& p_yoloFolderPath,
                                     const ObjectDetectorTypes& p_objectDetectorTypes,
                                     const ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                     const ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_inputVideo.open(p_inputVideoCamera);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video camera '" + std::to_string(p_inputVideoCamera) + "' cannot be found!\n";
            exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_VIDEO_MANAGER_INPUT_VIDEO_HEIGHT);

        m_objectDetector.SetProperties(p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);

        m_saveOutput = false;
    }

    void VideoManager::SetProperties(const std::string& p_inputVideoFilePath,
                                     const std::string& p_yoloFolderPath,
                                     const ObjectDetectorTypes& p_objectDetectorTypes,
                                     const ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                     const ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        m_inputVideo.open(p_inputVideoFilePath);

        if (!m_inputVideo.isOpened())
        {
            std::cout << "\nERROR: Input video file path '" + p_inputVideoFilePath + "' cannot be found!\n";
            exit(1);
        }

        m_inputVideo.set(cv::CAP_PROP_FRAME_WIDTH, Globals::G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH);
        m_inputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, Globals::G_VIDEO_MANAGER_INPUT_VIDEO_HEIGHT);

        m_objectDetector.SetProperties(p_yoloFolderPath, p_objectDetectorTypes, p_objectDetectorBackEnds, p_objectDetectorBlobSizes);

        m_saveOutput = false;
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

            m_objectDetector.RunObjectDetector(m_currentFrame);

            m_laneDetector.RunLaneDetector(m_currentFrame, m_objectDetector.GetInformation());

            FrameBuilder::UpdateFrame(m_currentFrame, m_objectDetector.GetInformation(), m_laneDetector.GetInformation(), m_performance.GetInformation(), {.m_saveOutput = m_saveOutput});

            if (m_saveOutput)
            {
                m_outputVideo.write(m_currentFrame);
            }

            cv::imshow("frame", m_currentFrame);

            switch (cv::waitKey(1))
            {
            case Globals::G_VIDEO_MANAGER_TOGGLE_RECORDING_KEY:
                ToggleSaveOutput();
                break;

            case Globals::G_VIDEO_MANAGER_QUIT_KEY:
                cv::destroyAllWindows();
                return;

            default:
                break;
            }

            m_performance.EndTimer();
        }
    }

    PerformanceInformation VideoManager::GetPerformanceInformation()
    {
        return m_performance.GetInformation();
    }

    void VideoManager::ToggleSaveOutput()
    {
        m_saveOutput = !m_saveOutput;

        if (m_saveOutput)
        {
            const std::string OUTPUT_FILE_NAME = std::format("{:%Y-%m-%d-%H-%M-%S}-output.mp4", std::chrono::system_clock::now());

            m_outputVideo.open(OUTPUT_FILE_NAME,
                               cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                               Globals::G_VIDEO_MANAGER_OUTPUT_VIDEO_FPS,
                               cv::Size(Globals::G_VIDEO_MANAGER_OUTPUT_VIDEO_WIDTH, Globals::G_VIDEO_MANAGER_OUTPUT_VIDEO_HEIGHT));

            if (!m_outputVideo.isOpened())
            {
                std::cout << std::format("\nERROR: Output video file '{}' could not be opened! Recording stopped!\n", OUTPUT_FILE_NAME);
                m_saveOutput = false;
            }
        }

        else
        {
            m_outputVideo.release();
        }
    }
}
