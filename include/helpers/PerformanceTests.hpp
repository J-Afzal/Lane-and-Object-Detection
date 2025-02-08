// NOLINTBEGIN

#pragma once

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "detectors/ObjectDetector.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief TODO
 */
namespace LaneAndObjectDetection
{
    /**
     * @class PerformanceTests
     * @brief TODO
     */
    class PerformanceTests
    {
    public:
        /**
         * @brief TODO
         */
        PerformanceTests(std::string p_inputVideoFilePath, std::string p_yoloResourcesFolderPath);

        /**
         * @brief TODO
         */
        ~PerformanceTests() = default;

        /**
         * @brief TODO
         */
        void Run();

    private:
        /**
         * @brief TODO
         */
        ///@{
        std::vector<std::optional<std::vector<uint32_t>>> m_frameTimes;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
        std::string m_inputVideoFilePath, m_yoloResourcesFolderPath;

        const uint32_t m_NUMBER_OF_TESTS = 21, m_NUMBER_OF_REPETITIONS = 5;

        const std::vector<Detector> m_YOLO_TYPES = {
            Detector::NONE,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::TINY,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD,
            Detector::STANDARD};

        const std::vector<BackEnd> m_BACK_END_TYPES = {
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CUDA,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
            BackEnd::CPU,
        };

        const std::vector<BlobSize> m_BLOB_SIZES = {
            BlobSize::ONE,
            BlobSize::ONE,
            BlobSize::TWO,
            BlobSize::THREE,
            BlobSize::FOUR,
            BlobSize::FIVE,
            BlobSize::ONE,
            BlobSize::TWO,
            BlobSize::THREE,
            BlobSize::FOUR,
            BlobSize::FIVE,
            BlobSize::ONE,
            BlobSize::TWO,
            BlobSize::THREE,
            BlobSize::FOUR,
            BlobSize::FIVE,
            BlobSize::ONE,
            BlobSize::TWO,
            BlobSize::THREE,
            BlobSize::FOUR,
            BlobSize::FIVE,
        };

        const std::vector<std::string> m_OUTPUT_FILE_NAMES = {
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
    };
}

// NOLINTEND
