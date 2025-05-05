#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "helpers/Globals.hpp"
#include "helpers/VideoManager.hpp"
#include "PerformanceTests.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    PerformanceTests::PerformanceTests(const std::string& p_currentPlatform, const std::string& p_inputVideoFilePath, const std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions) :
        m_currentPlatform(std::move(p_currentPlatform)),
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

        std::cout << "\nOpening the database path...";
        m_sqlLiteDatabase.OpenDatabase(m_databasePath);

        std::cout << "\nClearing current platform frame times from output table...";
        m_sqlLiteDatabase.ClearPlatformFrameTimes(m_currentPlatform);

        VideoManager videoManager;

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        for (uint32_t currentTestNumber = 0; currentTestNumber < Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS; currentTestNumber++)
        {
            std::cout << std::format("\n\n    ######## {} ({}/{}) ########",
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

                m_sqlLiteDatabase.InsertFrameTimes(m_currentPlatform,
                                                   Globals::G_PERFORMANCE_TESTS_OBJECT_DETECTOR_TYPES.at(currentTestNumber),
                                                   Globals::G_PERFORMANCE_TESTS_BACK_END_TYPES.at(currentTestNumber),
                                                   Globals::G_PERFORMANCE_TESTS_BLOB_SIZES.at(currentTestNumber),
                                                   currentRepetition,
                                                   videoManager.GetFrameTimes(),
                                                   "us"); // TODO: use GetUnit() function from performance class

                std::cout << std::format("\n        Finished repetition {}/{}", currentRepetition + 1, m_numberOfRepetitions);
            }
        }

        std::cout << std::format("\n\nTotal elapsed time = {} (H?:mm:ss)", Globals::GetTimeElapsed(START_TIME));
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n";

        // TODO: add output to run performance graphs python script to generate performance metrics
    }

    PerformanceTests::SQLiteDatabase::~SQLiteDatabase()
    {
        sqlite3_close(m_database);
    }

    void PerformanceTests::SQLiteDatabase::OpenDatabase(const std::string& p_databasePath)
    {
        int32_t resultCode = sqlite3_open(p_databasePath, &m_database);
        CheckResultCode(resultCode);

        const std::string CREATE_TABLE_SQL_STATEMENT = "CREATE TABLE IF NOT EXISTS FrameTimes"
                                                       "("
                                                       "    Id                      INTEGER PRIMARY KEY NOT NULL,"
                                                       "    Platform                TEXT                NOT NULL,"
                                                       "    YoloName                TEXT                NOT NULL,"
                                                       "    ObjectDetectorType      INTEGER             NOT NULL,"
                                                       "    ObjectDetectorBackEnd   INTEGER             NOT NULL,"
                                                       "    ObjectDetectorBlobSize  INTEGER             NOT NULL,"
                                                       "    Repetition              INTEGER             NOT NULL,"
                                                       "    FrameNumber             INTEGER             NOT NULL,"
                                                       "    FrameTime               INTEGER             NOT NULL,"
                                                       "    Unit                    TEXT                NOT NULL"
                                                       ");";
        ExecuteSQLStatement(CREATE_TABLE_SQL_STATEMENT);
    }

    void PerformanceTests::SQLiteDatabase::ClearPlatformFrameTimes(const std::string& p_currentPlatform)
    {
        ExecuteSQLStatement(std::format("DELETE FROM FrameTimes WHERE Platform = '{}'", p_currentPlatform));
    }

    void PerformanceTests::SQLiteDatabase::InsertFrameTimes(const std::string& p_currentPlatform,
                                                            const Globals::ObjectDetectorTypes& p_objectDetectorTypes,
                                                            const Globals::ObjectDetectorBackEnds& p_objectDetectorBackEnds,
                                                            const Globals::ObjectDetectorBlobSizes& p_objectDetectorBlobSize,
                                                            const uint32_t& p_repetitionNumber,
                                                            const std::vector<uint32_t>& p_frameTimes,
                                                            const std::string& p_unit)
    {
        for (uint32_t i = 0; i < p_frameTimes.size(); i++)
        {
            ExecuteSQLStatement(
                std::format(
                    "INSERT INTO"
                    "FrameTimes(Platform, YoloName, ObjectDetectorType, ObjectDetectorBackEnd, ObjectDetectorBlobSize, Repetition, FrameNumber, FrameTime, Unit)"
                    "VALUES({}, {}, {}, {}, {}, {}, {}, {}, {});",
                    p_currentPlatform,
                    "YOLOv7", // TODO: global?
                    p_objectDetectorTypes,
                    p_objectDetectorBackEnds,
                    p_objectDetectorBlobSize,
                    p_repetitionNumber,
                    i,
                    p_frameTimes[i],
                    p_unit));
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
