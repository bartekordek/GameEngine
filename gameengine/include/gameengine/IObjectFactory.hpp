#pragma once

#include "gameengine/Primitives/IQuad.hpp"
#include "gameengine/Primitives/IPoint.hpp"

#include "CUL/Filesystem/IFile.hpp"

NAMESPACE_BEGIN( LOGLW )

class Sprite;
class Triangle;
class Line;
using String = CUL::String;

class GAME_ENGINE_API IObjectFactory
{
public:
    IObjectFactory() = default;
    virtual ~IObjectFactory() = default;

    virtual IObject* createFromFile( IFile* file ) = 0;
    virtual IObject* createFromFile( const String& path ) = 0;

    virtual Triangle* createTriangle( const TriangleData& data, const ColorS& color, bool forceLegacy = false ) = 0;

    virtual IQuad* createQuad( const QuadData& data, bool legacy = false, const ColorS& color = ColorE::WHITE ) = 0;

    virtual IPoint* createPoint(const Point& position, const ColorS& color = ColorE::WHITE) = 0;

    virtual Sprite* createSprite( const String& path, bool withVBO = false ) = 0;
    virtual Sprite* createSprite( unsigned* data, unsigned width,
                                  unsigned height, bool withVBO = false ) = 0;

    virtual void removeObject( IObject* object ) = 0;

protected:
private:

// Deleted:
    IObjectFactory( const IObjectFactory& arg ) = delete;
    IObjectFactory( IObjectFactory&& arg ) = delete;
    IObjectFactory& operator=( const IObjectFactory& rhv ) = delete;
    IObjectFactory& operator=( IObjectFactory&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )