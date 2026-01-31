#pragma once

#include "gameengine/String.hpp"


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

    static ShaderType getShaderType( const String& extension );
};

NAMESPACE_END( LOGLW )