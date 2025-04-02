#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "helpers/Globals.hpp"
#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class ObjectDetector
     * @brief YOLOv4 based object detector.
     */
    class ObjectDetector
    {
    public:
        /**
         * @brief Construct a new %ObjectDetector object.
         */
        explicit ObjectDetector();

        /**
         * @brief Set the properties of the %ObjectDetector object.
         *
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_objectDetectorTypes The type of object detector to use with an option to disable object detection. The tiny
         * version is more performant at the cost of accuracy.
         * @param p_objectDetectorBackEnds The supported backends for the object detector to run on. CUDA is significantly more
         * performant.
         * @param p_objectDetectorBlobSizes The supported blob sizes for the object detector to run with. The larger the blob
         * size the more performant the detector at the cost of performance.
         */
        void SetProperties(const std::string& p_yoloFolderPath,
                           const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                           const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                           const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSizes);

        /**
         * @brief Run the object detector against `p_frame`.
         *
         * @param p_frame The frame to run the object detector against.
         */
        void RunObjectDetector(const cv::Mat& p_frame);

        /**
         * @brief Get the ObjectDetectionInformation struct.
         *
         * @return ObjectDetectionInformation The ObjectDetectionInformation struct.
         */
        ObjectDetectionInformation GetInformation();

    private:
        /**
         * @brief OpenCV object which allows the use of pre-trained neural networks.
         */
        cv::dnn::Net m_net;

        /**
         * @brief The ObjectDetectionInformation struct containing all object detection-related information.
         */
        ObjectDetectionInformation m_objectDetectionInformation;

        /**
         * @brief The names of layers with unconnected outputs.
         */
        std::vector<std::string> m_unconnectedOutputLayerNames;

        /**
         * @brief The spatial size for the output image used by the `cv::dnn::blobFromImage` function.
         */
        int32_t m_blobSize;

        /**
         * @brief Whether to skip object detection.
         */
        bool m_skipObjectDetection;
    };
}
