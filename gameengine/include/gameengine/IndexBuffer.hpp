#pragma once

#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "gameengine/IUtilityUser.hpp"

NAMESPACE_BEGIN( LOGLW )

class IndexBuffer final : private IUtilityUser
{
public:
    GAME_ENGINE_API IndexBuffer();
    GAME_ENGINE_API void loadData( std::vector<unsigned>& data );
    GAME_ENGINE_API const std::vector<unsigned>& getData() const;

    GAME_ENGINE_API void bind();

    GAME_ENGINE_API ~IndexBuffer();

private:
    unsigned m_id = 0;

    std::vector<unsigned> m_data;

private:
    IndexBuffer( const IndexBuffer& arg ) = delete;
    IndexBuffer( IndexBuffer&& arg ) = delete;
    IndexBuffer& operator=( const IndexBuffer& rhv ) = delete;
    IndexBuffer& operator=( IndexBuffer&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )