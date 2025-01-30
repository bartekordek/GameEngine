#include "SDL2Wrapper/RendererTypes.hpp"

using namespace SDL2W;

RenderTypes::RendererType RenderTypes::convertToEnum( const CUL::String& name )
{
    if( name.toLowerR() == "software" )
    {
        return RendererType::SOFTWARE;
    }
    else if( name.toLowerR() == "directx_9" || name.toLowerR() == "direct3d" )
    {
        return RendererType::DIRECTX_9;
    }
    else if( ( name.toLowerR() == "directx_11" ) || ( name.toLowerR() == "direct3d11" ) )
    {
        return RendererType::DIRECTX_11;
    }
    else if( name.toLowerR() == "directx_12" )
    {
        return RendererType::DIRECTX_12;
    }
    else if( name.toLowerR() == "opengl_legacy" )
    {
        return RendererType::OPENGL_LEGACY;
    }
    else if( ( name.toLowerR() == "opengl_modern" ) || ( name.toLowerR() == "opengl" ) ) 
    {
        return RendererType::OPENGL_MODERN;
    }
    else if( name.toLowerR() == "opengl_es2" )
    {
        return RendererType::OPENGL_ES2;
    }

    return RendererType::NONE;
}

const CUL::String RenderTypes::convertToString( RendererType type )
{
    switch( type )
    {
        case SDL2W::RenderTypes::RendererType::NONE:
            return "NONE";
            break;
        case SDL2W::RenderTypes::RendererType::SOFTWARE:
            return "SOFTWARE";
            break;
        case SDL2W::RenderTypes::RendererType::OPENGL_LEGACY:
            return "OPENGL_LEGACY";
            break;
        case SDL2W::RenderTypes::RendererType::OPENGL_MODERN:
            return "OPENGL_MODERN";
            break;
        case SDL2W::RenderTypes::RendererType::OPENGL_ES2:
            return "OPENGL_ES2";
            break;
        case SDL2W::RenderTypes::RendererType::DIRECTX_9:
            return "DIRECTX_9";
            break;
        case SDL2W::RenderTypes::RendererType::DIRECTX_11:
            return "DIRECTX_11";
            break;
        case SDL2W::RenderTypes::RendererType::DIRECTX_12:
            return "DIRECTX_12";
            break;
        default:
            return "NONE";
            break;
    }
}