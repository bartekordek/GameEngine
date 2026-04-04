#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

namespace LOGLW
{
class GAME_ENGINE_API ITextureFrameBuffer
{
public:
    ITextureFrameBuffer() = default;

    virtual void beginCapture() = 0;
    virtual void endCapture() = 0;
    virtual void drawCapture() = 0;
    virtual void setSize( std::int32_t inWidth, std::int32_t inHeight ) = 0;

    virtual ~ITextureFrameBuffer() = default;

protected:
private:
    CUL_NONCOPYABLE( ITextureFrameBuffer )
};
}  // namespace LOGLW