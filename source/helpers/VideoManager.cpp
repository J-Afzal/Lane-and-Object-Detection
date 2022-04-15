/**
 * @file VideoManager.cpp
 * @author Junaid Afzal
 * @brief Implementation of VideoManager.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "helpers/VideoManager.hpp"
#include "helpers/Exceptions.hpp"

VideoManager::VideoManager(const std::string &InputVideoPath,
                           const Enumerations::Detector &ObjectDetectorType,
                           const Enumerations::BackEnd &BackEndType,
                           const Enumerations::BlobSize &BlobSize,
                           const bool &RecordFrameTimes)
{
    if (String_Is_Integer(InputVideoPath))
        m_InputVideo.open(std::stoi(InputVideoPath, nullptr, 10));
    else
        m_InputVideo.open(InputVideoPath);
    m_InputVideo.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    m_InputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    if (!m_InputVideo.isOpened())
        throw Exceptions::InputVideoFileNotFound();

    m_Obj.Setup(ObjectDetectorType, BackEndType, BlobSize);

    m_RecordFrameTimes = RecordFrameTimes;
    m_Perf.Setup(m_RecordFrameTimes);
}

VideoManager::~VideoManager()
{
    m_InputVideo.release();
    m_OutputVideo.release();
    cv::destroyAllWindows();
}

std::optional<std::vector<int>> VideoManager::Run()
{
    while (true)
    {
        m_Perf.Start_Timer();

        try { Generate_Next_Frame(); }
        catch (Exceptions::OutOfFrames &e) { break; }

        m_Obj.Run_Detector(m_UneditedFrame);

        m_Lane.Run_Detector(m_UneditedFrame, m_Obj.Get_Bounding_Boxes());

        m_Obj.Print_To_Frame(m_Frame);

        m_Lane.Print_To_Frame(m_Frame);

        m_Perf.Print_FPS_To_Frame(m_Frame);

        Print_Recording_Status_To_Frame();

        Process_Current_Frame();

        cv::imshow("frame", m_Frame);

        int Key = cv::waitKey(1);
        if (Key == 'r')
            Toggle_Recording();
        else if (Key == 'q')
            break;

        m_Perf.Stop_Timer();
    }

    cv::destroyAllWindows();
    if (m_RecordFrameTimes)
        return m_Perf.Get_Frame_Times();
    else
        return {};
}

void VideoManager::Generate_Next_Frame()
{
    m_InputVideo >> m_Frame;
    m_UneditedFrame = m_Frame.clone();

    if (m_Frame.empty())
        throw Exceptions::OutOfFrames();
}

void VideoManager::Process_Current_Frame()
{
    if (m_RecordOutput)
        m_OutputVideo << m_Frame;
}

void VideoManager::Print_Recording_Status_To_Frame()
{
    cv::rectangle(m_Frame, m_RECORDING_STATUS_RECT, cv::Scalar(0), cv::FILLED);

    if (m_RecordOutput)
    {
        std::string recordingOutputText = "Recording Output";
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(recordingOutputText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        cv::Point textOrg = cv::Point((m_RECORDING_STATUS_RECT.x + (m_RECORDING_STATUS_RECT.width / 2.0)) - textSize.width / 2.0, m_RECORDING_STATUS_RECT.y + baseline + textSize.height);
        cv::putText(m_Frame, recordingOutputText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
    }

    else
    {
        std::string recordingOutputText = "Press 'r' to start recording";
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(recordingOutputText, m_FONT_FACE, m_FONT_SCALE - 0.2, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        cv::Point textOrg = cv::Point((m_RECORDING_STATUS_RECT.x + m_RECORDING_STATUS_RECT.width / 2.0) - textSize.width / 2.0, m_RECORDING_STATUS_RECT.y + baseline + textSize.height + 5);
        cv::putText(m_Frame, recordingOutputText, textOrg, m_FONT_FACE, m_FONT_SCALE - 0.2, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
    }
}

void VideoManager::Toggle_Recording()
{
    m_RecordOutput = !m_RecordOutput;

    if (m_RecordOutput)
    {
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(localtime(&now), "%Y-%m-%d %H-%M-%S");
        std::string currentTime = ss.str();

        m_OutputVideo.open(currentTime + " Frame.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(m_VIDEO_WIDTH, m_VIDEO_HEIGHT), true);

        if (!m_OutputVideo.isOpened())
            throw Exceptions::OutputVideoFileNotCreated();
    }
}

bool VideoManager::String_Is_Integer(const std::string &Input)
{
    for (const char &i : Input)
        if (!isdigit(i))
            return false;
    return true;
}
