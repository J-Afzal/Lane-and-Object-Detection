// NOLINTBEGIN

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/hal/interface.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/imgproc.hpp>

#include "detectors/ObjectDetector.hpp"

namespace LaneAndObjectDetection
{
    ObjectDetector::ObjectDetector(const std::string& p_yoloResourcesFolderPath, const Detector& p_yoloType, const BackEnd& p_backEndType, const BlobSize& p_blobSize)
    {
        std::ifstream modelNamesFile(p_yoloResourcesFolderPath + "coco.names");
        if (!modelNamesFile.is_open())
        {
            std::cout << "\ncoco.names file cannot be found\n";
            exit(1);
        }

        for (std::string line; std::getline(modelNamesFile, line);)
        {
            m_modelNamesAndColourList.insert(std::pair<std::string, cv::Scalar>(line, cv::Scalar(255, 255, 255)));
            m_modelNames.push_back(line);
        }

        modelNamesFile.close();

        // Setup these as custom colours
        m_modelNamesAndColourList["car"] = cv::Scalar(255, 64, 64);            // blue
        m_modelNamesAndColourList["truck"] = cv::Scalar(255, 64, 255);         // purple
        m_modelNamesAndColourList["bus"] = cv::Scalar(64, 64, 255);            // red
        m_modelNamesAndColourList["traffic light"] = cv::Scalar(64, 255, 255); // yellow

        // Setup up the Detector CUDA OpenCV DNN
        m_blobSize = static_cast<uint32_t>(p_blobSize);

        if (p_yoloType == Detector::STANDARD)
        {
            m_net = cv::dnn::readNetFromDarknet(p_yoloResourcesFolderPath + "yolov4.cfg", p_yoloResourcesFolderPath + "yolov4.weights");
        }
        else if (p_yoloType == Detector::TINY)
        {
            m_net = cv::dnn::readNetFromDarknet(p_yoloResourcesFolderPath + "yolov4-tiny.cfg", p_yoloResourcesFolderPath + "yolov4-tiny.weights");
        }
        else
        {
            m_skipDetection = true;
        }

        if (p_backEndType == BackEnd::CUDA)
        {
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        }
        else
        {
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        }

        m_unconnectedOutputLayerNames = m_net.getUnconnectedOutLayersNames();
    }

    void ObjectDetector::RunDetector(const cv::Mat& p_frame)
    {
        if (m_skipDetection)
        {
            return;
        }

        m_boundingBoxes.clear();
        m_names.clear();
        m_confidences.clear();
        m_preNmsObjectBoundingBoxes.clear();
        m_preNmsObjectNames.clear();
        m_preNmsObjectConfidences.clear();
        m_trafficLightStates.clear();

        // Get output blobs from the frame
        cv::dnn::blobFromImage(p_frame, m_blobFromImage, 1 / 255.0, cv::Size(m_blobSize, m_blobSize), cv::Scalar(0), true, false, CV_32F);
        m_net.setInput(m_blobFromImage);
        m_net.forward(m_outputBlobs, m_unconnectedOutputLayerNames);

        // Go through all output blobs and only allow those with confidence above threshold
        for (auto& outputBlob : m_outputBlobs)
        {
            for (uint32_t i = 0; i < outputBlob.rows; i++)
            {
                // rows represent number/ID of the detected objects (proposed region)
                // so loop over number/ID of detected objects.

                // for each row, the score is from element 5 up
                // to number of classes index (5 - N columns)
                // [x, y, w, h, confidence for class 1, confidence for class 2, ...]
                // minMacLoc gives the max value and its location, i.e. its classID
                cv::minMaxLoc(outputBlob.row(i).colRange(5, outputBlob.cols), nullptr, &m_confidence, nullptr, &m_classId);

                if (m_confidence > m_YOLO_CONFIDENCE_THRESHOLD)
                {
                    // Get the four uint32_t values from output blob for bounding box
                    m_centerX = outputBlob.at<float>(i, 0) * static_cast<float>(m_VIDEO_WIDTH);
                    m_centerY = outputBlob.at<float>(i, 1) * static_cast<float>(m_VIDEO_HEIGHT);
                    m_width = outputBlob.at<float>(i, 2) * static_cast<float>(m_VIDEO_WIDTH) + static_cast<float>(m_BOUNDING_BOX_BUFFER);
                    m_height = outputBlob.at<float>(i, 3) * static_cast<float>(m_VIDEO_HEIGHT) + static_cast<float>(m_BOUNDING_BOX_BUFFER);

                    // Remove object detections on the hood of car
                    if (m_centerY < m_ROI_BOTTOM_HEIGHT)
                    {
                        m_preNmsObjectNames.push_back(m_modelNames[m_classId.x]);
                        m_preNmsObjectBoundingBoxes.emplace_back(m_centerX - (m_width / 2.0), m_centerY - (m_height / 2.0), m_width, m_height);
                        m_preNmsObjectConfidences.push_back(static_cast<float>(m_confidence));
                    }
                }
            }
        }

        // Apply non-maxima suppression to suppress overlapping bounding boxes for objects that overlap, the highest confidence object will be chosen
        cv::dnn::NMSBoxes(m_preNmsObjectBoundingBoxes, m_preNmsObjectConfidences, 0.0, m_YOLO_NMS_THRESHOLD, m_indicesAfterNms);

        // Accept only the objects that don't overlap
        for (const uint32_t& i : m_indicesAfterNms)
        {
            m_boundingBoxes.push_back(m_preNmsObjectBoundingBoxes[i]);
            m_names.push_back(m_preNmsObjectNames[i]);
            m_confidences.push_back(m_preNmsObjectConfidences[i]);

            if (m_names.back() == "traffic light")
            {
                m_srcTrafficLight.clear();
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x, m_boundingBoxes.back().y);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x + m_boundingBoxes.back().width, m_boundingBoxes.back().y);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x, m_boundingBoxes.back().y + m_boundingBoxes.back().height);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x + m_boundingBoxes.back().width, m_boundingBoxes.back().y + m_boundingBoxes.back().height);

                m_dstTrafficLight.clear();
                m_dstTrafficLight.emplace_back(0, 0);
                m_dstTrafficLight.emplace_back(100, 0);
                m_dstTrafficLight.emplace_back(0, 200);
                m_dstTrafficLight.emplace_back(100, 200);

                // To warp perspective to only contain traffic light but only on the un-edited Frame so no bounding boxes shown
                cv::warpPerspective(p_frame, m_croppedImage, cv::getPerspectiveTransform(m_srcTrafficLight, m_dstTrafficLight, 0), cv::Size(100, 200));

                // count the number of green pixels
                cv::cvtColor(m_croppedImage, m_croppedImageInHsv, cv::COLOR_BGR2HSV);
                cv::inRange(m_croppedImageInHsv, cv::Scalar(32, 32, 32), cv::Scalar(80, 255, 255), m_croppedImageInHsv);
                m_nonZeroPixelsInGreen = cv::countNonZero(m_croppedImageInHsv);

                // count the number of red pixels
                cv::cvtColor(m_croppedImage, m_croppedImageInHsv, cv::COLOR_BGR2HSV);
                cv::inRange(m_croppedImageInHsv, cv::Scalar(0, 64, 64), cv::Scalar(10, 255, 255), m_croppedImageInHsv);
                m_nonZeroPixelsInRed = cv::countNonZero(m_croppedImageInHsv);

                if ((m_nonZeroPixelsInGreen > m_nonZeroPixelsInRed) && (m_nonZeroPixelsInGreen > 1000))
                {
                    m_trafficLightStates.emplace_back(" (Green)");
                }
                else if ((m_nonZeroPixelsInRed > m_nonZeroPixelsInGreen) && (m_nonZeroPixelsInRed > 1000))
                {
                    m_trafficLightStates.emplace_back(" (Red)");
                }
                else
                {
                    m_trafficLightStates.emplace_back("");
                }
            }
            else
            {
                m_trafficLightStates.emplace_back("");
            }
        }
    }

    std::vector<cv::Rect> ObjectDetector::GetBoundingBoxes()
    {
        return m_boundingBoxes;
    }

    void ObjectDetector::PrintToFrame(cv::Mat& p_frame)
    {
        if (m_skipDetection)
        {
            return;
        }

        for (uint32_t i = 0; i < m_names.size(); i++)
        {
            // Draw rectangle around detected object with the correct colour
            cv::rectangle(p_frame, m_boundingBoxes[i], m_modelNamesAndColourList[m_names[i]], 1, cv::LINE_AA);

            // Construct the correct name of object with confidence
            const std::string NAME = m_names[i] + ": " + std::to_string(static_cast<uint32_t>(100 * m_confidences[i])) + "%" + m_trafficLightStates[i];

            uint32_t size = 0;
            // This auto adjusts the background box to be the same size as 'name' expect
            // if name is smaller than object rectangle width, where it will be the same
            // size as object rectangle width
            if (m_boundingBoxes[i].width > NAME.size() * 9)
            {
                size = m_boundingBoxes[i].width;
            }
            else
            {
                size = NAME.size() * 9;
            }

            cv::rectangle(p_frame, cv::Rect(m_boundingBoxes[i].x, m_boundingBoxes[i].y - 15, size, 15), m_modelNamesAndColourList[m_names[i]], cv::FILLED, cv::LINE_AA);
            cv::putText(p_frame, NAME, cv::Point(m_boundingBoxes[i].x, m_boundingBoxes[i].y - 2), cv::FONT_HERSHEY_DUPLEX, 0.5, cv::Scalar(0), 1, cv::LINE_AA);
        }
    }
}

// NOLINTEND
