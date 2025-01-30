#include "WindowFactoryConcrete.hpp"
#include "RegularSDL2Window.hpp"

#include "gameengine/ISDL2Wrapper.hpp"

#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/STL_IMPORTS/STD_cstring.hpp"

using namespace LOGLW;

WindowCreatorConcrete::WindowCreatorConcrete( CUL::LOG::ILogger* logger ) : m_logger( logger )
{
    m_logger->log( "Initialized WindowCreatorConcrete... Done" );
}

IWindow* WindowCreatorConcrete::createWindow( const WinData& winData, ISDL2Wrapper* wrapper )
{
    CUL::Assert::simple( wrapper != nullptr, "SDL WRAPPER IS NULL!" );
    return new RegularSDL2Window( winData, wrapper, wrapper->getCul() );
}

WindowCreatorConcrete::~WindowCreatorConcrete()
{
}