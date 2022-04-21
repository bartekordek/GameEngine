#include "gameengine/IUtilityUser.hpp"

using namespace LOGLW;

IUtility* IUtilityUser::s_utility = nullptr;

IUtilityUser::IUtilityUser()
{
}

void IUtilityUser::useUtility( IUtility* utility )
{
    s_utility = utility;
}

IUtility* IUtilityUser::getUtility()
{
    return s_utility;
}

IUtilityUser::~IUtilityUser()
{
}