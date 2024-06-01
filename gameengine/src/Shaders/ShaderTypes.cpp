#include "gameengine/Shaders/ShaderTypes.hpp"

using namespace LOGLW;


CShaderTypes::ShaderType CShaderTypes::getShaderType( const CUL::String& extension )
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