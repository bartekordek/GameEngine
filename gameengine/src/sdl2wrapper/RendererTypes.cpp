#include "gameengine/Render/RendererTypes.hpp"

using namespace LOGLW;

RenderTypes::RendererType RenderTypes::convertToEnum( const String& inName )
{
    String name = inName;
    name.toLower();
    if( name.equals( "software" ) )
    {
        return RendererType::SOFTWARE;
    }
    else if( name.equals( "directx_9" ) || name.equals( "direct3d" ) )
    {
        return RendererType::DIRECTX_9;
    }
    else if( name.equals( "directx_11" ) || name.equals( "direct3d11" ) )
    {
        return RendererType::DIRECTX_11;
    }
    else if( name.equals( "directx_12" ) )
    {
        return RendererType::DIRECTX_12;
    }
    else if( name.equals( "opengl_legacy" ) )
    {
        return RendererType::OPENGL_LEGACY;
    }
    else if( name.equals( "opengl_modern" ) || name.equals( "opengl" ) )
    {
        return RendererType::OPENGL_MODERN;
    }
    else if( name.equals( "opengl_es2" ) )
    {
        return RendererType::OPENGL_ES2;
    }

    return RendererType::NONE;
}

const String RenderTypes::convertToString( RendererType type )
{
    switch( type )
    {
        case LOGLW::RenderTypes::RendererType::NONE:
            return "NONE";
            break;
        case LOGLW::RenderTypes::RendererType::SOFTWARE:
            return "SOFTWARE";
            break;
        case LOGLW::RenderTypes::RendererType::OPENGL_LEGACY:
            return "OPENGL_LEGACY";
            break;
        case LOGLW::RenderTypes::RendererType::OPENGL_MODERN:
            return "OPENGL_MODERN";
            break;
        case LOGLW::RenderTypes::RendererType::OPENGL_ES2:
            return "OPENGL_ES2";
            break;
        case LOGLW::RenderTypes::RendererType::DIRECTX_9:
            return "DIRECTX_9";
            break;
        case LOGLW::RenderTypes::RendererType::DIRECTX_11:
            return "DIRECTX_11";
            break;
        case LOGLW::RenderTypes::RendererType::DIRECTX_12:
            return "DIRECTX_12";
            break;
        default:
            return "NONE";
            break;
    }
}