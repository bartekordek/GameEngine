#include "gameengine/VertexArray.hpp"

NAMESPACE_BEGIN( LOGLW )

class GAME_ENGINE_API VAOConcrete final:
    public IUtilityUser
{
public:
    VAOConcrete();

    ~VAOConcrete();
protected:
private:
    VAOConcrete( const VAOConcrete& arg ) = delete;
    VAOConcrete( VAOConcrete&& arg ) = delete;
    VAOConcrete& operator=( const VAOConcrete& arg ) = delete;
    VAOConcrete& operator=( VAOConcrete&& arg ) = delete;

};

NAMESPACE_END( LOGLW )