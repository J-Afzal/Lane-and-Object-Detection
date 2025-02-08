#include <string>

#include "helpers/VideoManager.hpp"

VideoManager::VideoManager(const std::string& InputVideoFilePath,
                           const std::string& YoloResourcesFolderPath,
                           const Detector& ObjectDetectorType,
                           const BackEnd& BackEndType,
                           const BlobSize& BlobSize)
{
    // If InputVideoPath is an integer, convert it to one so that OpenCV will use a camera as the input
    if (std::all_of(InputVideoFilePath.begin(), InputVideoFilePath.end(), [](const char& i) { return isdigit(i); }))
    {
        m_InputVideo.open(std::stoi(InputVideoFilePath, nullptr, 10));
    }

    else
    {
        m_InputVideo.open(InputVideoFilePath);
    }

    // Set size to avoid errors in hard coded values
    m_InputVideo.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    m_InputVideo.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);

    if (!m_InputVideo.isOpened())
    {
        std::cout << "\nInput video file: " + InputVideoFilePath + " cannot be found\n";
        exit(1);
    }

    m_ObjectDetector = std::make_unique<ObjectDetector>(YoloResourcesFolderPath, ObjectDetectorType, BackEndType, BlobSize);
    m_LaneDetector = std::make_unique<LaneDetector>();
    m_Performance = std::make_unique<Performance>();
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
        m_Performance->Start_Timer();

        // Generate the next frame
        m_InputVideo >> m_Frame;
        if (m_Frame.empty())
        {
            break;
        }

        // Run the detectors
        m_ObjectDetector->Run_Detector(m_Frame);
        m_LaneDetector->Run_Detector(m_Frame, m_ObjectDetector->Get_Bounding_Boxes());

        // Print all info to the frame
        m_ObjectDetector->Print_To_Frame(m_Frame);
        m_LaneDetector->Print_To_Frame(m_Frame);
        m_Performance->Print_FPS_To_Frame(m_Frame);
        Print_Recording_Status_To_Frame();

        // If recording, add frame to output file
        if (m_RecordOutput)
            m_OutputVideo << m_Frame;

        cv::imshow("frame", m_Frame);

        int Key = cv::waitKey(1);

        if (Key == 'r')
        {
            Toggle_Recording();
        }

        else if (Key == 'q')
        {
            break;
        }

        m_Performance->Stop_Timer();
    }

    cv::destroyAllWindows();

    return m_Performance->Get_Frame_Times();
}

void VideoManager::Print_Recording_Status_To_Frame()
{
    cv::rectangle(m_Frame, m_RECORDING_STATUS_RECT, cv::Scalar(0), cv::FILLED);

    if (m_RecordOutput)
    {
        std::string recordingOutputText = "Recording Output";
        // The next four lines are used to center the text horizontally and vertically
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(recordingOutputText, m_FONT_FACE, m_FONT_SCALE, m_FONT_THICKNESS, &baseline);
        baseline += m_FONT_THICKNESS;
        cv::Point textOrg = cv::Point((m_RECORDING_STATUS_RECT.x + (m_RECORDING_STATUS_RECT.width / 2.0)) - textSize.width / 2.0, m_RECORDING_STATUS_RECT.y + baseline + textSize.height);
        cv::putText(m_Frame, recordingOutputText, textOrg, m_FONT_FACE, m_FONT_SCALE, cv::Scalar::all(255), m_FONT_THICKNESS, cv::LINE_AA);
    }

    else
    {
        std::string recordingOutputText = "Press 'r' to start recording";
        // The next four lines are used to center the text horizontally and vertically
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
        // Create a unique file name based on the current date and time
        time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(localtime(&now), "%Y-%m-%d %H-%M-%S");
        const std::string CURRENT_TIME = ss.str();

        m_OutputVideo.open(CURRENT_TIME + " Frame.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), 30, cv::Size(m_VIDEO_WIDTH, m_VIDEO_HEIGHT), true);

        if (!m_OutputVideo.isOpened())
        {
            std::cout << "\nERROR: Output video file could not be opened! Recording stopped!\n";
        }
    }
}
