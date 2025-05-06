#include "gameengine/Shaders/ShaderTypes.hpp"

using namespace LOGLW;


CShaderTypes::ShaderType CShaderTypes::getShaderType( const CUL::String& extension )
{
    if( extension.equals( ".vs" ) || extension.equals( "vs" ) || extension == ".vert" || extension == "vert" )
    {
        return ShaderType::Vertex;
    }

    if( extension.equals( ".fs" ) || extension.equals( "fs" ) || extension == ".frag" || extension == "frag" )
    {
        return ShaderType::Pixel;
    }

    return ShaderType::Unkown;
}