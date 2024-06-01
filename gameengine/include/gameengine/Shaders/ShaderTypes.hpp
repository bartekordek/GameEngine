#pragma once

#include "gameengine/Import.hpp"
#include "CUL/String.hpp"

#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API CShaderTypes
{
public:
    enum class ShaderType : uint_fast8_t
    {
        Unkown = 0u,
        Vertex,
        Pixel,
        Geometry
    };

    static ShaderType getShaderType( const CUL::String& extension );
};


NAMESPACE_END( LOGLW )