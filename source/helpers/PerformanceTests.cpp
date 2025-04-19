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
#include "helpers/PerformanceTest.hpp"
#include "helpers/VideoManager.hpp"

/**
 * @namespace LaneAndObjectDetection
 * @brief Contains all Lane-and-Object-Detection objects.
 */
namespace LaneAndObjectDetection
{
    PerformanceTest::PerformanceTest(std::string& p_inputVideoFilePath, std::string& p_yoloFolderPath, const uint32_t& p_numberOfRepetitions) :
        m_inputVideoFilePath(std::move(p_inputVideoFilePath)),
        m_yoloFolderPath(std::move(p_yoloFolderPath)),
        m_numberOfRepetitions(p_numberOfRepetitions)
    {}

    PerformanceTest::PerformanceTest(const std::vector<std::string>& p_commandLineArguments)
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
        if (m_inputVideoFilePath.empty() || m_yoloFolderPath.empty() || !numberOfRepetitionsGiven)
        {
            std::cout << Globals::G_PERFORMANCE_TESTS_CLI_HELP_MESSAGE;
            std::exit(1);
        }
    }

    void PerformanceTest::RunPerformanceTest()
    {
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";
        std::cout << "\n\nSettings:";
        std::cout << "\n    Number of tests: " << Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS;
        std::cout << "\n    Number of repetitions: " << m_numberOfRepetitions;

        std::cout << "\nTruncating output table...";
        m_sqlLiteDatabase.TruncateFrameTimesTable();

        VideoManager videoManager;

        const std::chrono::time_point<std::chrono::high_resolution_clock> START_TIME = std::chrono::high_resolution_clock::now();

        for (uint32_t currentTestNumber = 0; currentTestNumber < Globals::G_PERFORMANCE_TESTS_NUMBER_OF_TESTS; currentTestNumber++)
        {
            std::cout << std::format("\n\n######## {} ({}/{}) ########",
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

                m_sqlLiteDatabase.InsertFrameTimes(Globals::G_PERFORMANCE_TESTS_NAMES.at(currentTestNumber), currentRepetition, videoManager.GetFrameTimes());

                std::cout << std::format("\n    Finished repetition {}/{}!", currentRepetition + 1, m_numberOfRepetitions);
            }
        }

        std::cout << "\nGenerating performance graphs...";
        GeneratePerformanceGraphs();

        std::cout << std::format("\n\nTotal elapsed time = {} (H?:mm:ss)", Globals::GetTimeElapsed(START_TIME));
        std::cout << "\n\n################ Lane and Object Detection Performance Tests ################\n\n";
    }

    void PerformanceTest::GeneratePerformanceGraphs()
    {
        // TODO 2
    }

    PerformanceTest::SQLiteDatabase::SQLiteDatabase()
    {
        int32_t resultCode = sqlite3_open("performance_tests_sqlite.db", &m_database);

        CheckResultCode(resultCode);
    }

    PerformanceTest::SQLiteDatabase::~SQLiteDatabase()
    {
        sqlite3_close(m_database);
    }

    void PerformanceTest::SQLiteDatabase::TruncateFrameTimesTable()
    {
        const std::string DROP_TABLE_SQL_STATEMENT = "DROP TABLE IF EXISTS FrameTimes;";
        const std::string CREATE_TABLE_SQL_STATEMENT = "CREATE TABLE FrameTimes"
                                                       "("
                                                       "    Id          INTEGER PRIMARY KEY NOT NULL"
                                                       "    TestName    TEXT                NOT NULL"
                                                       "    Repetition  INTEGER             NOT NULL"
                                                       "    FrameNumber INTEGER             NOT NULL"
                                                       "    FrameTime   INTEGER             NOT NULL"
                                                       ");";

        ExecuteSQLStatement(DROP_TABLE_SQL_STATEMENT);
        ExecuteSQLStatement(CREATE_TABLE_SQL_STATEMENT);
    }

    void PerformanceTest::SQLiteDatabase::InsertFrameTimes(const std::string& p_testName, const uint32_t& p_repetitionNumber, const std::vector<uint32_t>& p_frameTimes)
    {
        for (uint32_t i = 0; i < p_frameTimes.size(); i++)
        {
            ExecuteSQLStatement(std::format("INSERT INTO FrameTimes(TestName, Repetition, FrameNumber, FrameTime) VALUES({}, {}, {}, {});", p_testName, p_repetitionNumber, i + 1, p_frameTimes[i]));
        }
    }

    std::vector<std::vector<uint32_t>> PerformanceTest::SQLiteDatabase::GetFrameTimes(const std::string& p_testName)
    {
        const std::vector<uint32_t> DISTINCT_REPETITIONS = GetSQLStatementOutputColumnValues<uint32_t>(
            ExecuteSelectSQLStatement(std::format("SELECT DISTINCT Repetition FROM FrameTimes WHERE TestName = '{}' ORDER BY Repetition ASC;", p_testName)),
            "Repetition");

        std::vector<std::vector<uint32_t>> frameTimes;
        for (const uint32_t& currentRepetition : DISTINCT_REPETITIONS)
        {
            frameTimes.push_back(GetSQLStatementOutputColumnValues<uint32_t>(
                ExecuteSelectSQLStatement(std::format("SELECT FrameTime FROM FrameTimes WHERE TestName = '{}' AND Repetition = {} ORDER BY FrameNumber ASC;", p_testName, currentRepetition)),
                "FrameTime"));
        }

        return frameTimes;
    }

    void PerformanceTest::SQLiteDatabase::ExecuteSQLStatement(const std::string& p_sqlStatement)
    {
        const int32_t RESULT_CODE = sqlite3_exec(m_database, p_sqlStatement.c_str(), nullptr, nullptr, nullptr);
        CheckResultCode(RESULT_CODE);
    }

    std::vector<std::unordered_map<std::string, std::string>> PerformanceTest::SQLiteDatabase::ExecuteSelectSQLStatement(const std::string& p_sqlStatement)
    {
        sqlite3_stmt* statementHandle;
        std::vector<std::unordered_map<std::string, std::string>> output;

        try
        {
            const int32_t RESULT_CODE = sqlite3_prepare_v2(m_database, p_sqlStatement.c_str(), -1, &statementHandle, nullptr);
            CheckResultCode(RESULT_CODE);

            const int32_t NUMBER_OF_COLUMNS = sqlite3_column_count(statementHandle);

            while (true)
            {
                if (sqlite3_step(statementHandle) == SQLITE_DONE)
                {
                    break;
                }

                std::unordered_map<std::string, std::string> row;

                for (int32_t i = 0; i < NUMBER_OF_COLUMNS; i++)
                {
                    const std::string COLUMN_NAME = std::string(sqlite3_column_name(statementHandle, i));
                    const std::string COLUMN_VALUE = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statementHandle, i)));
                    row.insert({COLUMN_NAME, COLUMN_VALUE});
                }

                output.push_back(row);
            }
        }

        catch (const std::exception& e)
        {
            sqlite3_finalize(statementHandle);
            throw e;
        }

        sqlite3_finalize(statementHandle);

        return output;
    }

    template<typename T>
    std::vector<T> PerformanceTest::SQLiteDatabase::GetSQLStatementOutputColumnValues(const std::vector<std::unordered_map<std::string, std::string>>& p_sqlStatementOutput, const std::string& p_columnName)
    {
        std::vector<T> output;

        for (const std::unordered_map<std::string, std::string>& row : p_sqlStatementOutput)
        {
            for (const std::pair<std::string, std::string>& columnNameValuePair : row)
            {
                if (columnNameValuePair.first == p_columnName)
                {
                    T value;
                    std::istringstream(columnNameValuePair.second) >> value;
                    output.push_back(value);
                    break;
                }
            }
        }

        return output;
    }

    void PerformanceTest::SQLiteDatabase::CheckResultCode(const int32_t& p_resultCode)
    {
        if (p_resultCode == SQLITE_OK)
        {
            return;
        }

        std::cout << "\nERROR: SQLite operation outputted following error message: " + std::string(sqlite3_errmsg(m_database));

        throw Globals::Exceptions::SQLiteDatabaseError();
    }
}
