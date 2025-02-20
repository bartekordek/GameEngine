#pragma once

#include "CUL/GenericUtils/Singleton.hpp"
#include "CUL/Math/VariableWithAverageValue.hpp"
#include "CUL/STL_IMPORTS/STD_chrono.hpp"

namespace LOGLW
{
class FrameTimeManager final: public CUL::Singleton<FrameTimeManager>
{
public:
    FrameTimeManager();

    void setFrameLengthMs( float inFrameLengthMs );
    void setTargetFPS( float inTargetFPS );
    void setSamplesCount( std::size_t inSamplesCount );

    float getTargetFrameTimeNS() const;
    float getTargetFrameTimeMS() const;
    float geAvgFrameTimeNS() const;
    float geAvgFrameTimeMS() const;

    float getTargetFPS() const;

    void startFrame();
    void endFrame();

    ~FrameTimeManager();

    FrameTimeManager( const FrameTimeManager& ) = delete;
    FrameTimeManager( FrameTimeManager&& ) = delete;
    FrameTimeManager& operator=( const FrameTimeManager& ) = delete;
    FrameTimeManager& operator=( FrameTimeManager&& ) = delete;

protected:
private:


    float m_targetFrameTimeNs{ 0u };
    float m_lastFrameTimeNs{ 0u };
    CUL::MATH::VariableWithAverageValue m_samples;

    std::chrono::high_resolution_clock m_clock;
    std::chrono::high_resolution_clock::time_point m_frameStart;
    std::chrono::high_resolution_clock::time_point m_frameEnd;
};
}  // namespace LOGLW