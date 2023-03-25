#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "gameengine/DX12/d3dx12.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#undef ERROR
#undef min
#undef max
#undef interface

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct WindowsUtils
{
	static void ThrowIfFailed( HRESULT hr );
};

#endif // #if defined(GAME_ENGINE_WINDOWS)