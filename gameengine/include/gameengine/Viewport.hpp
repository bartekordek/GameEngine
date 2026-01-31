#pragma once

#include "gameengine/String.hpp"

#include "CUL/Graphics/Pos2D.hpp"
#include "CUL/Graphics/Size2D.hpp"
#include "CUL/ISerializable.hpp"

NAMESPACE_BEGIN( LOGLW )

using Pos2Di = CUL::Graphics::Pos2Di;
using Size2Di = CUL::Graphics::Size2Di;

class GAME_ENGINE_API Viewport final:
    public CUL::ISerializable
{
public:
    Viewport();

    Pos2Di pos;
    Size2Di size;

    void set( const Pos2Di& newPos, const Size2Di newSize );

    bool operator!=( const Viewport& arg ) const;
    bool operator==( const Viewport& arg ) const;

    Viewport& operator=( const Viewport& rhv );

    ~Viewport();

protected:
private:
    String getSerializationContent( CUL::CounterType tabsSize, const bool separator = false ) const override;

    Viewport( const Viewport& rhv ) = delete;
    Viewport( Viewport&& rhv ) = delete;

    Viewport& operator=( Viewport&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )