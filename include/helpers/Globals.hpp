#pragma once

#include <array>
#include <cstdint>
#include <exception>
#include <map>
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
     * @namespace LaneAndObjectDetection::Globals::Exceptions
     * @brief Contains all custom exceptions.
     */
    namespace Exceptions
    {
        /**
         * @brief Used for functionality that has not been implemented.
         */
        class NotImplementedError : public std::exception
        {
        };
    }

    /**
     * @brief CLI help message for the video manager.
     */
    static inline const std::string G_CLI_HELP_MESSAGE = "\nUsage: lane-and-object-detection --input ... --yolo-folder-path ... --object-detector-type ... [optional]\n\nOPTIONS:\n\nGeneric Options:\n\n  -h --help                       Display available options\n\nRequired Options:\n\n  -i --input                      File path or camera ID\n  -y --yolo-folder-path           Path to the yolo folder\n  -o --object-detector-type       One of: none, standard or tiny\n\nOptional options:\n\n  -b --object-detector-backend    One of: cpu or cuda (default = cpu)\n  -s --object-detector-blob-size  One of: 208, 320, 416, 512 or 608 (default = 208)\n\n";

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
     * @brief Keyboard values when getting user input.
     */
    ///@{
    static inline const uint32_t G_TOGGLE_RECORDING_KEY = 'r';
    static inline const uint32_t G_QUIT_KEY = 'q';
    ///@}

    /**
     * @brief Font settings.
     */
    ///@{
    static inline const uint32_t G_DEFAULT_FONT_FACE = cv::FONT_HERSHEY_DUPLEX;
    static inline const uint32_t G_DEFAULT_FONT_THICKNESS = 1;
    static inline const double G_DEFAULT_FONT_SCALE = 1;
    ///@}

    /**
     * @brief OpenCV Colours (in BGR format).
     */
    ///@{
    static inline const cv::Scalar G_COLOUR_BLACK = cv::Scalar(0, 0, 0);
    static inline const cv::Scalar G_COLOUR_GREY = cv::Scalar(128, 128, 128);
    static inline const cv::Scalar G_COLOUR_WHITE = cv::Scalar(255, 255, 255);
    static inline const cv::Scalar G_COLOUR_RED = cv::Scalar(0, 0, 192);
    static inline const cv::Scalar G_COLOUR_ORANGE = cv::Scalar(0, 128, 192);
    static inline const cv::Scalar G_COLOUR_YELLOW = cv::Scalar(0, 192, 192);
    static inline const cv::Scalar G_COLOUR_GREEN = cv::Scalar(0, 192, 0);
    static inline const cv::Scalar G_COLOUR_BLUE = cv::Scalar(192, 0, 0);
    static inline const cv::Scalar G_COLOUR_PURPLE = cv::Scalar(192, 0, 192);
    ///@}

    /**
     * @brief Text to display whether recording or not recording.
     */
    ///@{
    static inline const std::string G_UI_TEXT_RECORDING = "Recording Output";
    static inline const std::string G_UI_TEXT_NOT_RECORDING = "Press 'r' to start recording";
    ///@}

    /**
     * @brief Generic UI element properties.
     */
    ///@{
    static inline const uint32_t G_UI_PADDING = 30;
    static inline const uint32_t G_UI_TITLE_HEIGHT = 50;
    static inline const uint32_t G_UI_SUBTITLE_HEIGHT = 30;
    ///@}

    /**
     * @brief Lane detector-related information UI widths.
     */
    ///@{
    static inline const uint32_t G_UI_DRIVING_STATE_WIDTH = 500;
    static inline const uint32_t G_UI_TURNING_STATE_WIDTH = 300;
    static inline const uint32_t G_UI_LANE_INFORMATION_WIDTH = 200;
    ///@}

    /**
     * @brief Lane detector-related information UI locations.
     */
    ///@{
    static inline const cv::Rect G_UI_DRIVING_STATE_RECT = cv::Rect((Globals::G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2.0) - (G_LANE_DETECTOR_LANE_INFORMATION_DRIVING_STATE_WIDTH / 2.0), G_UI_PADDING, G_LANE_DETECTOR_LANE_INFORMATION_DRIVING_STATE_WIDTH, G_TITLE_ELEMENT_HEIGHT);
    static inline const cv::Rect G_UI_TURNING_STATE_RECT = cv::Rect((Globals::G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2.0) - (G_LANE_DETECTOR_LANE_INFORMATION_TURNING_STATE_WIDTH / 2.0), G_LANE_DETECTOR_LANE_INFORMATION_DRIVING_STATE_RECT.y + G_LANE_DETECTOR_LANE_INFORMATION_DRIVING_STATE_RECT.height + G_UI_PADDING, G_LANE_DETECTOR_LANE_INFORMATION_TURNING_STATE_WIDTH, G_SUBTITLE_ELEMENT_HEIGHT);
    static inline const cv::Rect G_UI_LANE_INFORMATION_TITLE_RECT = cv::Rect(G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_RHS_UI_ELEMENT_WIDTH, G_UI_PADDING, G_RHS_UI_ELEMENT_WIDTH, G_SUBTITLE_ELEMENT_HEIGHT);
    static inline const cv::Rect G_UI_LANE_INFORMATION_RECT = cv::Rect(G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_RHS_UI_ELEMENT_WIDTH, G_LANE_DETECTOR_LANE_INFORMATION_TITLE_RECT.y + G_LANE_DETECTOR_LANE_INFORMATION_TITLE_RECT.height + G_UI_PADDING, G_RHS_UI_ELEMENT_WIDTH, 200);
    static inline const cv::Rect G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT = cv::Rect(G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_RHS_UI_ELEMENT_WIDTH, G_LANE_DETECTOR_LANE_INFORMATION_RECT.y + G_LANE_DETECTOR_LANE_INFORMATION_RECT.height + G_UI_PADDING, G_RHS_UI_ELEMENT_WIDTH, G_SUBTITLE_ELEMENT_HEIGHT);
    ///@}

    /**
     * @brief Recording status UI location.
     */
    static inline const cv::Rect G_RECORDING_STATUS_RECT = cv::Rect(G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH - G_RHS_UI_ELEMENT_WIDTH - G_UI_PADDING, G_LANE_DETECTOR_LANE_INFORMATION_TURING_RECT.y + G_LANE_DETECTOR_LANE_INFORMATION_TURING_RECT.height + G_UI_PADDING, G_RHS_UI_ELEMENT_WIDTH, G_SUBTITLE_ELEMENT_HEIGHT);

    /**
     * @brief Lane information-related lane state UI properties.
     */
    ///@{
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_PAD = 30;
    static inline const int32_t G_LANE_INFORMATION_LEFT_LANE_STATE_X_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.x + G_LANE_DETECTOR_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_MIDDLE_LANE_STATE_X_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.x + (G_LANE_DETECTOR_LANE_INFORMATION_RECT.width / 2.0);
    static inline const int32_t G_LANE_INFORMATION_RIGHT_LANE_STATE_X_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.x + G_LANE_DETECTOR_LANE_INFORMATION_RECT.width - G_LANE_DETECTOR_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_Y_START_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.y + G_LANE_DETECTOR_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_WIDTH = 3;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_HEIGHT = 15;
    ///@}

    /**
     * @brief Lane information-related vehicle position UI properties.
     */
    ///@{
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_WIDTH = G_LANE_DETECTOR_LANE_INFORMATION_RECT.width - (4 * G_UI_PADDING);
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_HEIGHT = G_LANE_DETECTOR_LANE_INFORMATION_RECT.height - (3 * G_UI_PADDING);
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_X_MIDDLE_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.x + (G_LANE_DETECTOR_LANE_INFORMATION_RECT.width / 2.0) - (G_LANE_DETECTION_VEHICLE_POSITION_WIDTH / 2.0);
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_Y_LOCATION = G_LANE_DETECTOR_LANE_INFORMATION_RECT.y + (1.5 * G_UI_PADDING);
    ///@}

    /**
     * @brief Translucent colour of the overlay for the current lane.
     */
    static inline const cv::Scalar G_LANE_INFORMATION_LANE_OVERLAY_COLOUR = cv::Scalar(0, 64, 0);
    
    /**
     * @brief Region of interest dimensions.
     */
    ///@{
    static inline const uint32_t G_ROI_TOP_HEIGHT = 660;
    static inline const uint32_t G_ROI_BOTTOM_HEIGHT = 840;
    static inline const uint32_t G_ROI_TOP_WIDTH = 200;
    static inline const uint32_t G_ROI_BOTTOM_WIDTH = 900;
    static inline const uint32_t G_NUMBER_OF_POINTS = 4;
    static inline const std::array<cv::Point, G_NUMBER_OF_POINTS> G_MASK_DIMENSIONS = {
        cv::Point((G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2) - (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2) + (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2) + (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
        cv::Point((G_VIDEO_MANAGER_INPUT_VIDEO_WIDTH / 2) - (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
    };
    ///@}

    /**
     * @brief Region of interest sub-division line equations.
     */
    ///@{
    static inline const double G_LEFT_EDGE_OF_MASK_M = (static_cast<double>(G_MASK_DIMENSIONS[0].y) - static_cast<double>(G_MASK_DIMENSIONS[3].y)) / (static_cast<double>(G_MASK_DIMENSIONS[0].x) - static_cast<double>(G_MASK_DIMENSIONS[3].x));
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
    static inline const uint32_t G_CANNY_LOWER_THRESHOLD = 128;
    static inline const uint32_t G_CANNY_UPPER_THRESHOLD = 255;
    ///@}

    /**
     * @brief Hough transform threshold and properties.
     */
    ///@{
    static inline const double G_HOUGH_RHO = 1;
    static inline const double G_HOUGH_THETA = CV_PI / 180.0;
    static inline const uint32_t G_HOUGH_THRESHOLD = 32;
    static inline const uint32_t G_HOUGH_MIN_LINE_LENGTH = 16;
    static inline const uint32_t G_HOUGH_MAX_LINE_GAP = 8;
    ///@}

    /**
     * @brief Threshold length to decide whether a line is to be considered solid line road marking.
     */
    static inline const uint32_t G_SOLID_LINE_LENGTH_THRESHOLD = 75;

    /**
     * @brief Threshold gradient to decide whether a line is to be considered horizontal.
     */
    static inline const double G_HORIZONTAL_GRADIENT_THRESHOLD = 0.15;

    /**
     * @brief Threshold length to decide whether a line is to be added to the count of horizontal lines.
     */
    static inline const uint32_t G_HORIZONTAL_LENGTH_THRESHOLD = 50;

    /**
     * @brief Threshold count to decide whether enough horizontal lines have been detected to prompt a giveway warning.
     */
    static inline const uint32_t G_HORIZONTAL_COUNT_THRESHOLD = 10;

    /**
     * @brief Smoothing threshold to determine when to flag the vehicle as changing lanes.
     */
    static inline const uint32_t G_FRAME_COUNT_THRESHOLD = 10;

    /**
     * @brief Rolling average properties.
     */
    ///@{
    static inline const uint32_t G_DEFAULT_ROLLING_AVERAGE_SIZE = 10;
    static inline const uint32_t G_NUMBER_OF_HORIZONTAL_LINE_STATES = 2;
    static inline const uint32_t G_NUMBER_OF_VERTICAL_LINE_STATES = 3;
    static inline const uint32_t G_NUMBER_OF_DRIVING_STATES = 5;
    ///@}

    /**
     * @brief Object detection threshold and properties.
     */
    ///@{
    static inline const double G_OBJECT_DETECTOR_SCALE_FACTOR = 1 / 255.0;
    static inline const double G_OBJECT_DETECTOR_CONFIDENCE_THRESHOLD = 0.4;
    static inline const double G_OBJECT_DETECTOR_NMS_THRESHOLD = 0.4;
    ///@}

    /**
     * @brief Object detection bounding box properties.
     */
    ///@{
    static inline const uint32_t G_OBJECT_DETECTOR_BOUNDING_BOX_BUFFER = 5;
    static inline const int32_t G_OBJECT_DETECTOR_BOUNDING_BOX_HEADER_HEIGHT = 15;
    static inline const int32_t G_OBJECT_DETECTOR_BOUNDING_BOX_BORDER_THICKNESS = 1;
    static inline const int32_t G_OBJECT_DETECTOR_BOUNDING_BOX_CHARACTER_WIDTH = 9;
    static inline const int32_t G_OBJECT_DETECTOR_BOUNDING_BOX_TEXT_HEIGHT_OFFSET = 2;
    static inline const double G_OBJECT_DETECTOR_BOUNDING_BOX_FONT_SCALE = 0.5;
    ///@}

    /**
     * @brief The indicies representing various values in the output blobs.
     */
    ///@{
    static inline const int32_t G_OBJECT_DETECTOR_OUTPUT_BLOBS_CENTER_X_COORD_INDEX = 0;
    static inline const int32_t G_OBJECT_DETECTOR_OUTPUT_BLOBS_CENTER_Y_COORD_INDEX = 1;
    static inline const int32_t G_OBJECT_DETECTOR_OUTPUT_BLOBS_WIDTH_INDEX = 2;
    static inline const int32_t G_OBJECT_DETECTOR_OUTPUT_BLOBS_HEIGHT_INDEX = 3;
    static inline const int32_t G_OBJECT_DETECTOR_OUTPUT_BLOBS_OBJECT_SCORES_START_INDEX = 5;
    ///@}

    /**
     * @brief Names of detectable objects. The order is significant and should not be changed.
     */
    ///@{
    static inline const uint32_t G_OBJECT_DETECTOR_NUMBER_OF_DETECTABLE_OBJECTS = 80;
    static inline const std::array<std::string, G_OBJECT_DETECTOR_NUMBER_OF_DETECTABLE_OBJECTS> G_OBJECT_DETECTOR_OBJECT_NAMES = {
        "person",
        "bicycle",
        "car",
        "motorbike",
        "aeroplane",
        "bus",
        "train",
        "truck",
        "boat",
        "traffic light",
        "fire hydrant",
        "stop sign",
        "parking meter",
        "bench",
        "bird",
        "cat",
        "dog",
        "horse",
        "sheep",
        "cow",
        "elephant",
        "bear",
        "zebra",
        "giraffe",
        "backpack",
        "umbrella",
        "handbag",
        "tie",
        "suitcase",
        "frisbee",
        "skis",
        "snowboard",
        "sports ball",
        "kite",
        "baseball bat",
        "baseball glove",
        "skateboard",
        "surfboard",
        "tennis racket",
        "bottle",
        "wine glass",
        "cup",
        "fork",
        "knife",
        "spoon",
        "bowl",
        "banana",
        "apple",
        "sandwich",
        "orange",
        "broccoli",
        "carrot",
        "hot dog",
        "pizza",
        "donut",
        "cake",
        "chair",
        "sofa",
        "pottedplant",
        "bed",
        "diningtable",
        "toilet",
        "tvmonitor",
        "laptop",
        "mouse",
        "remote",
        "keyboard",
        "cell phone",
        "microwave",
        "oven",
        "toaster",
        "sink",
        "refrigerator",
        "book",
        "clock",
        "vase",
        "scissors",
        "teddy bear",
        "hair drier",
        "toothbrush",
    };
    ///@}

    /**
     * @brief Object names and bounding box colours. G_OPENCV_WHITE is used as the default colour while custom colours are given
     * to object more commonly found while driving.
     */
    static inline const std::map<std::string, cv::Scalar> G_OBJECT_DETECTOR_OBJECT_NAMES_AND_COLOURS = {
        {"aeroplane",      G_OPENCV_WHITE },
        {"apple",          G_OPENCV_WHITE },
        {"backpack",       G_OPENCV_WHITE },
        {"banana",         G_OPENCV_WHITE },
        {"baseball bat",   G_OPENCV_WHITE },
        {"baseball glove", G_OPENCV_WHITE },
        {"bear",           G_OPENCV_WHITE },
        {"bed",            G_OPENCV_WHITE },
        {"bench",          G_OPENCV_WHITE },
        {"bicycle",        G_OPENCV_ORANGE},
        {"bird",           G_OPENCV_WHITE },
        {"boat",           G_OPENCV_WHITE },
        {"book",           G_OPENCV_WHITE },
        {"bottle",         G_OPENCV_WHITE },
        {"bowl",           G_OPENCV_WHITE },
        {"broccoli",       G_OPENCV_WHITE },
        {"bus",            G_OPENCV_GREY  },
        {"cake",           G_OPENCV_WHITE },
        {"car",            G_OPENCV_YELLOW},
        {"carrot",         G_OPENCV_WHITE },
        {"cat",            G_OPENCV_WHITE },
        {"cell phone",     G_OPENCV_WHITE },
        {"chair",          G_OPENCV_WHITE },
        {"clock",          G_OPENCV_WHITE },
        {"cow",            G_OPENCV_WHITE },
        {"cup",            G_OPENCV_WHITE },
        {"diningtable",    G_OPENCV_WHITE },
        {"dog",            G_OPENCV_WHITE },
        {"donut",          G_OPENCV_WHITE },
        {"elephant",       G_OPENCV_WHITE },
        {"fire hydrant",   G_OPENCV_WHITE },
        {"fork",           G_OPENCV_WHITE },
        {"frisbee",        G_OPENCV_WHITE },
        {"giraffe",        G_OPENCV_WHITE },
        {"hair drier",     G_OPENCV_WHITE },
        {"handbag",        G_OPENCV_WHITE },
        {"horse",          G_OPENCV_WHITE },
        {"hot dog",        G_OPENCV_WHITE },
        {"keyboard",       G_OPENCV_WHITE },
        {"kite",           G_OPENCV_WHITE },
        {"knife",          G_OPENCV_WHITE },
        {"laptop",         G_OPENCV_WHITE },
        {"microwave",      G_OPENCV_WHITE },
        {"motorbike",      G_OPENCV_GREEN },
        {"mouse",          G_OPENCV_WHITE },
        {"orange",         G_OPENCV_WHITE },
        {"oven",           G_OPENCV_WHITE },
        {"parking meter",  G_OPENCV_WHITE },
        {"person",         G_OPENCV_BLUE  },
        {"pizza",          G_OPENCV_WHITE },
        {"pottedplant",    G_OPENCV_WHITE },
        {"refrigerator",   G_OPENCV_WHITE },
        {"remote",         G_OPENCV_WHITE },
        {"sandwich",       G_OPENCV_WHITE },
        {"scissors",       G_OPENCV_WHITE },
        {"sheep",          G_OPENCV_WHITE },
        {"sink",           G_OPENCV_WHITE },
        {"skateboard",     G_OPENCV_WHITE },
        {"skis",           G_OPENCV_WHITE },
        {"snowboard",      G_OPENCV_WHITE },
        {"sofa",           G_OPENCV_WHITE },
        {"spoon",          G_OPENCV_WHITE },
        {"sports ball",    G_OPENCV_WHITE },
        {"stop sign",      G_OPENCV_WHITE },
        {"suitcase",       G_OPENCV_WHITE },
        {"surfboard",      G_OPENCV_WHITE },
        {"teddy bear",     G_OPENCV_WHITE },
        {"tennis racket",  G_OPENCV_WHITE },
        {"tie",            G_OPENCV_WHITE },
        {"toaster",        G_OPENCV_WHITE },
        {"toilet",         G_OPENCV_WHITE },
        {"toothbrush",     G_OPENCV_WHITE },
        {"traffic light",  G_OPENCV_RED   },
        {"train",          G_OPENCV_WHITE },
        {"truck",          G_OPENCV_PURPLE},
        {"tvmonitor",      G_OPENCV_WHITE },
        {"umbrella",       G_OPENCV_WHITE },
        {"vase",           G_OPENCV_WHITE },
        {"wine glass",     G_OPENCV_WHITE },
        {"zebra",          G_OPENCV_WHITE },
    };

    /**
     * @brief Performance-related information UI location.
     */
    ///@{
    static inline const cv::Rect G_PERFORMANCE_AVERAGE_FPS_BACKGROUND_RECT = cv::Rect(30, 30, 250, 30);
    static inline const cv::Rect G_PERFORMANCE_CURRENT_FPS_BACKGROUND_RECT = cv::Rect(30, 90, 250, 30);
    ///@}

    /**
     * @brief CLI help message for the performance tests.
     */
    static inline const std::string G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE = "Usage: lane-and-object-detection-performance-tests --input ... --yolo-folder-path ... --repetitions ...\n\nOPTIONS:\n\nGeneric Options:\n\n-h --help              Display available options\n\nRequired Options:\n\n-i --input             Benchmark video file path\n-y --yolo-folder-path  Path to the yolo folder\n-r --repetitions       Number of repetitions for each test";

    /**
     * @brief Performance tests settings.
     */
    ///@{
    static inline const uint32_t G_PERFORMANCE_TESTS_NUMBER_OF_TESTS = 21;

    static inline const std::array<ObjectDetectorTypes, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES = {
        ObjectDetectorTypes::NONE,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::TINY,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD,
        ObjectDetectorTypes::STANDARD};

    static inline const std::array<ObjectDetectorBackEnds, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_BACK_END_TYPES = {
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CPU,
    };

    static inline const std::array<ObjectDetectorBlobSizes, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_BLOB_SIZES = {
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FIVE,
    };

    static inline const std::array<std::string, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_OUTPUT_FILE_BASE_NAMES = {
        "no_yolov4",
        "yolov4-tiny_288_cuda",
        "yolov4-tiny_320_cuda",
        "yolov4-tiny_416_cuda",
        "yolov4-tiny_512_cuda",
        "yolov4-tiny_608_cuda",
        "yolov4_288_cuda",
        "yolov4_320_cuda",
        "yolov4_416_cuda",
        "yolov4_512_cuda",
        "yolov4_608_cuda",
        "yolov4-tiny_288",
        "yolov4-tiny_320",
        "yolov4-tiny_416",
        "yolov4-tiny_512",
        "yolov4-tiny_608",
        "yolov4_288",
        "yolov4_320",
        "yolov4_416",
        "yolov4_512",
        "yolov4_608",
    };
    ///@}
}
