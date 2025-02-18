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
#include "helpers/Globals.hpp"

namespace LaneAndObjectDetection
{
    ObjectDetector::ObjectDetector() :
        m_nonZeroPixelsInGreen(0),
        m_nonZeroPixelsInRed(0),
        m_blobSize(0),
        m_confidence(0),
        m_centerX(0),
        m_centerY(0),
        m_width(0),
        m_height(0),
        m_skipDetection(false)
    {}

    void ObjectDetector::SetProperties(
        const std::string& p_yoloResourcesFolderPath,
        const Detectors& p_yoloType,
        const BackEnds& p_backEndType,
        const BlobSizes& p_blobSize)
    {
        std::ifstream modelNamesFile(p_yoloResourcesFolderPath + "/coco.names");
        if (!modelNamesFile.is_open())
        {
            std::cout << "\nERROR: '" + p_yoloResourcesFolderPath + "/coco.names' file cannot be found!\n";
            exit(1);
        }

        for (std::string line; std::getline(modelNamesFile, line);)
        {
            m_modelNamesAndColourList.insert(std::pair<std::string, cv::Scalar>(line, Globals::G_OPENCV_WHITE));
            m_modelNames.push_back(line);
        }

        modelNamesFile.close();

        // Setup these as custom colours
        m_modelNamesAndColourList["car"] = Globals::G_OPENCV_BLUE;
        m_modelNamesAndColourList["truck"] = Globals::G_OPENCV_PURPLE;
        m_modelNamesAndColourList["bus"] = Globals::G_OPENCV_RED;
        m_modelNamesAndColourList["traffic light"] = Globals::G_OPENCV_YELLOW;

        // Setup up the Detector CUDA OpenCV DNN
        m_blobSize = static_cast<uint32_t>(p_blobSize);

        if (p_yoloType == Detectors::STANDARD)
        {
            m_net = cv::dnn::readNetFromDarknet(p_yoloResourcesFolderPath + "yolov4.cfg", p_yoloResourcesFolderPath + "yolov4.weights");
        }
        else if (p_yoloType == Detectors::TINY)
        {
            m_net = cv::dnn::readNetFromDarknet(p_yoloResourcesFolderPath + "yolov4-tiny.cfg", p_yoloResourcesFolderPath + "yolov4-tiny.weights");
        }
        else
        {
            m_skipDetection = true;
        }

        if (p_backEndType == BackEnds::CUDA)
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
        const double SCALE_FACTOR = 1 / 255.0;
        cv::dnn::blobFromImage(p_frame, m_blobFromImage, SCALE_FACTOR, cv::Size(static_cast<int>(m_blobSize), static_cast<int>(m_blobSize)), Globals::G_OPENCV_BLACK, true, false, CV_32F);
        m_net.setInput(m_blobFromImage);
        m_net.forward(m_outputBlobs, m_unconnectedOutputLayerNames);

        // Go through all output blobs and only allow those with confidence above threshold
        const int32_t X_COORD_LOCATION = 0;
        const int32_t Y_COORD_LOCATION = 1;
        const int32_t WIDTH_LOCATION = 2;
        const int32_t HEIGHT_LOCATION = 3;
        const int32_t START_COLUMN = 5;

        for (auto& outputBlob : m_outputBlobs)
        {
            for (int32_t i = 0; i < outputBlob.rows; i++)
            {
                // rows represent number/ID of the detected objects (proposed region)
                // so loop over number/ID of detected objects.

                // for each row, the score is from element 5 up
                // to number of classes index (5 - N columns)
                // [x, y, w, h, confidence for class 1, confidence for class 2, ...]
                // minMacLoc gives the max value and its location, i.e. its classID
                cv::minMaxLoc(outputBlob.row(i).colRange(START_COLUMN, outputBlob.cols), nullptr, &m_confidence, nullptr, &m_classId); // Should it be five?

                if (m_confidence > Globals::G_YOLO_CONFIDENCE_THRESHOLD)
                {
                    // Get the four uint32_t values from output blob for bounding box
                    m_centerX = outputBlob.at<float>(i, X_COORD_LOCATION) * static_cast<float>(Globals::G_VIDEO_WIDTH);
                    m_centerY = outputBlob.at<float>(i, Y_COORD_LOCATION) * static_cast<float>(Globals::G_VIDEO_HEIGHT);
                    m_width = outputBlob.at<float>(i, WIDTH_LOCATION) * static_cast<float>(Globals::G_VIDEO_WIDTH) + static_cast<float>(Globals::G_BOUNDING_BOX_BUFFER);
                    m_height = outputBlob.at<float>(i, HEIGHT_LOCATION) * static_cast<float>(Globals::G_VIDEO_HEIGHT) + static_cast<float>(Globals::G_BOUNDING_BOX_BUFFER);

                    // Remove object detections on the hood of car
                    if (m_centerY < Globals::G_ROI_BOTTOM_HEIGHT)
                    {
                        const double DIVISOR = 2;
                        m_preNmsObjectNames.push_back(m_modelNames[m_classId.x]);
                        m_preNmsObjectBoundingBoxes.emplace_back(m_centerX - (m_width / DIVISOR), m_centerY - (m_height / DIVISOR), m_width, m_height);
                        m_preNmsObjectConfidences.push_back(static_cast<float>(m_confidence));
                    }
                }
            }
        }

        // Apply non-maxima suppression to suppress overlapping bounding boxes for objects that overlap, the highest confidence object will be chosen
        cv::dnn::NMSBoxes(m_preNmsObjectBoundingBoxes, m_preNmsObjectConfidences, 0.0, Globals::G_YOLO_NMS_THRESHOLD, m_indicesAfterNms);

        // Accept only the objects that don't overlap
        for (const int32_t& index : m_indicesAfterNms)
        {
            m_boundingBoxes.push_back(m_preNmsObjectBoundingBoxes[index]);
            m_names.push_back(m_preNmsObjectNames[index]);
            m_confidences.push_back(m_preNmsObjectConfidences[index]);

            if (m_names.back() == "traffic light")
            {
                m_srcTrafficLight.clear();
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x, m_boundingBoxes.back().y);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x + m_boundingBoxes.back().width, m_boundingBoxes.back().y);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x, m_boundingBoxes.back().y + m_boundingBoxes.back().height);
                m_srcTrafficLight.emplace_back(m_boundingBoxes.back().x + m_boundingBoxes.back().width, m_boundingBoxes.back().y + m_boundingBoxes.back().height);

                const uint32_t TRAFFIC_LIGHT_WIDTH = 100;
                const uint32_t TRAFFIC_LIGHT_HEIGHT = 200;

                m_dstTrafficLight.clear();
                m_dstTrafficLight.emplace_back(0, 0);
                m_dstTrafficLight.emplace_back(TRAFFIC_LIGHT_WIDTH, 0);
                m_dstTrafficLight.emplace_back(0, TRAFFIC_LIGHT_HEIGHT);
                m_dstTrafficLight.emplace_back(TRAFFIC_LIGHT_WIDTH, TRAFFIC_LIGHT_HEIGHT);

                // To warp perspective to only contain traffic light but only on the un-edited Frame so no bounding boxes shown
                cv::warpPerspective(p_frame, m_croppedImage, cv::getPerspectiveTransform(m_srcTrafficLight, m_dstTrafficLight, 0), cv::Size(TRAFFIC_LIGHT_WIDTH, TRAFFIC_LIGHT_HEIGHT));

                // count the number of green pixels
                const cv::Scalar GREEN_PIXEL_LOWER_RANGE = cv::Scalar(32, 32, 32);
                const cv::Scalar GREEN_PIXEL_UPPER_RANGE = cv::Scalar(80, 255, 255);
                cv::cvtColor(m_croppedImage, m_croppedImageInHsv, cv::COLOR_BGR2HSV);
                cv::inRange(m_croppedImageInHsv, GREEN_PIXEL_LOWER_RANGE, GREEN_PIXEL_UPPER_RANGE, m_croppedImageInHsv);
                m_nonZeroPixelsInGreen = cv::countNonZero(m_croppedImageInHsv);

                // count the number of red pixels
                const cv::Scalar RED_PIXEL_LOWER_RANGE = cv::Scalar(0, 64, 64);
                const cv::Scalar RED_PIXEL_UPPER_RANGE = cv::Scalar(10, 255, 255);
                cv::cvtColor(m_croppedImage, m_croppedImageInHsv, cv::COLOR_BGR2HSV);
                cv::inRange(m_croppedImageInHsv, RED_PIXEL_LOWER_RANGE, RED_PIXEL_UPPER_RANGE, m_croppedImageInHsv);
                m_nonZeroPixelsInRed = cv::countNonZero(m_croppedImageInHsv);

                const uint32_t PIXEL_COUNT_THRESHOLD = 1000;

                if ((m_nonZeroPixelsInGreen > m_nonZeroPixelsInRed) && (m_nonZeroPixelsInGreen > PIXEL_COUNT_THRESHOLD))
                {
                    m_trafficLightStates.emplace_back(" (Green)");
                }
                else if ((m_nonZeroPixelsInRed > m_nonZeroPixelsInGreen) && (m_nonZeroPixelsInRed > PIXEL_COUNT_THRESHOLD))
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
            cv::rectangle(p_frame, m_boundingBoxes[i], m_modelNamesAndColourList[m_names[i]]);

            // Construct the correct name of object with confidence
            const uint32_t CONVERT_TO_PERCENTAGE = 100;
            const std::string NAME = m_names[i] + ": " + std::to_string(static_cast<uint32_t>(m_confidences[i] * CONVERT_TO_PERCENTAGE)) + "%" + m_trafficLightStates[i];

            int32_t size = 0;
            // This auto adjusts the background box to be the same size as 'name' expect
            // if name is smaller than object rectangle width, where it will be the same
            // size as object rectangle width
            const int32_t CHARACTER_WIDTH = 9;
            if (m_boundingBoxes[i].width > NAME.size() * CHARACTER_WIDTH)
            {
                size = m_boundingBoxes[i].width;
            }
            else
            {
                size = static_cast<int>(NAME.size()) * CHARACTER_WIDTH;
            }

            const int32_t OBJECT_NAME_HEADER_HEIGHT = 15;

            cv::rectangle(p_frame, cv::Rect(m_boundingBoxes[i].x, m_boundingBoxes[i].y - OBJECT_NAME_HEADER_HEIGHT, size, OBJECT_NAME_HEADER_HEIGHT), m_modelNamesAndColourList[m_names[i]], cv::FILLED, cv::LINE_AA);
            cv::putText(p_frame, NAME, cv::Point(m_boundingBoxes[i].x, m_boundingBoxes[i].y - 2), Globals::G_DEFAULT_FONT_FACE, Globals::G_EXTRA_SMALL_FONT_SCALE, Globals::G_OPENCV_BLACK);
        }
    }
}
