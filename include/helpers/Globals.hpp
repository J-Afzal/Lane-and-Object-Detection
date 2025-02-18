#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <opencv2/core/cvdef.h>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

#include "detectors/ObjectDetector.hpp"

/**
 * @namespace LaneAndObjectDetection::Globals
 * @brief Contains all global and constant objects.
 */
namespace LaneAndObjectDetection::Globals
{
    /**
     * @brief Input video dimensions.
     */
    ///@{
    static inline const uint32_t G_INPUT_VIDEO_HEIGHT = 1080;
    static inline const uint32_t G_INPUT_VIDEO_WIDTH = 1920;
    ///@}

    /**
     * @brief Output video dimensions.
     */
    ///@{
    static inline const uint32_t G_OUTPUT_VIDEO_HEIGHT = 1080;
    static inline const uint32_t G_OUTPUT_VIDEO_WIDTH = 1920;
    ///@}

    /**
     * @brief Output video FPS
     */
    static inline const uint32_t G_OUTPUT_VIDEO_FPS = 30;

    /**
     * @brief UI component locations
     */
    ///@{
    static inline const cv::Rect G_FPS_RECT = cv::Rect(25, 25, 350, 100); // TODO: combine these in to bottom bar?
    static inline const cv::Rect G_RECORDING_STATUS_RECT = cv::Rect(1495, 410, 400, 50);
    static inline const cv::Rect G_RIGHT_INFO_RECT = cv::Rect(1495, 25, 400, 360); // Except this?
    ///@}

    /**
     * @brief Font settings.
     */
    ///@{
    static inline const uint32_t G_DEFAULT_FONT_FACE = cv::FONT_HERSHEY_DUPLEX; // TODO: better font settings?
    static inline const uint32_t G_DEFAULT_FONT_THICKNESS = 1;
    static inline const double G_DEFAULT_FONT_SCALE = 1;
    static inline const double G_SMALL_FONT_SCALE = 0.8;
    static inline const double G_EXTRA_SMALL_FONT_SCALE = 0.5;
    ///@}

    /**
     * @brief OpenCV Colours.
     */
    ///@{
    static inline const cv::Scalar G_OPENCV_BLACK = cv::Scalar(0, 0, 0); // TODO: standardise colours
    static inline const cv::Scalar G_OPENCV_WHITE = cv::Scalar(255, 255, 255);
    static inline const cv::Scalar G_OPENCV_BLUE = cv::Scalar(255, 64, 64);
    static inline const cv::Scalar G_OPENCV_PURPLE = cv::Scalar(255, 64, 255);
    static inline const cv::Scalar G_OPENCV_RED = cv::Scalar(64, 64, 255);
    static inline const cv::Scalar G_OPENCV_YELLOW = cv::Scalar(64, 255, 255);
    static inline const cv::Scalar G_OPENCV_BRIGHTER_YELLOW = cv::Scalar(0, 200, 200);
    static inline const cv::Scalar G_OPENCV_GREEN = cv::Scalar(0, 64, 0);
    ///@}

    /**
     * @brief Region of interest dimensions.
     */
    ///@{
    static inline const uint32_t G_ROI_TOP_HEIGHT = 660; // TODO: this might need to be dynamic so move out of globals
    static inline const uint32_t G_ROI_BOTTOM_HEIGHT = 840;
    static inline const uint32_t G_ROI_TOP_WIDTH = 200;
    static inline const uint32_t G_ROI_BOTTOM_WIDTH = 900;
    static inline const uint32_t G_NUMBER_OF_POINTS = 4;
    static inline const std::array<cv::Point, G_NUMBER_OF_POINTS> G_MASK_DIMENSIONS = {
        cv::Point((G_VIDEO_WIDTH / 2) - (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_WIDTH / 2) + (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_WIDTH / 2) + (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
        cv::Point((G_VIDEO_WIDTH / 2) - (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
    };
    ///@}

    /**
     * @brief Region of interest sub-division line equations.
     */
    ///@{
    static inline const double G_LEFT_EDGE_OF_MASK_M = (static_cast<double>(G_MASK_DIMENSIONS[0].y) - static_cast<double>(G_MASK_DIMENSIONS[3].y)) / (static_cast<double>(G_MASK_DIMENSIONS[0].x) - static_cast<double>(G_MASK_DIMENSIONS[3].x)); // TODO: this might need to be dynamic so move out of globals
    static inline const double G_LEFT_EDGE_OF_MASK_C = G_MASK_DIMENSIONS[0].y - (G_LEFT_EDGE_OF_MASK_M * G_MASK_DIMENSIONS[0].x);
    static inline const double G_RIGHT_EDGE_OF_MASK_M = (static_cast<double>(G_MASK_DIMENSIONS[1].y) - static_cast<double>(G_MASK_DIMENSIONS[2].y)) / (static_cast<double>(G_MASK_DIMENSIONS[1].x) - static_cast<double>(G_MASK_DIMENSIONS[2].x));
    static inline const double G_RIGHT_EDGE_OF_MASK_C = G_MASK_DIMENSIONS[1].y - (G_RIGHT_EDGE_OF_MASK_M * G_MASK_DIMENSIONS[1].x);

    static inline const double G_TOP_MID_POINT = G_MASK_DIMENSIONS[0].x + ((static_cast<double>(G_MASK_DIMENSIONS[1].x) - static_cast<double>(G_MASK_DIMENSIONS[0].x)) / 2.);
    static inline const double G_BOTTOM_ONE_THIRD = G_MASK_DIMENSIONS[3].x + ((static_cast<double>(G_MASK_DIMENSIONS[2].x) - static_cast<double>(G_MASK_DIMENSIONS[3].x)) / 3.);
    static inline const double G_BOTTOM_TWO_THIRD = G_MASK_DIMENSIONS[3].x + (2. * (static_cast<double>(G_MASK_DIMENSIONS[2].x) - static_cast<double>(G_MASK_DIMENSIONS[3].x)) / 3.);

    static inline const double G_LEFT_EDGE_THRESHOLD_M = (static_cast<double>(G_ROI_TOP_HEIGHT) - static_cast<double>(G_ROI_BOTTOM_HEIGHT)) / (G_TOP_MID_POINT - G_BOTTOM_ONE_THIRD);
    static inline const double G_LEFT_EDGE_THRESHOLD_C = G_ROI_TOP_HEIGHT - (G_LEFT_EDGE_THRESHOLD_M * G_TOP_MID_POINT);
    static inline const double G_RIGHT_EDGE_THRESHOLD_M = (static_cast<double>(G_ROI_TOP_HEIGHT) - static_cast<double>(G_ROI_BOTTOM_HEIGHT)) / (G_TOP_MID_POINT - G_BOTTOM_TWO_THIRD);
    static inline const double G_RIGHT_EDGE_THRESHOLD_C = G_ROI_TOP_HEIGHT - (G_RIGHT_EDGE_THRESHOLD_M * G_TOP_MID_POINT);
    ///@}

    /**
     * @brief Canny algorithm thresholds.
     */
    ///@{
    static inline const uint32_t G_CANNY_LOWER_THRESHOLD = 128; // TODO: this might need to be dynamic so move out of globals
    static inline const uint32_t G_CANNY_UPPER_THRESHOLD = 255;
    ///@}

    /**
     * @brief Hough transform threshold and properties.
     */
    ///@{
    static inline const double G_HOUGH_RHO = 1; // TODO: this might need to be dynamic so move out of globals
    static inline const double G_HOUGH_THETA = CV_PI / 180.0;
    static inline const uint32_t G_HOUGH_THRESHOLD = 32;
    static inline const uint32_t G_HOUGH_MIN_LINE_LENGTH = 16;
    static inline const uint32_t G_HOUGH_MAX_LINE_GAP = 8;
    ///@}

    /**
     * @brief Threshold length to decide whether a line is to be considered solid line road marking.
     */
    static inline const uint32_t G_SOLID_LINE_LENGTH_THRESHOLD = 75; // TODO: this might need to be dynamic so move out of globals

    /**
     * @brief Threshold gradient to decide whether a line is to be considered horizontal.
     */
    static inline const double G_HORIZONTAL_GRADIENT_THRESHOLD = 0.15; // TODO: this might need to be dynamic so move out of globals

    /**
     * @brief Threshold length to decide whether a line is to be added to the count of horizontal lines.
     */
    static inline const uint32_t G_HORIZONTAL_LENGTH_THRESHOLD = 50; // TODO: this might need to be dynamic so move out of globals

    /**
     * @brief Threshold count to decide whether enough horizontal lines have been detected to prompt a giveway warning.
     */
    static inline const uint32_t G_HORIZONTAL_COUNT_THRESHOLD = 10; // TODO: this might need to be dynamic so move out of globals

    /**
     * @brief Smoothing threshold to determine when to flag the vehicle as changing lanes.
     */
    static inline const uint32_t G_FRAME_COUNT_THRESHOLD = 10; // TODO: this might need to be dynamic so move out of globals

    /**
     * @brief Rolling average properties.
     */
    ///@{
    static inline const uint32_t G_DEFAULT_ROLLING_AVERAGE_SIZE = 10; // TODO: this might need to be dynamic so move out of globals
    static inline const uint32_t G_NUMBER_OF_HORIZONTAL_LINE_STATES = 2;
    static inline const uint32_t G_NUMBER_OF_VERTICAL_LINE_STATES = 3;
    static inline const uint32_t G_NUMBER_OF_DRIVING_STATES = 5;
    ///@}

    /**
     * @brief YOLO threshold and properties.
     */
    ///@{
    static inline const uint32_t G_BOUNDING_BOX_BUFFER = 5; // TODO: this might need to be dynamic so move out of globals
    static inline const double G_YOLO_CONFIDENCE_THRESHOLD = 0.4;
    static inline const float G_YOLO_NMS_THRESHOLD = 0.4;
    ///@}

    /**
     * @brief Performance tests settings.
     */
    ///@{
    static inline const uint32_t G_NUMBER_OF_TESTS = 21;
    static inline const uint32_t G_NUMBER_OF_REPETITIONS = 5;

    static inline const std::array<Detectors, G_NUMBER_OF_TESTS> G_YOLO_TYPES = {
        Detectors::NONE,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::TINY,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD,
        Detectors::STANDARD};

    static inline const std::array<BackEnds, G_NUMBER_OF_TESTS> G_BACK_END_TYPES = {
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CUDA,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
        BackEnds::CPU,
    };

    static inline const std::array<BlobSizes, G_NUMBER_OF_TESTS> G_BLOB_SIZES = {
        BlobSizes::ONE,
        BlobSizes::ONE,
        BlobSizes::TWO,
        BlobSizes::THREE,
        BlobSizes::FOUR,
        BlobSizes::FIVE,
        BlobSizes::ONE,
        BlobSizes::TWO,
        BlobSizes::THREE,
        BlobSizes::FOUR,
        BlobSizes::FIVE,
        BlobSizes::ONE,
        BlobSizes::TWO,
        BlobSizes::THREE,
        BlobSizes::FOUR,
        BlobSizes::FIVE,
        BlobSizes::ONE,
        BlobSizes::TWO,
        BlobSizes::THREE,
        BlobSizes::FOUR,
        BlobSizes::FIVE,
    };

    static inline const std::array<std::string, G_NUMBER_OF_TESTS> G_OUTPUT_FILE_NAMES = {
        "no_yolov4.txt",
        "yolov4-tiny_288_cuda.txt",
        "yolov4-tiny_320_cuda.txt",
        "yolov4-tiny_416_cuda.txt",
        "yolov4-tiny_512_cuda.txt",
        "yolov4-tiny_608_cuda.txt",
        "yolov4_288_cuda.txt",
        "yolov4_320_cuda.txt",
        "yolov4_416_cuda.txt",
        "yolov4_512_cuda.txt",
        "yolov4_608_cuda.txt",
        "yolov4-tiny_288.txt",
        "yolov4-tiny_320.txt",
        "yolov4-tiny_416.txt",
        "yolov4-tiny_512.txt",
        "yolov4-tiny_608.txt",
        "yolov4_288.txt",
        "yolov4_320.txt",
        "yolov4_416.txt",
        "yolov4_512.txt",
        "yolov4_608.txt",
    };
    ///@}
}
