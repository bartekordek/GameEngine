#pragma once

#include "SDL2Wrapper/Import.hpp"
#include "CUL/String.hpp"

NAMESPACE_BEGIN( SDL2W )

class SDL2WAPI RenderTypes
{
public:
    enum class RendererType : short
    {
        NONE = 0,
        SOFTWARE,
        OPENGL_LEGACY,
        OPENGL_MODERN,
        OPENGL_ES2,
        DIRECTX_9,
        DIRECTX_11,
        DIRECTX_12
    };

    static RendererType convertToEnum( const CUL::String& name );
    static const CUL::String convertToString( RendererType type );
};


NAMESPACE_END( SDL2W )