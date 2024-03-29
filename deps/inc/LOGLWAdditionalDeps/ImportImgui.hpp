#pragma once

#include "CUL/GenericUtils/DisableErros.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl2.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef _MSC_VER
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx12.h"
#endif

#ifdef _MSC_VER
#pragma warning( pop )
#endif