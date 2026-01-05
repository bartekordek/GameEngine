#pragma once

#include "gameengine/Import.hpp"
#include "CUL/GenericUtils/NonCopyable.hpp"

namespace LOGLW
{
class GAME_ENGINE_API ITextureFrameBuffer
{
public:
    ITextureFrameBuffer() = default;

    virtual void beginCapture() = 0;
    virtual void endCapture() = 0;
    virtual void drawCapture() = 0;

    virtual ~ITextureFrameBuffer() = default;

protected:
private:
    CUL_NONCOPYABLE( ITextureFrameBuffer )
};
}  // namespace LOGLW