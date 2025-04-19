#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <sqlite3.h>

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
        explicit PerformanceTest(std::string& p_inputVideoFilePath, std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions);

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

        /**
         * @brief TODO
         */
        void GeneratePerformanceGraphs();

    private:
        /**
         * @class SQLiteDatabase
         * @brief TODO (mention it is performance test specific hence why in private scope)
         */
        class SQLiteDatabase
        {
        public:
            /**
             * @brief Construct a new %SQLiteDatabase object.
             */
            SQLiteDatabase();

            /**
             * @brief Destructs a %SQLiteDatabase object.
             */
            ~SQLiteDatabase();

            /**
             * @brief TODO
             */
            void TruncateFrameTimesTable();

            /**
             * @brief TODO
             *
             * @param p_testName TODO
             * @param p_repetitionNumber TODO
             * @param p_frameTimes TODO
             */
            void InsertFrameTimes(const std::string& p_testName, const uint32_t& p_repetitionNumber, const std::vector<uint32_t>& p_frameTimes);

            /**
             * @brief TODO
             *
             * @param p_testName TODO
             * @return `std::vector<std::vector<uint32_t>>`
             */
            std::vector<std::vector<uint32_t>> GetFrameTimes(const std::string& p_testName);

        private:
            /**
             * @brief TODO
             *
             * @param p_sqlStatement TODO
             */
            void ExecuteSQLStatement(const std::string& p_sqlStatement);

            /**
             * @brief TODO
             *
             * @param p_sqlStatement TODO
             * @return `std::vector<std::unordered_map<std::string, std::string>>` TODO
             */
            std::vector<std::unordered_map<std::string, std::string>> ExecuteSelectSQLStatement(const std::string& p_sqlStatement);

            /**
             * @brief TODO
             *
             * @tparam T TODO
             * @param p_sqlStatementOutput TODO
             * @param p_columnName TODO
             * @return `std::vector<T>` TODO
             */
            template<typename T>
            std::vector<T> GetSQLStatementOutputColumnValues(const std::vector<std::unordered_map<std::string, std::string>>& p_sqlStatementOutput, const std::string& p_columnName);

            /**
             * @brief TODO
             *
             * @param p_resultCode TODO
             * @param p_errorMessage TODO
             * @exception SQLiteDatabaseError TODO
             */
            void CheckResultCode(const int32_t& p_resultCode);

            /**
             * @brief TODO (smart pointer?)
             */
            sqlite3* m_database;
        };

        /**
         * @brief TODO
         */
        SQLiteDatabase m_sqlLiteDatabase;

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
