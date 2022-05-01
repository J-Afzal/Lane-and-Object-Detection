/**
 * @file PerformanceTests.hpp
 * @author Junaid Afzal
 * @brief Performs the performance tests. First the source code is
 * ran without an object detector as a control. Then the following
 * three variables will be tested: YOLOv4 vs YOLOv4-tiny, blob
 * sizes, and CUDA vs CPU
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"
#include "helpers/Enumerations.hpp"

class PerformanceTests
{
public:
    explicit PerformanceTests(std::string FilePath);

    ~PerformanceTests() = default;

    void Run();

private:
    std::vector<std::optional<std::vector<int>>> m_FrameTimes;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
    std::string m_FilePath;

    const int m_NUMBER_OF_TESTS = 21, m_NUMBER_OF_REPETITIONS = 5;

    const std::vector<Enumerations::Detector> m_YOLO_TYPES = {
            Enumerations::Detector::NONE,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::TINY,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD,
            Enumerations::Detector::STANDARD
    };

    const std::vector<Enumerations::BackEnd> m_BACK_END_TYPES = {
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CUDA,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
            Enumerations::BackEnd::CPU,
    };

    const std::vector<Enumerations::BlobSize> m_BLOB_SIZES = {
            Enumerations::BlobSize::ONE,
            Enumerations::BlobSize::ONE,
            Enumerations::BlobSize::TWO,
            Enumerations::BlobSize::THREE,
            Enumerations::BlobSize::FOUR,
            Enumerations::BlobSize::FIVE,
            Enumerations::BlobSize::ONE,
            Enumerations::BlobSize::TWO,
            Enumerations::BlobSize::THREE,
            Enumerations::BlobSize::FOUR,
            Enumerations::BlobSize::FIVE,
            Enumerations::BlobSize::ONE,
            Enumerations::BlobSize::TWO,
            Enumerations::BlobSize::THREE,
            Enumerations::BlobSize::FOUR,
            Enumerations::BlobSize::FIVE,
            Enumerations::BlobSize::ONE,
            Enumerations::BlobSize::TWO,
            Enumerations::BlobSize::THREE,
            Enumerations::BlobSize::FOUR,
            Enumerations::BlobSize::FIVE,
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
};
