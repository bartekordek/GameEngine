#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#undef ERROR
#undef min
#undef max
#undef interface

struct WindowsUtils
{
	static void ThrowIfFailed( HRESULT hr );
};

#endif // #if defined(GAME_ENGINE_WINDOWS)