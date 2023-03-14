#include "DX12/ImportWindows.hpp"

#if defined(GAME_ENGINE_WINDOWS)


void LOGLW::ThrowIfFailed( HRESULT hr )
{
    if( FAILED( hr ) )
    {

        char str[64] = {};
        sprintf_s( str, "**ERROR** Fatal Error with HRESULT of %08X\n", static_cast<unsigned int>( hr ) );
        throw;
    }
}

#endif // GAME_ENGINE_WINDOWS