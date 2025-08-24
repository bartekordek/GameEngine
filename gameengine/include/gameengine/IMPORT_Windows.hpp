#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "CUL/GenericUtils/DisableErros.hpp"
CUL_MSVC_SKIP_DEFAULT_WARNING_BEGIN
#include <windows.h>
#include <windowsx.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#include "gameengine/DX12/d3dx12.h"
#ifdef _MSC_VER
#pragma warning( pop )
#endif

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