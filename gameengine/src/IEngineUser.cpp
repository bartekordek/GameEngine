#include "gameengine/IEngineUser.hpp"

using namespace LOGLW;


IEngineUser::IEngineUser(IGameEngine* engine):
m_engine(engine)
{

}

IGameEngine* IEngineUser::getEngine()
{
    return m_engine;
}

IEngineUser::~IEngineUser()
{

}