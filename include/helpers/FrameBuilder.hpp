#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @brief TODO
     */
    struct ObjectDetectionInformation
    {
        struct ObjectInformation
        {
            /**
             * @brief TODO
             */
            cv::Rect m_boundingBox;

            /**
             * @brief TODO
             */
            cv::Scalar m_boundingBoxColour;

            /**
             * @brief TODO
             */
            std::string m_objectName;
        };

        /**
         * @brief TODO
         */
        std::vector<ObjectInformation> m_objectInformation;
    };

    /**
     * @brief TODO
     */
    struct LaneDetectionInformation
    {
    };

    /**
     * @brief TODO
     */
    struct PerformanceInformation
    {
    };

    /**
     * @class TODO
     * @brief TODO
     */
    class FrameBuilder
    {
    public:
        /**
         * @brief Disable constructing a new %FrameBuilder object.
         */
        explicit FrameBuilder() = delete;

        /**
         * @brief TODO
         *
         * @param p_frame TODO
         * @param p_objectDetectionInformation TODO
         * @param p_laneDetectionInformation TODO
         * @param p_performanceInformation TODO
         */
        static void UpdateFrame(cv::Mat& p_frame,
                                const ObjectDetectionInformation& p_objectDetectionInformation,
                                const LaneDetectionInformation& p_laneDetectionInformation,
                                const PerformanceInformation& p_performanceInformation);

    private:
        /**
         * @brief TODO
         *
         * @param p_frame TODO
         * @param p_objectDetectionInformation TODO
         */
        static void AddObjectDetectionInformation(cv::Mat& p_frame, const ObjectDetectionInformation& p_objectDetectionInformation);

        /**
         * @brief TODO
         *
         * @param p_frame TODO
         * @param p_laneDetectionInformation TODO
         */
        static void AddLaneDetectionInformation(cv::Mat& p_frame, const LaneDetectionInformation& p_laneDetectionInformation);

        /**
         * @brief TODO
         *
         * @param p_frame TODO
         * @param p_performanceInformation TODO
         */
        static void AddPerformanceInformation(cv::Mat& p_frame, const PerformanceInformation& p_performanceInformation);
    };
}
