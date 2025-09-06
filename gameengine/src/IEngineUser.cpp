#include "gameengine/IEngineUser.hpp"
#include <gameengine/IGameEngine.hpp>

using namespace LOGLW;


IEngineUser::IEngineUser():
    m_engine( *IGameEngine::getInstance() )
{
}

IGameEngine& IEngineUser::getEngine()
{
    return m_engine;
}

IEngineUser::~IEngineUser()
{

}