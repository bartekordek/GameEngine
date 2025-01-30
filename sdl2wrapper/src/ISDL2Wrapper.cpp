#include "SDL2Wrapper/ISDL2Wrapper.hpp"
#include "SDL2WrapperImpl.hpp"

using namespace SDL2W;

ISDL2Wrapper::ISDL2Wrapper()
{
}

ISDL2Wrapper* ISDL2Wrapper::createSDL2Wrapper()
{
    return new SDL2WrapperImpl();
}

ISDL2Wrapper::~ISDL2Wrapper()
{
}

int ISDL2Wrapper::getRendererId( const RenderTypes::RendererType type ) const
{
    return m_renderers.at( type );
}

const std::map<RenderTypes::RendererType, int>& ISDL2Wrapper::getRenderersList() const
{
    return m_renderers;
}