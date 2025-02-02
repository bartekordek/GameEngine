#include "gameengine/Windowing/WinData.hpp"

using namespace LOGLW;

WinData::WinData()
{

}

WinData::WinData( const WinData& arg ):
	Pos( arg.Pos ),
	CurrentRes( arg.CurrentRes ),
	Name( arg.Name ),
	RendererType( arg.RendererType )
{
}

WinData::WinData( WinData&& arg ) noexcept
    :
	Pos( arg.Pos ),
	CurrentRes( arg.CurrentRes ),
	Name( arg.Name ),
	RendererType( arg.RendererType )
{
    arg.Pos = { 0, 0 };
    arg.CurrentRes = {0u, 0u};
    arg.NativeRes = { 0u, 0u };
    arg.WindowRes = { 0u, 0u };
    arg.Name.clear();
    arg.RendererType = RenderTypes::RendererType::NONE;
}

WinData& WinData::operator=( const WinData& arg )
{
    if( this != &arg )
    {
        Pos = arg.Pos;
        CurrentRes = arg.CurrentRes;
        Name = arg.Name;
        RendererType = arg.RendererType;
    }
    return *this;
}

WinData& LOGLW::WinData::operator=( WinData&& arg ) noexcept
{
    if( this != &arg )
    {
        Pos = arg.Pos;
        CurrentRes = arg.CurrentRes;
        Name = arg.Name;
        RendererType = arg.RendererType;

        arg.Pos = { 0, 0 };
        arg.CurrentRes = { 0u, 0u };
        arg.NativeRes = { 0u, 0u };
        arg.WindowRes = { 0u, 0u };
        arg.Name.clear();
        arg.RendererType = RenderTypes::RendererType::NONE;
    }
    return *this;
}

WinData::~WinData()
{
}