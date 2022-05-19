/**
 * @file PerformanceTests.hpp
 * @author Junaid Afzal
 * @brief This class performs the performance tests by calling
 * the VideoManager class with different optional parameters.
 * The first test acts as a control by running it without an
 * object detector. Then the following three variables are tested:
 * YOLOv4 vs YOLOv4-tiny, blob  sizes, and CUDA vs CPU
 * @version 1.0
 * @date 14-04-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once
#include "pch.hpp"
#include "detectors/ObjectDetector.hpp"

class PerformanceTests
{
public:
    PerformanceTests(std::string InputVideoFilePath, std::string YoloResourcesFolderPath);

    ~PerformanceTests() = default;

    void Run();

private:
    std::vector<std::optional<std::vector<int>>> m_FrameTimes;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
    std::string m_InputVideoFilePath, m_YoloResourcesFolderPath;

    const int m_NUMBER_OF_TESTS = 21, m_NUMBER_OF_REPETITIONS = 5;

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
            Detector::STANDARD
    };

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
};
