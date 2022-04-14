/**
 * @file ObjectDetector.cpp
 * @author Junaid Afzal
 * @brief Implementation of ObjectDetector.hpp
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "detectors/ObjectDetector.hpp"
#include "helpers/Exceptions.hpp"

void ObjectDetector::Setup(const Enumerations::Detector &YoloType, const Enumerations::BackEnd &BackEndType, const Enumerations::BlobSize &BlobSize)
{
    std::ifstream ModelNamesFile("../../resources/yolo/coco.names");
    if (!ModelNamesFile.is_open())
        throw Exceptions::CocoNamesFileNotFound();

    for (std::string line; std::getline(ModelNamesFile, line);)
    {
        m_ModelNamesAndColourList.insert(std::pair<std::string, cv::Scalar>(line, cv::Scalar(255, 255, 255)));
        m_ModelNames.push_back(line);
    }

    ModelNamesFile.close();

    // Setup these as custom colours
    m_ModelNamesAndColourList["car"] = cv::Scalar(255, 64, 64);            // blue
    m_ModelNamesAndColourList["truck"] = cv::Scalar(255, 64, 255);         // purple
    m_ModelNamesAndColourList["bus"] = cv::Scalar(64, 64, 255);            // red
    m_ModelNamesAndColourList["traffic light"] = cv::Scalar(64, 255, 255); // yellow

    // Setup up the Detector CUDA OpenCV DNN
    m_BlobSize = (int)BlobSize;

    if (YoloType == Enumerations::Detector::STANDARD)
        m_Net = cv::dnn::readNetFromDarknet("../../resources/yolo/yolov4.cfg", "../../resources/yolo/yolov4.weights");
    else if (YoloType == Enumerations::Detector::TINY)
        m_Net = cv::dnn::readNetFromDarknet("../../resources/yolo/yolov4-tiny.cfg", "../../resources/yolo/yolov4-tiny.weights");
    else
        m_Skip_Detection = true;

    if (BackEndType == Enumerations::BackEnd::CUDA)
    {
        m_Net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        m_Net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        m_Net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        m_Net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }

    m_UnconnectedOutputLayerNames = m_Net.getUnconnectedOutLayersNames();
}

void ObjectDetector::Run_Detector(const cv::Mat &Frame)
{
    if (m_Skip_Detection)
        return;

    m_BoundingBoxes.clear();
    m_Names.clear();
    m_Confidences.clear();
    m_PreNMSObjectBoundingBoxes.clear();
    m_PreNMSObjectNames.clear();
    m_PreNMSObjectConfidences.clear();
    m_TrafficLightStates.clear();

    cv::dnn::blobFromImage(Frame, m_BlobFromImage, 1 / 255.0, cv::Size(m_BlobSize, m_BlobSize), cv::Scalar(0), true, false, CV_32F);
    m_Net.setInput(m_BlobFromImage);
    m_Net.forward(m_OutputBlobs, m_UnconnectedOutputLayerNames);

    // Go through all output blobs and only allow those with confidence above threshold
    for (auto &OutputBlob : m_OutputBlobs)
        for (int i = 0; i < OutputBlob.rows; i++)
        {
            // rows represent number/ID of the detected objects (proposed region)
            // so loop over number/ID of detected objects.

            // for each row, the score is from element 5 up
            // to number of classes index (5 - N columns)
            // [x, y, w, h, confidence for class 1, confidence for class 2, ...]
            // minMacLoc gives the max value and its location, i.e. its classID
            cv::minMaxLoc(OutputBlob.row(i).colRange(5, OutputBlob.cols), nullptr, &m_Confidence, nullptr, &m_ClassID);

            if (m_Confidence > m_YOLO_CONFIDENCE_THRESHOLD)
            {
                // Get the four int values from output blob for bounding box
                m_CenterX = OutputBlob.at<float>(i, 0) * (float)m_VIDEO_WIDTH;
                m_CenterY = OutputBlob.at<float>(i, 1) * (float)m_VIDEO_HEIGHT;
                m_Width = OutputBlob.at<float>(i, 2) * (float)m_VIDEO_WIDTH + (float)m_BOUNDING_BOX_BUFFER;
                m_Height = OutputBlob.at<float>(i, 3) * (float)m_VIDEO_HEIGHT + (float)m_BOUNDING_BOX_BUFFER;

                // Remove object detections on the hood of car
                if (m_CenterY < m_ROI_BOTTOM_HEIGHT)
                {
                    m_PreNMSObjectNames.push_back(m_ModelNames[m_ClassID.x]);
                    m_PreNMSObjectBoundingBoxes.emplace_back(cv::Rect(m_CenterX - m_Width / 2.0, m_CenterY - m_Height / 2.0, m_Width, m_Height));
                    m_PreNMSObjectConfidences.push_back((float)m_Confidence);
                }
            }
        }

    // Apply non-maxima suppression to suppress overlapping bounding boxes for objects that overlap, the highest confidence object will be chosen
    cv::dnn::NMSBoxes(m_PreNMSObjectBoundingBoxes, m_PreNMSObjectConfidences, 0.0, m_YOLO_NMS_THRESHOLD, m_IndicesAfterNMS);

    // Accept only the objects that don't overlap
    for (int i : m_IndicesAfterNMS)
    {
        m_BoundingBoxes.push_back(m_PreNMSObjectBoundingBoxes[i]);
        m_Names.push_back(m_PreNMSObjectNames[i]);
        m_Confidences.push_back(m_PreNMSObjectConfidences[i]);

        if (m_Names.back() == "traffic light")
        {
            m_SrcTrafficLight.clear();
            m_SrcTrafficLight.emplace_back(m_BoundingBoxes.back().x, m_BoundingBoxes.back().y);
            m_SrcTrafficLight.emplace_back(m_BoundingBoxes.back().x + m_BoundingBoxes.back().width, m_BoundingBoxes.back().y);
            m_SrcTrafficLight.emplace_back(m_BoundingBoxes.back().x, m_BoundingBoxes.back().y + m_BoundingBoxes.back().height);
            m_SrcTrafficLight.emplace_back(m_BoundingBoxes.back().x + m_BoundingBoxes.back().width, m_BoundingBoxes.back().y + m_BoundingBoxes.back().height);

            m_DstTrafficLight.clear();
            m_DstTrafficLight.emplace_back(0, 0);
            m_DstTrafficLight.emplace_back(100, 0);
            m_DstTrafficLight.emplace_back(0, 200);
            m_DstTrafficLight.emplace_back(100, 200);

            // To warp perspective to only contain traffic light but only on the un-edited Frame so no bounding boxes shown
            cv::warpPerspective(Frame, m_CroppedImage, cv::getPerspectiveTransform(m_SrcTrafficLight, m_DstTrafficLight, 0), cv::Size(100, 200));

            // count the number of green pixels
            cv::cvtColor(m_CroppedImage, m_CroppedImageInHSV, cv::COLOR_BGR2HSV);
            cv::inRange(m_CroppedImageInHSV, cv::Scalar(32, 32, 32), cv::Scalar(80, 255, 255), m_CroppedImageInHSV);
            m_NonZeroPixelsInGreen = cv::countNonZero(m_CroppedImageInHSV);

            // count the number of red pixels
            cv::cvtColor(m_CroppedImage, m_CroppedImageInHSV, cv::COLOR_BGR2HSV);
            cv::inRange(m_CroppedImageInHSV, cv::Scalar(0, 64, 64), cv::Scalar(10, 255, 255), m_CroppedImageInHSV);
            m_NonZeroPixelsInRed = cv::countNonZero(m_CroppedImageInHSV);

            if ((m_NonZeroPixelsInGreen > m_NonZeroPixelsInRed) && (m_NonZeroPixelsInGreen > 1000))
                m_TrafficLightStates.emplace_back(" (Green)");
            else if ((m_NonZeroPixelsInRed > m_NonZeroPixelsInGreen) && (m_NonZeroPixelsInRed > 1000))
                m_TrafficLightStates.emplace_back(" (Red)");
            else
                m_TrafficLightStates.emplace_back("");
        }
        else
            m_TrafficLightStates.emplace_back("");
    }
}

std::vector<cv::Rect> ObjectDetector::Get_Bounding_Boxes() { return m_BoundingBoxes; }

void ObjectDetector::Print_To_Frame(cv::Mat &Frame)
{
    if (m_Skip_Detection)
        return;

    for (int i = 0; i < m_Names.size(); i++)
    {
        // Draw rectangle around detected object with the correct colour
        cv::rectangle(Frame, m_BoundingBoxes[i], m_ModelNamesAndColourList[m_Names[i]], 1, cv::LINE_AA);

        // Construct the correct name of object with confidence
        std::string name = m_Names[i] + ": " + std::to_string((int)(100 * m_Confidences[i])) + "%" + m_TrafficLightStates[i];

        int size;
        // This auto adjusts the background box to be the same size as 'name' expect
        // if name is smaller than object rectangle width, where it will be the same
        // size as object rectangle width
        if (m_BoundingBoxes[i].width > name.size() * 9)
            size = m_BoundingBoxes[i].width;
        else
            size = name.size() * 9;

        cv::rectangle(Frame, cv::Rect(m_BoundingBoxes[i].x, m_BoundingBoxes[i].y - 15, size, 15), m_ModelNamesAndColourList[m_Names[i]], cv::FILLED, cv::LINE_AA);
        cv::putText(Frame, name, cv::Point(m_BoundingBoxes[i].x, m_BoundingBoxes[i].y - 2), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0), 1, cv::LINE_AA);
    }
}
