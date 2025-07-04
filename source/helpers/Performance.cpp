#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "helpers/Globals.hpp"

#include "helpers/Performance.hpp"

namespace LaneAndObjectDetection
{
    Performance::Performance() :
        m_performanceInformation({.m_averageFramesPerSecond = 0, .m_currentFramesPerSecond = 0})
    {}

    void Performance::StartTimer()
    {
        m_startTime = std::chrono::high_resolution_clock::now();
    }

    void Performance::EndTimer()
    {
        const uint32_t FRAME_TIME = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - m_startTime).count();

        m_frameTimes.push_back(FRAME_TIME);

        m_performanceInformation.m_currentFramesPerSecond = Globals::G_MICROSECONDS_IN_SECOND / FRAME_TIME;

        // Calculate the average FPS as a running average
        const double FRAME_COUNT = static_cast<double>(m_frameTimes.size());
        m_performanceInformation.m_averageFramesPerSecond = ((m_performanceInformation.m_averageFramesPerSecond * (FRAME_COUNT - 1)) + m_performanceInformation.m_currentFramesPerSecond) / FRAME_COUNT;
    }

    PerformanceInformation Performance::GetInformation()
    {
        return m_performanceInformation;
    }

    std::vector<uint32_t> Performance::GetFrameTimes()
    {
        return m_frameTimes;
    }

    std::string Performance::GetTimeUnit()
    {
        return Globals::G_TIME_UNIT;
    }

    uint32_t Performance::GetTimeUnitConversion()
    {
        return static_cast<uint32_t>(Globals::G_MICROSECONDS_IN_SECOND);
    }

    void Performance::ClearPerformanceInformation()
    {
        m_frameTimes.clear();
        m_performanceInformation = {.m_averageFramesPerSecond = 0, .m_currentFramesPerSecond = 0};
    }
}
