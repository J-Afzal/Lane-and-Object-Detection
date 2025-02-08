// NOLINTBEGIN

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
    enum class Detector
    {
        NONE = 0,
        STANDARD = 1,
        TINY = 2
    };

    /**
     * @brief TODO
     */
    enum class BackEnd
    {
        CPU = 0,
        CUDA = 1
    };

    /**
     * @brief TODO
     */
    enum class BlobSize
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
        ObjectDetector(const std::string& p_yoloResourcesFolderPath, const Detector& p_yoloType, const BackEnd& p_backEndType, const BlobSize& p_blobSize);

        /**
         * @brief TODO
         */
        ~ObjectDetector() = default;

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
        std::vector<cv::Rect> m_boundingBoxes, m_preNmsObjectBoundingBoxes;
        std::vector<std::string> m_unconnectedOutputLayerNames, m_modelNames, m_names, m_preNmsObjectNames, m_trafficLightStates;
        std::vector<cv::Point2f> m_srcTrafficLight, m_dstTrafficLight;
        std::vector<float> m_confidences;
        std::vector<float> m_preNmsObjectConfidences;
        std::vector<int> m_indicesAfterNms;
        cv::dnn::Net m_net;
        cv::Point m_classId;
        cv::Mat m_blobFromImage, m_croppedImage, m_croppedImageInHsv;
        double m_confidence {}, m_centerX {}, m_centerY {}, m_width {}, m_height {};
        uint32_t m_nonZeroPixelsInGreen {}, m_nonZeroPixelsInRed {}, m_blobSize {};
        bool m_skipDetection {};

        const double m_YOLO_CONFIDENCE_THRESHOLD = 0.4;
        const float m_YOLO_NMS_THRESHOLD = 0.4;
        const uint32_t m_BOUNDING_BOX_BUFFER = 5, m_VIDEO_WIDTH = 1920, m_VIDEO_HEIGHT = 1080, m_ROI_BOTTOM_HEIGHT = 840;
        ///@}
    };
}

// NOLINTEND
