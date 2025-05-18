#include <cfloat>
#include <cstdint>
#include <format>
#include <string>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "helpers/Globals.hpp"

#include "detectors/ObjectDetector.hpp"

namespace LaneAndObjectDetection
{
    ObjectDetector::ObjectDetector() :
        m_blobSize(0),
        m_skipObjectDetection(false)
    {}

    void ObjectDetector::SetProperties(const std::string& p_yoloFolderPath,
                                       const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                       const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                       const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes)
    {
        switch (p_objectDetectorTypes)
        {
        case Globals::ObjectDetectorTypes::NONE:
            m_skipObjectDetection = true;
            break;

        case Globals::ObjectDetectorTypes::STANDARD:
            m_net = cv::dnn::readNetFromDarknet(p_yoloFolderPath + "yolov7.cfg", p_yoloFolderPath + "yolov7.weights");
            break;

        case Globals::ObjectDetectorTypes::TINY:
            m_net = cv::dnn::readNetFromDarknet(p_yoloFolderPath + "yolov7-tiny.cfg", p_yoloFolderPath + "yolov7-tiny.weights");
            break;

        default:
            throw Globals::Exceptions::NotImplementedError();
            break;
        }

        switch (p_objectDetectorBackEnds)
        {
        case Globals::ObjectDetectorBackEnds::CPU:
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
            break;

        case Globals::ObjectDetectorBackEnds::CUDA:
            m_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            m_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
            break;

        default:
            throw Globals::Exceptions::NotImplementedError();
            break;
        }

        m_blobSize = static_cast<int32_t>(p_objectDetectorBlobSizes);

        m_unconnectedOutputLayerNames = m_net.getUnconnectedOutLayersNames();
    }

    void ObjectDetector::RunObjectDetector(const cv::Mat& p_frame)
    {
        if (m_skipObjectDetection)
        {
            return;
        }

        m_objectDetectionInformation.m_objectInformation.clear();

        // Get output blobs from the frame
        std::vector<cv::Mat> outputBlobs;
        const cv::Mat BLOB_FROM_IMAGE = cv::dnn::blobFromImage(p_frame, Globals::G_OBJECT_DETECTOR_SCALE_FACTOR, cv::Size(m_blobSize, m_blobSize), Globals::G_COLOUR_BLACK);
        m_net.setInput(BLOB_FROM_IMAGE);
        m_net.forward(outputBlobs, m_unconnectedOutputLayerNames);

        // Go through all output blobs and only allow those with confidence above threshold
        std::vector<std::string> initialObjectNames;
        std::vector<cv::Rect> initialObjectBoundingBoxes;
        std::vector<float> initialObjectConfidences;
        cv::Point maxConfidenceObjectIndex;
        double maxConfidence = -DBL_MAX;

        for (auto& outputBlob : outputBlobs)
        {
            for (int32_t rowIndex = 0; rowIndex < outputBlob.rows; rowIndex++)
            {
                // Each row is structured as the following [cx, cy, w, h, box score, object 1 score, object 2 score, ..., object n score]

                // minMaxLoc returns the object index that has the maximum confidence as well as the maximum confidence
                cv::minMaxLoc(outputBlob.row(rowIndex).colRange(Globals::G_OBJECT_DETECTOR_OUTPUT_BLOBS_OBJECT_SCORES_START_INDEX, outputBlob.cols), nullptr, &maxConfidence, nullptr, &maxConfidenceObjectIndex);

                if (maxConfidence > Globals::G_OBJECT_DETECTOR_CONFIDENCE_THRESHOLD)
                {
                    const double CENTER_X = outputBlob.at<float>(rowIndex, Globals::G_OBJECT_DETECTOR_OUTPUT_BLOBS_CENTER_X_COORD_INDEX) * Globals::G_VIDEO_INPUT_WIDTH;
                    const double CENTER_Y = outputBlob.at<float>(rowIndex, Globals::G_OBJECT_DETECTOR_OUTPUT_BLOBS_CENTER_Y_COORD_INDEX) * Globals::G_VIDEO_INPUT_HEIGHT;
                    const double WIDTH = (outputBlob.at<float>(rowIndex, Globals::G_OBJECT_DETECTOR_OUTPUT_BLOBS_WIDTH_INDEX) * Globals::G_VIDEO_INPUT_WIDTH) + Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_BUFFER;
                    const double HEIGHT = (outputBlob.at<float>(rowIndex, Globals::G_OBJECT_DETECTOR_OUTPUT_BLOBS_HEIGHT_INDEX) * Globals::G_VIDEO_INPUT_HEIGHT) + Globals::G_OBJECT_DETECTOR_BOUNDING_BOX_BUFFER;

                    // Remove object detections on the hood of car
                    if (CENTER_Y < Globals::G_ROI_BOTTOM_HEIGHT)
                    {
                        initialObjectNames.push_back(Globals::G_OBJECT_DETECTOR_OBJECT_NAMES.at(maxConfidenceObjectIndex.x));
                        initialObjectBoundingBoxes.emplace_back(CENTER_X - (WIDTH / Globals::G_DIVIDE_BY_TWO), CENTER_Y - (HEIGHT / Globals::G_DIVIDE_BY_TWO), WIDTH, HEIGHT);
                        initialObjectConfidences.push_back(static_cast<float>(maxConfidence));
                    }
                }
            }
        }

        // Apply non-maxima suppression to suppress overlapping bounding boxes for objects that overlap, the highest confidence object will be chosen
        std::vector<int> nonMaximaSuppressedFilteredIndicies;

        cv::dnn::NMSBoxes(initialObjectBoundingBoxes, initialObjectConfidences, 0.0, static_cast<float>(Globals::G_OBJECT_DETECTOR_NMS_THRESHOLD), nonMaximaSuppressedFilteredIndicies);

        for (const int32_t& index : nonMaximaSuppressedFilteredIndicies)
        {
            m_objectDetectionInformation.m_objectInformation.push_back({.m_boundingBox = initialObjectBoundingBoxes.at(index),
                                                                        .m_boundingBoxColour = Globals::G_OBJECT_DETECTOR_OBJECT_NAMES_AND_COLOURS.at(initialObjectNames.at(index)),
                                                                        .m_objectName = std::format("{} ({} %)", initialObjectNames.at(index), std::to_string(static_cast<uint32_t>(initialObjectConfidences.at(index) * Globals::G_CONVERT_DECIMAL_TO_PERCENTAGE)))});
        }
    }

    ObjectDetectionInformation ObjectDetector::GetInformation()
    {
        return m_objectDetectionInformation;
    }
}
