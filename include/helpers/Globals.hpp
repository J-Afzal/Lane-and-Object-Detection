#pragma once

#include <array>
#include <cstdint>
#include <exception>
#include <map>
#include <string>
#include <unordered_map>

#include <opencv2/core/cvdef.h>
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

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
    static inline const std::string G_CLI_HELP_MESSAGE = "\nUsage: lane-and-object-detection --input ... --yolo-folder-path ... --object-detector-type ... [optional]\n\nOPTIONS:\n\nGeneric Options:\n\n  -h --help                       Display available options\n\nRequired Options:\n\n  -i --input                      File path or camera ID\n  -y --yolo-folder-path           Path to the yolo folder\n\nOptional options:\n\n  -o --object-detector-type       One of: none, standard or tiny (default = none)\n  -b --object-detector-backend    One of: cpu or cuda (default = cpu)\n  -s --object-detector-blob-size  One of: 208, 320, 416, 512 or 608 (default = 208)\n\n";

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
    static inline const int32_t G_DEFAULT_HORIZONTAL_PADDING = 10;
    static inline const int32_t G_DEFAULT_VERTICAL_PADDING = 15;
    static inline const double G_DEFAULT_FONT_DECREMENT = 0.1;
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
     * @brief Conversion between time units.
     */
    ///@{
    static inline const double G_MINUTES_IN_HOUR = 60;
    static inline const double G_SECONDS_IN_HOUR = 3600;
    static inline const double G_SECONDS_IN_MINUTE = 60;
    static inline const double G_MILLISECONDS_IN_SECOND = 1000;
    ///@}

    /**
     * @brief Convert a decimal value to a percentage.
     */
    static inline const uint32_t G_CONVERT_DECIMAL_TO_PERCENTAGE = 100;

    /**
     * @brief Divide by two.
     */
    static inline const double G_DIVIDE_BY_TWO = 2;

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
    static inline const int32_t G_UI_PADDING = 30;
    static inline const int32_t G_UI_TITLE_HEIGHT = 50;
    static inline const int32_t G_UI_SUBTITLE_HEIGHT = 30;
    ///@}

    /**
     * @brief Lane detector-related information UI widths.
     */
    ///@{
    static inline const int32_t G_UI_DRIVING_STATE_WIDTH = 500;
    static inline const int32_t G_UI_TURNING_STATE_WIDTH = 300;
    static inline const int32_t G_UI_LANE_INFORMATION_WIDTH = 200;
    ///@}

    /**
     * @brief Lane detector-related information UI locations.
     */
    ///@{
    static inline const cv::Rect G_UI_DRIVING_STATE_RECT = cv::Rect((Globals::G_INPUT_VIDEO_WIDTH / 2.0) - (G_UI_DRIVING_STATE_WIDTH / 2.0), G_UI_PADDING, G_UI_DRIVING_STATE_WIDTH, G_UI_TITLE_HEIGHT);
    static inline const cv::Rect G_UI_TURNING_STATE_RECT = cv::Rect((Globals::G_INPUT_VIDEO_WIDTH / 2.0) - (G_UI_TURNING_STATE_WIDTH / 2.0), G_UI_DRIVING_STATE_RECT.y + G_UI_DRIVING_STATE_RECT.height + G_UI_PADDING, G_UI_TURNING_STATE_WIDTH, G_UI_SUBTITLE_HEIGHT);
    static inline const cv::Rect G_UI_LANE_INFORMATION_TITLE_RECT = cv::Rect(G_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_UI_LANE_INFORMATION_WIDTH, G_UI_PADDING, G_UI_LANE_INFORMATION_WIDTH, G_UI_SUBTITLE_HEIGHT);
    static inline const cv::Rect G_UI_LANE_INFORMATION_RECT = cv::Rect(G_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_UI_LANE_INFORMATION_WIDTH, G_UI_LANE_INFORMATION_TITLE_RECT.y + G_UI_LANE_INFORMATION_TITLE_RECT.height + G_UI_PADDING, G_UI_LANE_INFORMATION_WIDTH, 200);
    static inline const cv::Rect G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT = cv::Rect(G_INPUT_VIDEO_WIDTH - G_UI_PADDING - G_UI_LANE_INFORMATION_WIDTH, G_UI_LANE_INFORMATION_RECT.y + G_UI_LANE_INFORMATION_RECT.height + G_UI_PADDING, G_UI_LANE_INFORMATION_WIDTH, G_UI_SUBTITLE_HEIGHT);
    ///@}

    /**
     * @brief Recording status UI location.
     */
    static inline const cv::Rect G_RECORDING_STATUS_RECT = cv::Rect(G_INPUT_VIDEO_WIDTH - G_UI_LANE_INFORMATION_WIDTH - G_UI_PADDING, G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT.y + G_UI_LANE_INFORMATION_TURNING_REQUIRED_RECT.height + G_UI_PADDING, G_UI_LANE_INFORMATION_WIDTH, G_UI_SUBTITLE_HEIGHT);

    /**
     * @brief Lane information-related lane state UI properties.
     */
    ///@{
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_PAD = 30;
    static inline const int32_t G_LANE_INFORMATION_LEFT_LANE_STATE_X_LOCATION = G_UI_LANE_INFORMATION_RECT.x + G_LANE_INFORMATION_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_MIDDLE_LANE_STATE_X_LOCATION = G_UI_LANE_INFORMATION_RECT.x + static_cast<int32_t>(G_UI_LANE_INFORMATION_RECT.width / 2.0);
    static inline const int32_t G_LANE_INFORMATION_RIGHT_LANE_STATE_X_LOCATION = G_UI_LANE_INFORMATION_RECT.x + G_UI_LANE_INFORMATION_RECT.width - G_LANE_INFORMATION_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_Y_START_LOCATION = G_UI_LANE_INFORMATION_RECT.y + G_LANE_INFORMATION_LANE_STATE_PAD;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_WIDTH = 3;
    static inline const int32_t G_LANE_INFORMATION_LANE_STATE_HEIGHT = 15;
    ///@}

    /**
     * @brief Lane information-related vehicle position UI properties.
     */
    ///@{
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_WIDTH = G_UI_LANE_INFORMATION_RECT.width - (4 * G_UI_PADDING);
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_HEIGHT = G_UI_LANE_INFORMATION_RECT.height - (3 * G_UI_PADDING);
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_X_MIDDLE_LOCATION = G_UI_LANE_INFORMATION_RECT.x + static_cast<int32_t>((G_UI_LANE_INFORMATION_RECT.width / 2.0) - (G_LANE_INFORMATION_VEHICLE_POSITION_WIDTH / 2.0));
    static inline const int32_t G_LANE_INFORMATION_VEHICLE_POSITION_Y_LOCATION = G_UI_LANE_INFORMATION_RECT.y + static_cast<int32_t>(1.5 * G_UI_PADDING);
    ///@}

    /**
     * @brief Translucent colour of the overlay for the current lane.
     */
    static inline const cv::Scalar G_LANE_INFORMATION_LANE_OVERLAY_COLOUR = cv::Scalar(0, 64, 0);

    /**
     * @brief Default rolling average size.
     */
    static inline const uint32_t G_DEFAULT_ROLLING_AVERAGE_SIZE = 10;

    /**
     * @brief The number of lane lines to display to the frame.
     */
    static inline const uint32_t G_NUMBER_OF_LANE_LINES_TO_DISPLAY = 5;

    /**
     * @brief Region of interest dimensions.
     */
    ///@{
    static inline const uint32_t G_ROI_TOP_HEIGHT = 660;
    static inline const uint32_t G_ROI_BOTTOM_HEIGHT = 840;
    static inline const uint32_t G_ROI_TOP_WIDTH = 200;
    static inline const uint32_t G_ROI_BOTTOM_WIDTH = 900;
    static inline const uint32_t G_NUMBER_OF_POINTS = 4;
    ///@}

    /**
     * @brief Region of interest points. Below is how the indicies of the array map to the points.
     *
     *       Top left (0) >  ____________________  < Top right (1)
     *                      /         / \        \
     *                     /         /   \        \
     *                    /         /     \        \
     *                   /         /       \        \
     *                  /         /         \        \
     *                 /         /           \        \
     *                 ---------- ----------- ----------
     * Bottom Left (3) ^                               ^ Bottom right (2)
     */
    static inline const std::array<cv::Point, G_NUMBER_OF_POINTS> G_ROI_MASK_POINTS = {
        cv::Point((G_INPUT_VIDEO_WIDTH / 2) - (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_INPUT_VIDEO_WIDTH / 2) + (G_ROI_TOP_WIDTH / 2), G_ROI_TOP_HEIGHT),
        cv::Point((G_INPUT_VIDEO_WIDTH / 2) + (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
        cv::Point((G_INPUT_VIDEO_WIDTH / 2) - (G_ROI_BOTTOM_WIDTH / 2), G_ROI_BOTTOM_HEIGHT),
    };

    /**
     * @brief The index values which represent each corner of G_ROI_MASK_POINTS.
     */
    ///@{
    static inline const uint32_t G_ROI_TOP_LEFT_INDEX = 0;
    static inline const uint32_t G_ROI_TOP_RIGHT_INDEX = 1;
    static inline const uint32_t G_ROI_BOTTOM_RIGHT_INDEX = 2;
    static inline const uint32_t G_ROI_BOTTOM_LEFT_INDEX = 3;
    ///@}

    /**
     * @brief Region of interest sub-division line equations (y = mx + c).
     *
     *                                   Top mid-point
     *                         __________|__________
     *                        /         # &        \
     *                       /         #   &        \
     * Left edge of mask >  /         #     &        \  < Right edge of mask
     *                     /         #       &        \
     *                    /         #         &        \
     *                   /         #           &        \
     *                   ---------- ----------- ----------
     *           Bottom one third ^             ^ Bottom two thirds
     *
     * Left edge threshold = '#'
     * Right edge threshold = '@'
     */
    ///@{
    static inline const double G_LEFT_EDGE_OF_MASK_M = (G_ROI_MASK_POINTS[G_ROI_TOP_LEFT_INDEX].y - G_ROI_MASK_POINTS[G_ROI_BOTTOM_LEFT_INDEX].y) / static_cast<double>(G_ROI_MASK_POINTS[G_ROI_TOP_LEFT_INDEX].x - G_ROI_MASK_POINTS[G_ROI_BOTTOM_LEFT_INDEX].x);
    static inline const double G_LEFT_EDGE_OF_MASK_C = G_ROI_MASK_POINTS[G_ROI_TOP_LEFT_INDEX].y - (G_LEFT_EDGE_OF_MASK_M * G_ROI_MASK_POINTS[G_ROI_TOP_LEFT_INDEX].x);

    static inline const double G_RIGHT_EDGE_OF_MASK_M = (G_ROI_MASK_POINTS[G_ROI_TOP_RIGHT_INDEX].y - G_ROI_MASK_POINTS[G_ROI_BOTTOM_RIGHT_INDEX].y) / static_cast<double>(G_ROI_MASK_POINTS[G_ROI_TOP_RIGHT_INDEX].x - G_ROI_MASK_POINTS[G_ROI_BOTTOM_RIGHT_INDEX].x);
    static inline const double G_RIGHT_EDGE_OF_MASK_C = G_ROI_MASK_POINTS[G_ROI_TOP_RIGHT_INDEX].y - (G_RIGHT_EDGE_OF_MASK_M * G_ROI_MASK_POINTS[G_ROI_TOP_RIGHT_INDEX].x);

    static inline const double G_TOP_MID_POINT_X_LOCATION = G_ROI_MASK_POINTS[G_ROI_TOP_LEFT_INDEX].x + (G_ROI_TOP_WIDTH / 2.0);
    static inline const double G_BOTTOM_ONE_THIRD_LOCATION = G_ROI_MASK_POINTS[G_ROI_BOTTOM_LEFT_INDEX].x + (G_ROI_BOTTOM_WIDTH / 3.0);
    static inline const double G_BOTTOM_TWO_THIRD_LOCATION = G_ROI_MASK_POINTS[G_ROI_BOTTOM_LEFT_INDEX].x + (2.0 * G_ROI_BOTTOM_WIDTH / 3.0);

    static inline const double G_LEFT_LINE_THRESHOLD_M = (G_ROI_TOP_HEIGHT - G_ROI_BOTTOM_HEIGHT) / (G_TOP_MID_POINT_X_LOCATION - G_BOTTOM_ONE_THIRD_LOCATION);
    static inline const double G_LEFT_LINE_THRESHOLD_C = G_ROI_TOP_HEIGHT - (G_LEFT_LINE_THRESHOLD_M * G_TOP_MID_POINT_X_LOCATION);

    static inline const double G_RIGHT_LINE_THRESHOLD_M = (G_ROI_TOP_HEIGHT - G_ROI_BOTTOM_HEIGHT) / (G_TOP_MID_POINT_X_LOCATION - G_BOTTOM_TWO_THIRD_LOCATION);
    static inline const double G_RIGHT_LINE_THRESHOLD_C = G_ROI_TOP_HEIGHT - (G_RIGHT_LINE_THRESHOLD_M * G_TOP_MID_POINT_X_LOCATION);
    ///@}

    /**
     * @brief Canny algorithm thresholds.
     */
    ///@{
    static inline const uint32_t G_CANNY_ALGORITHM_LOWER_THRESHOLD = 128;
    static inline const uint32_t G_CANNY_ALGORITHM_UPPER_THRESHOLD = 255;
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
     * @brief Hough line index mapping.
     */
    ///@{
    static inline const uint32_t G_VEC4_X1_INDEX = 0;
    static inline const uint32_t G_VEC4_Y1_INDEX = 1;
    static inline const uint32_t G_VEC4_X2_INDEX = 2;
    static inline const uint32_t G_VEC4_Y2_INDEX = 3;
    ///@}

    /**
     * @brief Threshold gradient to decide whether a line is to be considered horizontal.
     */
    static inline const double G_HOUGH_LINE_HORIZONTAL_GRADIENT_THRESHOLD = 0.15;

    /**
     * @brief Threshold length to decide whether a line is to be considered solid line road marking.
     */
    static inline const uint32_t G_SOLID_LINE_LENGTH_THRESHOLD = 75;

    /**
     * @brief The type of lane line.
     */
    enum class LaneLineType : std::uint8_t
    {
        EMPTY = 0,
        DASHED,
        SOLID
    };

    /**
     * @brief The different driving states supported by the lane detector.
     */
    enum class DrivingState : std::uint8_t
    {
        WITHIN_LANE = 0,
        CHANGING_LANES,
        ONLY_LEFT_LANE_MARKING_DETECTED,
        ONLY_RIGHT_LANE_MARKING_DETECTED,
        NO_LANE_MARKINGS_DETECTED
    };

    /**
     * @brief The driving state titles to display.
     */
    static inline const std::unordered_map<DrivingState, std::string> G_DRIVING_STATE_TITLES = {
        {DrivingState::WITHIN_LANE,                      "Within Lanes"                             },
        {DrivingState::CHANGING_LANES,                   "WARNING: Changing lanes"                  },
        {DrivingState::ONLY_LEFT_LANE_MARKING_DETECTED,  "WARNING: Only left road marking detected" },
        {DrivingState::ONLY_RIGHT_LANE_MARKING_DETECTED, "WARNING: Only right road marking detected"},
        {DrivingState::NO_LANE_MARKINGS_DETECTED,        "WARNING: No road markings detected"       },
    };

    /**
     * @brief Text to display whether recording or not recording.
     */
    ///@{
    static inline const std::string G_LANE_INFORMATION_TITLE_LANE_DETECTED = "Detected Lanes";
    static inline const std::string G_LANE_INFORMATION_TITLE_LANE_NOT_DETECTED = "No Lanes Detected";
    ///@}

    /**
     * @brief The clamp parameters for determining the distance difference when the vehicle is within a lane.
     */
    ///@{
    static inline const double G_WITHIN_LANE_MINIMUM_CLAMP_DIFFERENCE_DISTANCE = -100;
    static inline const double G_WITHIN_LANE_MAXIMUM_CLAMP_DIFFERENCE_DISTANCE = 100;
    ///@}

    /**
     * @brief The nearest percentage amount to round the turning required down to.
     */
    static inline const uint32_t G_WITHIN_LANE_TURNING_REQUIRED_ROUNDING = 10;

    /**
     * @brief The number of frames to wait before calculating another distance difference while the vehicle is changing lanes.
     */
    static inline const uint32_t G_CHANGING_LANES_DISTANCE_DIFFERENCE_FRAME_COUNT_THRESHOLD = 10;

    /**
     * @brief The type of object detector to use with an option to disable object detection. The tiny version is more performant
     * at the cost of accuracy.
     */
    enum class ObjectDetectorTypes : std::uint8_t
    {
        NONE = 0,
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
        {"aeroplane",      G_COLOUR_WHITE },
        {"apple",          G_COLOUR_WHITE },
        {"backpack",       G_COLOUR_WHITE },
        {"banana",         G_COLOUR_WHITE },
        {"baseball bat",   G_COLOUR_WHITE },
        {"baseball glove", G_COLOUR_WHITE },
        {"bear",           G_COLOUR_WHITE },
        {"bed",            G_COLOUR_WHITE },
        {"bench",          G_COLOUR_WHITE },
        {"bicycle",        G_COLOUR_ORANGE},
        {"bird",           G_COLOUR_WHITE },
        {"boat",           G_COLOUR_WHITE },
        {"book",           G_COLOUR_WHITE },
        {"bottle",         G_COLOUR_WHITE },
        {"bowl",           G_COLOUR_WHITE },
        {"broccoli",       G_COLOUR_WHITE },
        {"bus",            G_COLOUR_GREY  },
        {"cake",           G_COLOUR_WHITE },
        {"car",            G_COLOUR_YELLOW},
        {"carrot",         G_COLOUR_WHITE },
        {"cat",            G_COLOUR_WHITE },
        {"cell phone",     G_COLOUR_WHITE },
        {"chair",          G_COLOUR_WHITE },
        {"clock",          G_COLOUR_WHITE },
        {"cow",            G_COLOUR_WHITE },
        {"cup",            G_COLOUR_WHITE },
        {"diningtable",    G_COLOUR_WHITE },
        {"dog",            G_COLOUR_WHITE },
        {"donut",          G_COLOUR_WHITE },
        {"elephant",       G_COLOUR_WHITE },
        {"fire hydrant",   G_COLOUR_WHITE },
        {"fork",           G_COLOUR_WHITE },
        {"frisbee",        G_COLOUR_WHITE },
        {"giraffe",        G_COLOUR_WHITE },
        {"hair drier",     G_COLOUR_WHITE },
        {"handbag",        G_COLOUR_WHITE },
        {"horse",          G_COLOUR_WHITE },
        {"hot dog",        G_COLOUR_WHITE },
        {"keyboard",       G_COLOUR_WHITE },
        {"kite",           G_COLOUR_WHITE },
        {"knife",          G_COLOUR_WHITE },
        {"laptop",         G_COLOUR_WHITE },
        {"microwave",      G_COLOUR_WHITE },
        {"motorbike",      G_COLOUR_GREEN },
        {"mouse",          G_COLOUR_WHITE },
        {"orange",         G_COLOUR_WHITE },
        {"oven",           G_COLOUR_WHITE },
        {"parking meter",  G_COLOUR_WHITE },
        {"person",         G_COLOUR_BLUE  },
        {"pizza",          G_COLOUR_WHITE },
        {"pottedplant",    G_COLOUR_WHITE },
        {"refrigerator",   G_COLOUR_WHITE },
        {"remote",         G_COLOUR_WHITE },
        {"sandwich",       G_COLOUR_WHITE },
        {"scissors",       G_COLOUR_WHITE },
        {"sheep",          G_COLOUR_WHITE },
        {"sink",           G_COLOUR_WHITE },
        {"skateboard",     G_COLOUR_WHITE },
        {"skis",           G_COLOUR_WHITE },
        {"snowboard",      G_COLOUR_WHITE },
        {"sofa",           G_COLOUR_WHITE },
        {"spoon",          G_COLOUR_WHITE },
        {"sports ball",    G_COLOUR_WHITE },
        {"stop sign",      G_COLOUR_WHITE },
        {"suitcase",       G_COLOUR_WHITE },
        {"surfboard",      G_COLOUR_WHITE },
        {"teddy bear",     G_COLOUR_WHITE },
        {"tennis racket",  G_COLOUR_WHITE },
        {"tie",            G_COLOUR_WHITE },
        {"toaster",        G_COLOUR_WHITE },
        {"toilet",         G_COLOUR_WHITE },
        {"toothbrush",     G_COLOUR_WHITE },
        {"traffic light",  G_COLOUR_RED   },
        {"train",          G_COLOUR_WHITE },
        {"truck",          G_COLOUR_PURPLE},
        {"tvmonitor",      G_COLOUR_WHITE },
        {"umbrella",       G_COLOUR_WHITE },
        {"vase",           G_COLOUR_WHITE },
        {"wine glass",     G_COLOUR_WHITE },
        {"zebra",          G_COLOUR_WHITE },
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

    static inline const std::array<Globals::ObjectDetectorTypes, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES = {
        Globals::ObjectDetectorTypes::NONE,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::TINY,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD,
        Globals::ObjectDetectorTypes::STANDARD};

    static inline const std::array<Globals::ObjectDetectorBackEnds, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_BACK_END_TYPES = {
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CUDA,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
        Globals::ObjectDetectorBackEnds::CPU,
    };

    static inline const std::array<Globals::ObjectDetectorBlobSizes, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_BLOB_SIZES = {
        Globals::ObjectDetectorBlobSizes::ONE,
        Globals::ObjectDetectorBlobSizes::ONE,
        Globals::ObjectDetectorBlobSizes::TWO,
        Globals::ObjectDetectorBlobSizes::THREE,
        Globals::ObjectDetectorBlobSizes::FOUR,
        Globals::ObjectDetectorBlobSizes::FIVE,
        Globals::ObjectDetectorBlobSizes::ONE,
        Globals::ObjectDetectorBlobSizes::TWO,
        Globals::ObjectDetectorBlobSizes::THREE,
        Globals::ObjectDetectorBlobSizes::FOUR,
        Globals::ObjectDetectorBlobSizes::FIVE,
        Globals::ObjectDetectorBlobSizes::ONE,
        Globals::ObjectDetectorBlobSizes::TWO,
        Globals::ObjectDetectorBlobSizes::THREE,
        Globals::ObjectDetectorBlobSizes::FOUR,
        Globals::ObjectDetectorBlobSizes::FIVE,
        Globals::ObjectDetectorBlobSizes::ONE,
        Globals::ObjectDetectorBlobSizes::TWO,
        Globals::ObjectDetectorBlobSizes::THREE,
        Globals::ObjectDetectorBlobSizes::FOUR,
        Globals::ObjectDetectorBlobSizes::FIVE,
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
