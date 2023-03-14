#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)


#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN( LOGLW )

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

void ThrowIfFailed( HRESULT hr );


NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS