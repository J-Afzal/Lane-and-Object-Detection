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
         * @brief SQLite wrapper class. This class is in the private scope as the implementation is PerformanceTests-specific.
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

            /**
             * @brief Opens the provided database file.
             *
             * @param p_databasePath The path to the SQLite database file.
             */
            void OpenDatabase(const std::string& p_databasePath);

            /**
             * @brief Drops all rows for the specified platform.
             *
             * @param p_currentPlatform The platform name whose rows should be dropped.
             * @warning This function should only be called after `OpenDatabase()` has been successfully called.
             */
            void ClearPlatformFrameTimes(const std::string& p_currentPlatform);

            /**
             * @brief Inserts rows in to the FrameTimes table for every frame time in `p_frameTimes`.
             *
             * @param p_currentPlatform The platform the frame times are associated with.
             * @param p_objectDetectorTypes The type of object detector used.Globals
             * @param p_objectDetectorBackEnds The backend used by the object detector.
             * @param p_objectDetectorBlobSize The object detector blob size.
             * @param p_repetitionNumber The repetition number of the current test
             * @param p_frameTimes The frame times.
             * @param p_timeUnit The time unit that the frame times are measured in.
             * @param p_timeUnitConversion The divisor needed to convert the frame times to seconds.
             * @warning This function should only be called after `OpenDatabase()` has been successfully called.
             */
            void InsertFrameTimes(const std::string& p_currentPlatform,
                                  const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                  const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                  const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSize,
                                  const uint32_t& p_repetitionNumber,
                                  const std::vector<uint32_t>& p_frameTimes,
                                  const std::string& p_timeUnit,
                                  const uint32_t& p_timeUnitConversion);

        private:
            /**
             * @brief Executes `p_sqlStatement` against the SQLite database.
             *
             * @param p_sqlStatement The SQL statement to execute against the SQLite database.
             * @warning This function should only be called after `OpenDatabase()` has been successfully called.
             */
            void ExecuteSQLStatement(const std::string& p_sqlStatement);

            /**
             * @brief Checks that `p_resultCode` is `SQLITE_OK`. If not then raises SQLiteDatabaseError.
             *
             * @param p_resultCode The returned result code from the SQLite database.
             * @exception `SQLiteDatabaseError` Raised when `p_resultCode` is not `SQLITE_OK`.
             */
            void CheckResultCode(const int32_t& p_resultCode);

            /**
             * @brief SQLite database connection handle.
             */
            sqlite3* m_database;
        };

        /**
         * @brief The SQLite database which will store the measure frame times.
         */
        SQLiteDatabase m_sqlLiteDatabase;

        /**
         * @brief The platform that the performance tests will be ran against.
         */
        std::string m_currentPlatform;

        /**
         * @brief The path to the SQLite database file.
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
