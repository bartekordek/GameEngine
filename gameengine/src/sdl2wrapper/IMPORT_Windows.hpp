#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/DisableErros.hpp"
CUL_MSVC_SKIP_DEFAULT_WARNING_BEGIN

#if defined( CUL_WINDOWS )
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif  // #if defined(CUL_WINDOWS)

#ifdef _MSC_VER
#pragma warning( pop )
#endif