#pragma once

#include "gameengine/Render/RendererTypes.hpp"
#include "gameengine/Windowing/WinSize.hpp"
#include "gameengine/Defines.hpp"

namespace LOGLW
{
struct WinPos
{
    std::int32_t X{ static_cast<std::int32_t>( 0 ) };
    std::int32_t Y{ static_cast<std::int32_t>( 0 ) };
};

class GAME_ENGINE_API WinData final
{
public:
    WinPos Pos;
    WinSize CurrentRes{ .W = 640u, .H = 480u };
    WinSize NativeRes;
    WinSize WindowRes;
    String Name;
    RenderTypes::RendererType RendererType{ RenderTypes::RendererType::NONE };
    bool Resizable{ false };

protected:
private:
    // Deleted:

};
}  // namespace LOGLW