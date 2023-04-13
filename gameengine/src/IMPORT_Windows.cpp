#include "gameengine/IMPORT_Windows.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "CUL/STL_IMPORTS/STD_exception.hpp"

void WindowsUtils::ThrowIfFailed( HRESULT hr )
{
    if (FAILED(hr))
    {
        throw std::exception();
    }
}

#endif // #if defined(GAME_ENGINE_WINDOWS)