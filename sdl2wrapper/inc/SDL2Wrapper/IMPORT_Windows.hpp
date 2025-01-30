#pragma once

#include "SDL2Wrapper/Import.hpp"
#include "CUL/GenericUtils/DisableErros.hpp"

#if defined(SDL2W_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif // #if defined(SDL2W_WINDOWS)

#ifdef _MSC_VER
#pragma warning( pop )
#endif