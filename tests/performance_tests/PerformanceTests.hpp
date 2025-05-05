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
     * @class PerformanceTests
     * @brief Conducts performance tests across various detector settings against an input video benchmark file.
     */
    class PerformanceTests
    {
    public:
        /**
         * @brief Constructs a new %PerformanceTests object for use with a video file input (for API use).
         *
         * @param p_inputVideoFilePath The file path of the video file to use as the benchmark input.
         * @param p_yoloFolderPath The folder containing the `.cfg` and `.weights` YOLO files.
         * @param p_numberOfRepetitions The number of times to repeat each test.
         */
        explicit PerformanceTests(const std::string& p_currentPlatform, const std::string& p_inputVideoFilePath, const std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions);

        /**
         * @brief Constructs a new %PerformanceTests object (for CLI use).
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
         * -p --platform          The current platform being tested
         * -i --input             Benchmark video file path
         * -y --yolo-folder-path  Path to the yolo folder
         * -r --repetitions       Number of repetitions for each test
         *
         * @param p_commandLineArguments List of all command line arguments.
         */
        explicit PerformanceTests(const std::vector<std::string>& p_commandLineArguments);

        /**
         * @brief Runs the performance tests against the input benchmark video file.
         */
        void RunPerformanceTests();

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
            SQLiteDatabase() = default;

            /**
             * @brief Destructs a %SQLiteDatabase object.
             */
            ~SQLiteDatabase();

            //*@param p_databasePath The path to the SQLite database file.

            /**
             * @brief Opens the provided database file.
             *
             * @param p_databasePath The path to the SQLite database file.
             */
            void OpenDatabase(const std::string& p_databasePath);

            /**
             * @brief TODO
             *
             * @param p_currentPlatform TODO
             */
            void ClearPlatformFrameTimes(const std::string& p_currentPlatform);

            /**
             * @brief TODO
             *
             * @param p_currentPlatform TODO
             * @param p_objectDetectorTypes TODO
             * @param p_objectDetectorBackEnds TODO
             * @param p_objectDetectorBlobSize TODO
             * @param p_repetitionNumber TODO
             * @param p_frameTimes TODO
             * @param p_unit TODO
             */
            void InsertFrameTimes(const std::string& p_currentPlatform,
                                  const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                  const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                  const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSize,
                                  const uint32_t& p_repetitionNumber,
                                  const std::vector<uint32_t>& p_frameTimes,
                                  const std::string& p_unit);

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
         * @brief TODO
         */
        std::string m_currentPlatform;

        /**
         * @brief TODO
         */
        std::string m_databasePath;

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
