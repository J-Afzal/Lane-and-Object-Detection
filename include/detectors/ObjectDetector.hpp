#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/dnn/dnn.hpp>

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
 */
namespace LaneAndObjectDetection
{
    /**
     * @brief TODO
     */
    enum class Detectors : std::uint8_t
    {
        NONE = 0,
        STANDARD,
        TINY
    };

    /**
     * @brief TODO
     */
    enum class BackEnds : std::uint8_t
    {
        CPU = 0,
        CUDA
    };

    /**
     * @brief TODO
     */
    enum class BlobSizes : std::uint16_t
    {
        ONE = 288,
        TWO = 320,
        THREE = 416,
        FOUR = 512,
        FIVE = 608
    };

    /**
     * @class ObjectDetector
     * @brief TODO
     */
    class ObjectDetector
    {
    public:
        /**
         * @brief TODO
         */
        ObjectDetector();

        /**
         * @brief TODO
         */
        void SetProperties(
            const std::string& p_yoloResourcesFolderPath,
            const Detectors& p_yoloType,
            const BackEnds& p_backEndType,
            const BlobSizes& p_blobSize);

        /**
         * @brief TODO
         */
        void RunDetector(const cv::Mat& p_frame);

        /**
         * @brief TODO
         */
        std::vector<cv::Rect> GetBoundingBoxes();

        /**
         * @brief TODO
         */
        void PrintToFrame(cv::Mat& p_frame);

    private:
        /**
         * @brief TODO
         */
        ///@{
        std::map<std::string, cv::Scalar> m_modelNamesAndColourList;
        std::vector<cv::Mat> m_outputBlobs;
        std::vector<cv::Rect> m_boundingBoxes;
        std::vector<cv::Rect> m_preNmsObjectBoundingBoxes;
        std::vector<std::string> m_unconnectedOutputLayerNames;
        std::vector<std::string> m_modelNames;
        std::vector<std::string> m_names;
        std::vector<std::string> m_preNmsObjectNames;
        std::vector<std::string> m_trafficLightStates;
        std::vector<cv::Point2f> m_srcTrafficLight;
        std::vector<cv::Point2f> m_dstTrafficLight;
        std::vector<float> m_confidences;
        std::vector<float> m_preNmsObjectConfidences;
        std::vector<int> m_indicesAfterNms;
        cv::dnn::Net m_net;
        cv::Point m_classId;
        cv::Mat m_blobFromImage;
        cv::Mat m_croppedImage;
        cv::Mat m_croppedImageInHsv;
        uint32_t m_nonZeroPixelsInGreen;
        uint32_t m_nonZeroPixelsInRed;
        uint32_t m_blobSize;
        double m_confidence;
        double m_centerX;
        double m_centerY;
        double m_width;
        double m_height;
        bool m_skipDetection;
        ///@}
    };
}
