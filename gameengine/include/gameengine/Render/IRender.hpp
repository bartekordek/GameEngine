#pragma once

#include "gameengine/Import.hpp"

#include "CUL/Video/IFPSObservable.hpp"
#include "CUL/Graphics/Color.hpp"

NAMESPACE_BEGIN( LOGLW )

using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;

class GAME_ENGINE_API IRender: public CUL::Video::IFPSObservable
{
public:
    IRender() noexcept;

    virtual void setBackgroundColor( const ColorS& color ) = 0;
    virtual void setBackgroundColor( const ColorE color ) = 0;
    virtual ColorS getBackgroundColor() const = 0;
    virtual void clearBuffers() = 0;
    virtual void renderAll() = 0;
    virtual void updateScreenBuffers() = 0;

    virtual ~IRender();

protected:
private:
    IRender( const IRender& arg ) = delete;
    IRender( IRender&& arg ) = delete;
    IRender& operator=( const IRender& arg ) = delete;
    IRender& operator=( IRender&& arg ) = delete;
};

NAMESPACE_END( LOGLW )