/**
 * @file ObjectDetector.hpp
 * @author Junaid Afzal
 * @brief Performs the object detection
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"
#include "helpers/Enumerations.hpp"

class ObjectDetector
{
public:
    ObjectDetector() = default;

    ~ObjectDetector() = default;

    void Setup(const Enumerations::Detector &YoloType, const Enumerations::BackEnd &BackEndType, const Enumerations::BlobSize &BlobSize);

    void Run_Detector(const cv::Mat &Frame);

    std::vector<cv::Rect> Get_Bounding_Boxes();

    void Print_To_Frame(cv::Mat &Frame);

private:
    std::map<std::string, cv::Scalar> m_ModelNamesAndColourList;
    std::vector<cv::Mat> m_OutputBlobs;
    std::vector<cv::Rect> m_BoundingBoxes, m_PreNMSObjectBoundingBoxes;
    std::vector<std::string> m_UnconnectedOutputLayerNames, m_ModelNames, m_Names, m_PreNMSObjectNames, m_TrafficLightStates;
    std::vector<cv::Point2f> m_SrcTrafficLight, m_DstTrafficLight;
    std::vector<float> m_Confidences;
    std::vector<float> m_PreNMSObjectConfidences;
    std::vector<int> m_IndicesAfterNMS;
    cv::dnn::Net m_Net;
    cv::Point m_ClassID;
    cv::Mat m_BlobFromImage, m_CroppedImage, m_CroppedImageInHSV;
    double m_Confidence{}, m_CenterX{}, m_CenterY{}, m_Width{}, m_Height{};
    int m_NonZeroPixelsInGreen{}, m_NonZeroPixelsInRed{}, m_BlobSize{};
    bool m_Skip_Detection{};

    const double m_YOLO_CONFIDENCE_THRESHOLD = 0.4;
    const float m_YOLO_NMS_THRESHOLD = 0.4;
    const int m_BOUNDING_BOX_BUFFER = 5, m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_ROI_BOTTOM_HEIGHT = 840;
};
