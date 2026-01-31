#include "gameengine/Shaders/ShaderTypes.hpp"

using namespace LOGLW;

CShaderTypes::ShaderType CShaderTypes::getShaderType( const String& extension )
{
    if( extension.equals( ".vs" ) || extension.equals( "vs" ) || extension.equals( ".vert" ) || extension.equals( "vert" ) )
    {
        return ShaderType::Vertex;
    }

    if( extension.equals( ".fs" ) || extension.equals( "fs" ) || extension.equals( ".frag" ) || extension.equals( "frag" ) )
    {
        return ShaderType::Pixel;
    }

    return ShaderType::Unkown;
}