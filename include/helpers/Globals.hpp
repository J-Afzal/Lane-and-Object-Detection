#pragma once

#include <array>
#include <chrono>
#include <cmath>
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

        /**
         * @brief Used for SQLite errors.
         */
        class SQLiteDatabaseError : public std::exception
        {
        };
    }

    /**
     * @brief Gets the elapsed time from `p_startTime` to now.
     *
     * @param p_startTime The time to compare to now.
     * @return `std::string` The elapsed time in the format `mm:ss` if less than an hour or `H:mm:ss` if more than one hour.
     */
    static inline std::string GetTimeElapsed(const std::chrono::time_point<std::chrono::high_resolution_clock>& p_startTime)
    {
        const double MINUTES_IN_HOUR = 60;
        const double SECONDS_IN_HOUR = 3600;
        const double SECONDS_IN_MINUTE = 60;
        const uint32_t PADDING_THRESHOLD = 10;

        const uint32_t TOTAL_TIME_IN_SECONDS = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - p_startTime).count();

        const double TOTAL_HOURS = TOTAL_TIME_IN_SECONDS / SECONDS_IN_HOUR;
        const double TOTAL_MINUTES = (TOTAL_HOURS - std::floor(TOTAL_HOURS)) * MINUTES_IN_HOUR;
        const double TOTAL_SECONDS = (TOTAL_MINUTES - std::floor(TOTAL_MINUTES)) * SECONDS_IN_MINUTE;

        const uint32_t HOURS = std::floor(TOTAL_HOURS);
        const uint32_t MINUTES = std::floor(TOTAL_MINUTES);
        const uint32_t SECONDS = std::floor(TOTAL_SECONDS);

        std::string output;

        if (HOURS > 0)
        {
            output += std::to_string(HOURS);
            output += ":";
        }

        if (MINUTES < PADDING_THRESHOLD)
        {
            output += "0";
            output += std::to_string(MINUTES);
        }

        else
        {
            output += std::to_string(MINUTES);
        }

        output += ":";

        if (SECONDS < PADDING_THRESHOLD)
        {
            output += "0";
            output += std::to_string(SECONDS);
        }

        else
        {
            output += std::to_string(SECONDS);
        }

        return output;
    }

    /**
     * @brief CLI help message for the video manager.
     */
    static inline const std::string G_CLI_HELP_MESSAGE = "\nUsage: lane-and-object-detection --input ... --yolo-folder-path ... --object-detector-type ... [optional]\n\nOPTIONS:\n\nGeneric Options:\n\n  -h --help                       Display available options\n\nRequired Options:\n\n  -i --input                      File path or camera ID\n  -y --yolo-folder-path           Path to the yolo folder\n\nOptional options:\n\n  -o --object-detector-type       One of: none, standard or tiny (default = none)\n  -b --object-detector-backend    One of: cpu or cuda (default = cpu)\n  -s --object-detector-blob-size  One of: 208, 320, 416, 512 or 608 (default = 208)\n\n";

    /**
     * @brief Input video dimensions.
     */
    ///@{
    static inline const uint32_t G_VIDEO_INPUT_HEIGHT = 1080;
    static inline const uint32_t G_VIDEO_INPUT_WIDTH = 1920;
    ///@}

    /**
     * @brief Output video dimensions.
     */
    ///@{
    static inline const uint32_t G_VIDEO_OUTPUT_HEIGHT = 1080;
    static inline const uint32_t G_VIDEO_OUTPUT_WIDTH = 1920;
    ///@}

    /**
     * @brief Output video FPS
     */
    static inline const uint32_t G_VIDEO_OUTPUT_FPS = 30;

    /**
     * @brief Keyboard values when getting user input.
     */
    ///@{
    static inline const uint32_t G_KEY_DEBUG_MODE = 'd';
    static inline const uint32_t G_KEY_TOGGLE_SAVE_OUTPUT = 'r';
    static inline const uint32_t G_KEY_QUIT = 'q';
    ///@}

    /**
     * @brief Font settings.
     */
    ///@{
    static inline const uint32_t G_FONT_DEFAULT_FACE = cv::FONT_HERSHEY_DUPLEX;
    static inline const uint32_t G_FONT_DEFAULT_THICKNESS = 1;
    static inline const int32_t G_FONT_DEFAULT_HORIZONTAL_PADDING = 10;
    static inline const int32_t G_FONT_DEFAULT_VERTICAL_PADDING = 15;
    static inline const double G_FONT_DEFAULT_DECREMENT = 0.1;
    static inline const double G_FONT_DEFAULT_SCALE = 1;
    ///@}

    /**
     * @brief OpenCV Colours (in BGR format).
     */
    ///@{
    static inline const cv::Scalar G_COLOUR_BLACK = cv::Scalar(0, 0, 0);
    static inline const cv::Scalar G_COLOUR_GREY = cv::Scalar(128, 128, 128);
    static inline const cv::Scalar G_COLOUR_WHITE = cv::Scalar(255, 255, 255);
    static inline const cv::Scalar G_COLOUR_RED = cv::Scalar(0, 0, 192);
    static inline const cv::Scalar G_COLOUR_LIGHT_RED = cv::Scalar(128, 128, 192);
    static inline const cv::Scalar G_COLOUR_ORANGE = cv::Scalar(0, 128, 192);
    static inline const cv::Scalar G_COLOUR_YELLOW = cv::Scalar(0, 192, 192);
    static inline const cv::Scalar G_COLOUR_GREEN = cv::Scalar(0, 192, 0);
    static inline const cv::Scalar G_COLOUR_BLUE = cv::Scalar(192, 0, 0);
    static inline const cv::Scalar G_COLOUR_LIGHT_BLUE = cv::Scalar(192, 128, 128);
    static inline const cv::Scalar G_COLOUR_PURPLE = cv::Scalar(192, 0, 192);
    ///@}

    /**
     * @brief Conversion between time units.
     */
    static inline const double G_MICROSECONDS_IN_SECOND = 1000000;

    /**
     * @brief Convert a decimal value to a percentage.
     */
    static inline const uint32_t G_CONVERT_DECIMAL_TO_PERCENTAGE = 100;

    /**
     * @brief Divide by two.
     */
    static inline const double G_DIVIDE_BY_TWO = 2;

    /**
     * @brief Text to display whether or not recording.
     */
    ///@{
    static inline const std::string G_UI_TEXT_RECORDING = "Output Recording Enabled";
    static inline const std::string G_UI_TEXT_NOT_RECORDING = "Press 'r' to record output";
    ///@}

    /**
     * @brief Text to display whether or not in debug mode.
     */
    ///@{
    static inline const std::string G_UI_TEXT_DEBUG_MODE = "Debug Mode Enabled";
    static inline const std::string G_UI_TEXT_NOT_DEBUG_MODE = "Press 'd' to enter debug mode";
    ///@}

    /**
     * @brief Font scales for different heading sizes.
     */
    ///@{
    static inline const double G_UI_H1_FONT_SCALE = 0.6;
    static inline const double G_UI_H2_FONT_SCALE = 0.4;
    ///@}

    /**
     * @brief UI heights.
     */
    ///@{
    static inline const int32_t G_UI_BOTTOM_BAR_HEIGHT = 55;
    static inline const int32_t G_UI_TITLE_HEIGHT = 30;
    static inline const int32_t G_UI_SUBTITLE_HEIGHT = 25;
    ///@}

    /**
     * @brief Timestamp UI location.
     */
    static inline const cv::Rect G_UI_RECT_TIMESTAMP = cv::Rect(0, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, 300, G_UI_BOTTOM_BAR_HEIGHT);

    /**
     * @brief Performance-related information UI location.
     */
    static inline const cv::Rect G_UI_RECT_FPS = cv::Rect(300, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, 300, G_UI_BOTTOM_BAR_HEIGHT);

    /**
     * @brief Driving state UI location (width is the entire screen for centering).
     */
    static inline const cv::Rect G_UI_RECT_DRIVING_STATE = cv::Rect(0, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, G_VIDEO_INPUT_WIDTH, G_UI_TITLE_HEIGHT);

    /**
     * @brief Driving state sub-title UI location (width is the entire screen for centering).
     */
    static inline const cv::Rect G_UI_RECT_DRIVING_STATE_SUBTITLE = cv::Rect(0, G_VIDEO_INPUT_HEIGHT - G_UI_SUBTITLE_HEIGHT, G_VIDEO_INPUT_WIDTH, G_UI_SUBTITLE_HEIGHT);

    /**
     * @brief Debug mode status UI location
     */
    static inline const cv::Rect G_UI_RECT_DEBUG_MODE_STATUS = cv::Rect(G_VIDEO_INPUT_WIDTH - 600, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, 300, G_UI_BOTTOM_BAR_HEIGHT);

    /**
     * @brief Recording status UI locations.
     */
    ///@{
    static inline const cv::Rect G_UI_RECT_RECORDING_STATUS = cv::Rect(G_VIDEO_INPUT_WIDTH - 300, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, 300, G_UI_TITLE_HEIGHT);
    static inline const cv::Rect G_UI_RECT_RECORDING_ELAPSED_TIME = cv::Rect(G_VIDEO_INPUT_WIDTH - 300, G_VIDEO_INPUT_HEIGHT - G_UI_SUBTITLE_HEIGHT, 300, G_UI_SUBTITLE_HEIGHT);
    static inline const cv::Point G_UI_POINT_RECORDING_DOT = cv::Point(G_VIDEO_INPUT_WIDTH - 190, G_VIDEO_INPUT_HEIGHT - 13);
    static inline const uint32_t G_UI_RADIUS_RECORDING_DOT = 5;
    static inline const cv::Rect G_UI_RECT_NOT_RECORDING_STATUS = cv::Rect(G_VIDEO_INPUT_WIDTH - 300, G_VIDEO_INPUT_HEIGHT - G_UI_BOTTOM_BAR_HEIGHT, 300, G_UI_BOTTOM_BAR_HEIGHT);
    ///@}

    /**
     * @brief The thickness of the hough lines drawn in debug mode.
     */
    static inline const int32_t G_HOUGH_LINE_THICKNESS = 2;

    /**
     * @brief Translucent colour of the overlay for the current lane.
     */
    static inline const cv::Scalar G_LANE_OVERLAY_COLOUR = cv::Scalar(0, 64, 0);

    /**
     * @brief Default rolling average size.
     */
    static inline const uint32_t G_DEFAULT_ROLLING_AVERAGE_SIZE = 10;

    /**
     * @brief Region-of-interest dimensions.
     */
    ///@{
    static inline const int32_t G_ROI_TOP_HEIGHT = 660;
    static inline const int32_t G_ROI_BOTTOM_HEIGHT = 840;
    static inline const int32_t G_ROI_TOP_WIDTH = 200;
    static inline const int32_t G_ROI_BOTTOM_WIDTH = 900;
    ///@}

    /**
     * @brief Region-of-interest points. Below is how the indicies of the array map to the points.
     *
     *       Top left (0) >  ____________________  < Top right (1)
     *                      /                    \
     *                     /                      \
     *                    /                        \
     *                   /                          \
     *                  /                            \
     *                 /                              \
     *                 --------------------------------
     * Bottom Left (3) ^                               ^ Bottom right (2)
     */
    ///@{
    static inline const uint32_t G_NUMBER_OF_POINTS = 4;
    static inline const std::array<cv::Point, G_NUMBER_OF_POINTS> G_ROI_MASK_POINTS = {
        cv::Point((G_VIDEO_INPUT_WIDTH / 2.0) - (G_ROI_TOP_WIDTH / 2.0), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_INPUT_WIDTH / 2.0) + (G_ROI_TOP_WIDTH / 2.0), G_ROI_TOP_HEIGHT),
        cv::Point((G_VIDEO_INPUT_WIDTH / 2.0) + (G_ROI_BOTTOM_WIDTH / 2.0), G_ROI_BOTTOM_HEIGHT),
        cv::Point((G_VIDEO_INPUT_WIDTH / 2.0) - (G_ROI_BOTTOM_WIDTH / 2.0), G_ROI_BOTTOM_HEIGHT),
    };
    ///@}

    /**
     * @brief The bounding box of the region-of-interest which is used to crop debugging frames. The '#' characters show the
     * bounding box with respect to the region-of-interest.
     *
     *                #######____________________######## < Start Y
     *                #     /                    \      #
     *                #    /                      \     #
     *                #   /                        \    #
     *                #  /                          \   #
     *                # /                            \  #
     *                #/                              \ #
     *                #---------------------------------# < End Y
     *        Start X ^                           End X ^
     */
    ///@{
    static inline const uint32_t G_ROI_BOUND_BOX_PADDING = 10;
    static inline const uint32_t G_ROI_BOUNDING_BOX_START_X = (G_VIDEO_INPUT_WIDTH / 2.0) - (G_ROI_BOTTOM_WIDTH / 2.0) - G_ROI_BOUND_BOX_PADDING;
    static inline const uint32_t G_ROI_BOUNDING_BOX_END_X = (G_VIDEO_INPUT_WIDTH / 2.0) + (G_ROI_BOTTOM_WIDTH / 2.0) + G_ROI_BOUND_BOX_PADDING;
    static inline const uint32_t G_ROI_BOUNDING_BOX_START_Y = G_ROI_TOP_HEIGHT - G_ROI_BOUND_BOX_PADDING;
    static inline const uint32_t G_ROI_BOUNDING_BOX_END_Y = G_ROI_BOTTOM_HEIGHT + G_ROI_BOUND_BOX_PADDING;
    static inline const cv::Range G_ROI_BOUNDING_BOX_X_RANGE = cv::Range(G_ROI_BOUNDING_BOX_START_X, G_ROI_BOUNDING_BOX_END_X);
    static inline const cv::Range G_ROI_BOUNDING_BOX_Y_RANGE = cv::Range(G_ROI_BOUNDING_BOX_START_Y, G_ROI_BOUNDING_BOX_END_Y);
    ///@}

    /**
     * @brief Scaling factor for debugging frames.
     */
    static inline const double G_DEBUGGING_FRAME_SCALING_FACTOR = static_cast<double>(G_VIDEO_INPUT_WIDTH / 3.0) / static_cast<double>(G_ROI_BOUNDING_BOX_END_X - G_ROI_BOUNDING_BOX_START_X);

    /**
     * @brief The index values which represent each corner of `G_ROI_MASK_POINTS`.
     */
    ///@{
    static inline const uint32_t G_ROI_TOP_LEFT_INDEX = 0;
    static inline const uint32_t G_ROI_TOP_RIGHT_INDEX = 1;
    static inline const uint32_t G_ROI_BOTTOM_RIGHT_INDEX = 2;
    static inline const uint32_t G_ROI_BOTTOM_LEFT_INDEX = 3;
    ///@}

    /**
     * @brief Region-of-interest sub-division line equations (y = mx + c).
     *
     *                                   Top mid-point
     *                         __________|__________
     *                        /         # &         \
     *                       /         #   &         \
     * Left edge of mask >  /         #     &         \  < Right edge of mask
     *                     /         #       &         \
     *                    /         #         &         \
     *                   /         #           &         \
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
        NONE = 0,
        CPU,
        CUDA
    };

    /**
     * @brief The supported blob sizes for the object detector to run with. The larger the blob size the more performant the
     * detector at the cost of performance.
     */
    enum class ObjectDetectorBlobSizes : std::int16_t
    {
        NONE = 0,
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
     * @brief Object names and bounding box colours. `G_OPENCV_WHITE` is used as the default colour while custom colours are
     * given to object more commonly found while driving.
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
     * @brief CLI help message for the performance tests.
     */
    static inline const std::string G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE = "Usage: lane-and-object-detection-performance-tests --input ... --yolo-folder-path ... --repetitions ...\n\nOPTIONS:\n\nGeneric Options:\n\n-h --help              Display available options\n\nRequired Options:\n\n-p --platform          The current platform being tested\n-d --database-path     Path to SQLite database file\n-i --input             Benchmark video file path\n-y --yolo-folder-path  Path to the yolo folder\n-r --repetitions       Number of repetitions for each test";

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
        ObjectDetectorBackEnds::NONE,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
        ObjectDetectorBackEnds::CPU,
        ObjectDetectorBackEnds::CUDA,
    };

    static inline const std::array<ObjectDetectorBlobSizes, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_BLOB_SIZES = {
        ObjectDetectorBlobSizes::NONE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::ONE,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::TWO,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::THREE,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FOUR,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::FIVE,
        ObjectDetectorBlobSizes::FIVE,
    };

    static inline const std::array<std::string, G_PERFORMANCE_TESTS_NUMBER_OF_TESTS> G_PERFORMANCE_TESTS_NAMES = {
        "No YOLOv7",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7-tiny",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
        "YOLOv7",
    };
    ///@}
}
