#pragma once

#include "gameengine/IGameEngineApp.hpp"

class CApp final: public LOGLW::IGameEngineApp
{
public:
    CApp( bool fullscreen, unsigned width, unsigned height, int x, int y, const char* winName, const char* configPath );

    ~CApp();

protected:
private:
    void onInit() override;
    void customFrame() override;
    void customLogicThreadFrame( LOGLW::DurationType ) override;
    void guiIteration( float x, float y );

public:
    CUL_NONCOPYABLE( CApp )
};