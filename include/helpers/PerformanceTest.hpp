#include <cstdint>
#include <string>
#include <vector>

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    /**
     * @class PerformanceTest
     * @brief Conducts performance tests across various detector settings against an input video benchmark file.
     */
    class PerformanceTest
    {
    public:
        /**
         * @brief Constructs a new %PerformanceTest object for use with a video file input (for API use).
         *
         * @param p_inputVideoFilePath The file path of the video file to use as the benchmark input.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_numberOfRepetitions The number of times to repeat each test.
         */
        explicit PerformanceTest(const std::string& p_inputVideoFilePath, const std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions);

        /**
         * @brief Constructs a new %PerformanceTest object (for CLI use).
         *
         * Usage: lane-and-object-detection-performance-tests --input ... --yolo-folder-path ... --repetitions ...
         *
         * OPTIONS:
         *
         * Generic Options:
         *
         * -h --help              Display available options
         *
         * Required Options:
         *
         * -i --input             Benchmark video file path
         * -y --yolo-folder-path  Path to the yolo folder
         * -r --repetitions       Number of repetitions for each test
         *
         * @param p_commandLineArguments List of all command line arguments.
         */
        explicit PerformanceTest(const std::vector<std::string>& p_commandLineArguments);

        /**
         * @brief Runs the performance tests against the input benchmark video file.
         */
        void RunPerformanceTest();

    private:
        /**
         * @brief The file path of the video file to use as the benchmark input.
         */
        std::string m_inputVideoFilePath;

        /**
         * @brief The folder containing the `.cfg` and `.weights` YOLO files.
         */
        std::string m_yoloFolderPath;

        /**
         * @brief The number of times to repeat each test.
         */
        uint32_t m_numberOfRepetitions;
    };
}
