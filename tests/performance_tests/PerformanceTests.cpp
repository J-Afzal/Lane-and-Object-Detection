#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <sqlite3.h>

#include "helpers/Globals.hpp"
#include "helpers/VideoManager.hpp"

#include "PerformanceTests.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    PerformanceTests::PerformanceTests(std::string p_currentPlatform, std::string p_databasePath, std::string p_inputVideoFilePath, std::string p_yoloFolderPath, const uint32_t& p_numberOfRepetitions) :
        m_currentPlatform(std::move(p_currentPlatform)),
        m_databasePath(std::move(p_databasePath)),
        m_inputVideoFilePath(std::move(p_inputVideoFilePath)),
        m_yoloFolderPath(std::move(p_yoloFolderPath)),
        m_numberOfRepetitions(p_numberOfRepetitions)
    {}

    PerformanceTests::PerformanceTests(const std::vector<std::string>& p_commandLineArguments)
    {
        uint32_t index = 0;

        bool numberOfRepetitionsGiven = false;

        for (const std::string& argument : p_commandLineArguments)
        {
            if (argument == "-h" || argument == "--help")
            {
                std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            try
            {
                if (argument == "-p" || argument == "--platform")
                {
                    m_currentPlatform = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-d" || argument == "--database-path")
                {
                    m_databasePath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-i" || argument == "--input")
                {
                    m_inputVideoFilePath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-y" || argument == "--yolo-folder-path")
                {
                    m_yoloFolderPath = p_commandLineArguments.at(index + 1);
                }

                if (argument == "-r" || argument == "--repetitions")
                {
                    m_numberOfRepetitions = std::stoi(p_commandLineArguments.at(index + 1));
                    numberOfRepetitionsGiven = true;
                }
            }

            catch (...)
            {
                std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
                std::exit(1);
            }

            index++;
        }

        // Check that the required arguments have been provided
        if (m_currentPlatform.empty() || m_databasePath.empty() || m_inputVideoFilePath.empty() || m_yoloFolderPath.empty() || !numberOfRepetitionsGiven)
        {
            std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
            std::exit(1);
        }
    }

    void PerformanceTests::RunPerformanceTests()
    {
        std::cout << "\n################ Lane and Object Detection Performance Tests ################";
        std::cout << "\nSettings:";
        std::cout << "\n    Number of tests: " << Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS;
        std::cout << "\n    Number of repetitions: " << m_numberOfRepetitions;

        std::cout << "\nOpening the database...";
        m_sqliteDatabase.OpenDatabase(m_databasePath);

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        VideoManager videoManager;

        for (uint32_t currentTestNumber = 0; currentTestNumber < Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS; currentTestNumber++)
        {
            std::cout << std::format("\n\n    ######## {} ({}/{}) ########\n",
                                     Globals::G_PERFORMANCE_TESTS_NAMES.at(currentTestNumber),
                                     currentTestNumber,
                                     Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS);

            for (uint32_t currentRepetition = 0; currentRepetition < m_numberOfRepetitions; currentRepetition++)
            {
                videoManager.SetProperties(m_inputVideoFilePath,
                                           m_yoloFolderPath,
                                           Globals::G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES.at(currentTestNumber),
                                           Globals::G_PERFORMANCE_TESTS_BACK_END_TYPES.at(currentTestNumber),
                                           Globals::G_PERFORMANCE_TESTS_BLOB_SIZES.at(currentTestNumber));

                videoManager.RunLaneAndObjectDetector();

                m_sqliteDatabase.InsertFrameTimes(m_currentPlatform,
                                                  Globals::G_YOLO_NAME,
                                                  Globals::G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES.at(currentTestNumber),
                                                  Globals::G_PERFORMANCE_TESTS_BACK_END_TYPES.at(currentTestNumber),
                                                  Globals::G_PERFORMANCE_TESTS_BLOB_SIZES.at(currentTestNumber),
                                                  currentRepetition,
                                                  videoManager.GetPerformance().GetFrameTimes(),
                                                  Performance::GetTimeUnit(),
                                                  Performance::GetTimeUnitConversion());

                std::cout << std::format("\n        Finished repetition {}/{}", currentRepetition + 1, m_numberOfRepetitions);
            }
        }

        std::cout << std::format("\n\nTotal elapsed time = {} (HH:mm:ss)", Globals::GetTimeElapsed(START_TIME));
        std::cout << std::format("\n\nRun './tests/main.py -p {} -o ./tests/performance_graphs/' to generate the performance graphs", m_databasePath);
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n";
    }

    PerformanceTests::SQLiteDatabase::~SQLiteDatabase()
    {
        sqlite3_close(m_database);
    }

    void PerformanceTests::SQLiteDatabase::OpenDatabase(const std::string& p_databasePath)
    {
        CheckResultCode(sqlite3_open(p_databasePath.c_str(), &m_database));

        const std::string DROP_TABLE_SQL_STATEMENT = "DROP TABLE IF EXISTS FrameTimes;";

        const std::string CREATE_TABLE_SQL_STATEMENT = "CREATE TABLE IF NOT EXISTS FrameTimes"
                                                       "("
                                                       "    Id                      INTEGER PRIMARY KEY NOT NULL,"
                                                       "    PlatformName            TEXT                NOT NULL,"
                                                       "    YoloName                TEXT                NOT NULL,"
                                                       "    ObjectDetectorType      INTEGER             NOT NULL,"
                                                       "    ObjectDetectorBackEnd   INTEGER             NOT NULL,"
                                                       "    ObjectDetectorBlobSize  INTEGER             NOT NULL,"
                                                       "    Repetition              INTEGER             NOT NULL,"
                                                       "    FrameNumber             INTEGER             NOT NULL,"
                                                       "    FrameTime               INTEGER             NOT NULL,"
                                                       "    TimeUnit                TEXT                NOT NULL,"
                                                       "    TimeUnitConversion      INTEGER             NOT NULL"
                                                       ");";

        ExecuteSQLStatement(DROP_TABLE_SQL_STATEMENT);

        ExecuteSQLStatement(CREATE_TABLE_SQL_STATEMENT);
    }

    void PerformanceTests::SQLiteDatabase::InsertFrameTimes(const std::string& p_currentPlatform,
                                                            const std::string& p_currentYoloName,
                                                            const Globals::ObjectDetectorTypes& p_objectDetectorType,
                                                            const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnd,
                                                            const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSize,
                                                            const uint32_t& p_repetitionNumber,
                                                            const std::vector<uint32_t>& p_frameTimes,
                                                            const std::string& p_timeUnit,
                                                            const uint32_t& p_timeUnitConversion)
    {
        for (uint32_t i = 0; i < p_frameTimes.size(); i++)
        {
            ExecuteSQLStatement(
                std::format(
                    "INSERT INTO "
                    "FrameTimes(PlatformName, YoloName, ObjectDetectorType, ObjectDetectorBackEnd, ObjectDetectorBlobSize, Repetition, FrameNumber, FrameTime, TimeUnit, TimeUnitConversion) "
                    "VALUES('{}', '{}', {}, {}, {}, {}, {}, {}, '{}', {});",
                    p_currentPlatform,
                    p_currentYoloName,
                    static_cast<uint8_t>(p_objectDetectorType),
                    static_cast<uint8_t>(p_objectDetectorBackEnd),
                    static_cast<uint16_t>(p_objectDetectorBlobSize),
                    p_repetitionNumber,
                    i,
                    p_frameTimes[i],
                    p_timeUnit,
                    p_timeUnitConversion));
        }
    }

    void PerformanceTests::SQLiteDatabase::ExecuteSQLStatement(const std::string& p_sqlStatement)
    {
        const int32_t RESULT_CODE = sqlite3_exec(m_database, p_sqlStatement.c_str(), nullptr, nullptr, nullptr);
        CheckResultCode(RESULT_CODE);
    }

    void PerformanceTests::SQLiteDatabase::CheckResultCode(const int32_t& p_resultCode)
    {
        if (p_resultCode == SQLITE_OK)
        {
            return;
        }

        std::cout << "\nERROR: SQLite operation outputted following error message: " + std::string(sqlite3_errmsg(m_database));

        throw Globals::Exceptions::SQLiteDatabaseError();
    }
}
