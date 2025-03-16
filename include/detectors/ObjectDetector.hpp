#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn/dnn.hpp>

#include "helpers/Information.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{

    /**
     * @brief The type of object detector to use with an option to disable object detection. The tiny version is more performant
     * at the cost of accuracy.
     */
    enum class ObjectDetectorTypes : std::uint8_t
    {
        DEFAULT = 0,
        NONE,
        STANDARD,
        TINY
    };

    /**
     * @brief The supported backends for the object detector to run on. CUDA is significantly more performant.
     */
    enum class ObjectDetectorBackEnds : std::uint8_t
    {
        CPU = 0,
        CUDA
    };

    /**
     * @brief The supported blob sizes for the object detector to run with. The larger the blob size the more performant the
     * detector at the cost of performance.
     */
    enum class ObjectDetectorBlobSizes : std::int16_t
    {
        ONE = 288,
        TWO = 320,
        THREE = 416,
        FOUR = 512,
        FIVE = 608
    };

    /**
     * @class ObjectDetector
     * @brief The object detector built on YOLOv4.
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
                           const ObjectDetectorTypes& p_objectDetectorTypes,
                           const ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                           const ObjectDetectorBlobSizes& p_objectDetectorBlobSizes);

        /**
         * @brief Run the object detector.
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
         * @brief The spatial size for the output image used by the cv::dnn::blobFromImage function.
         */
        int32_t m_blobSize;

        /**
         * @brief Whether to skip object detection.
         */
        bool m_skipObjectDetection;
    };
}
