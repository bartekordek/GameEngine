#include "gameengine/IUtilityUser.hpp"

using namespace LOGLW;

IRenderDevice* IUtilityUser::s_utility = nullptr;

IUtilityUser::IUtilityUser()
{
}

void IUtilityUser::useUtility( IRenderDevice* utility )
{
    s_utility = utility;
}

IRenderDevice* IUtilityUser::getDevice()
{
    return s_utility;
}

IUtilityUser::~IUtilityUser()
{
}