#include "gameengine/ShaderTypes.hpp"

using namespace LOGLW;


ShaderTypes::ShaderType ShaderTypes::getShaderType( const CUL::String& extension )
{
    if( extension == ".vert" )
    {
        return ShaderType::Vertex;
    }

    if( extension == ".frag" )
    {
        return ShaderType::Pixel;
    }

    return ShaderType::Unkown;
}