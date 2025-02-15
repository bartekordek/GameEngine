#include "gameengine/Render/FrameTimeManager.hpp"
#include "CUL/IMPORT_tracy.hpp"
#include "CUL/STL_IMPORTS/STD_thread.hpp"

using namespace LOGLW;

FrameTimeManager::FrameTimeManager()
{
    m_samples.setCapacity( 32u );
}

void FrameTimeManager::setFrameLengthMs( float inFrameLengthMs )
{
    m_targetFrameTimeNs = inFrameLengthMs / 1000000.f;
}

void FrameTimeManager::setTargetFPS( float inTargetFPS )
{
    m_targetFrameTimeNs = 1000000000.f / inTargetFPS;
}

float LOGLW::FrameTimeManager::getTargetFrameTimeNS() const
{
    return m_targetFrameTimeNs;
}

float LOGLW::FrameTimeManager::getTargetFrameTimeMS() const
{
    return m_targetFrameTimeNs / 1000.f;
}

float LOGLW::FrameTimeManager::geAvgFrameTimeNS() const
{
    return m_samples.getAverage();
}

float LOGLW::FrameTimeManager::geAvgFrameTimeMS() const
{
    return m_samples.getAverage() / 1000.f;
}

float LOGLW::FrameTimeManager::getTargetFPS() const
{
    return 1.f / m_targetFrameTimeNs;
}

void FrameTimeManager::startFrame()
{
    m_frameStart = m_clock.now();
}

void FrameTimeManager::endFrame()
{
    ZoneScoped;
    m_frameEnd = m_clock.now();
    const auto difference = m_frameEnd - m_frameStart;
    m_lastFrameTimeNs = std::chrono::duration_cast<std::chrono::nanoseconds>( difference ).count();
    static float differenceNs = m_targetFrameTimeNs - m_lastFrameTimeNs;
    if( differenceNs > 0 )
    {
        std::this_thread::sleep_for( std::chrono::nanoseconds( static_cast<std::uint64_t>( differenceNs ) ) );
    }
    m_samples.add( m_lastFrameTimeNs + differenceNs );
}

FrameTimeManager::~FrameTimeManager()
{
}
