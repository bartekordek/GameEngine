#include "DX12/ImportWindows.hpp"

using namespace LOGLW;


void ThrowIfFailed( HRESULT hr )
{
    if( FAILED( hr ) )
    {

        char str[64] = {};
        sprintf_s( str, "**ERROR** Fatal Error with HRESULT of %08X\n", static_cast<unsigned int>( hr ) );
        throw;
    }
}
