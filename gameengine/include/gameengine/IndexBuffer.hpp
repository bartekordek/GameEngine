#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class IndexBuffer final : private IUtilityUser, public CUL::IName, public CUL::IRegisterdObject
{
public:
    GAME_ENGINE_API IndexBuffer();
    GAME_ENGINE_API void loadData( std::vector<unsigned>& data );
    GAME_ENGINE_API const std::vector<unsigned>& getData() const;

    GAME_ENGINE_API void bind();

    GAME_ENGINE_API ~IndexBuffer();

private:
    std::uint32_t m_id = 0u;

    std::vector<unsigned> m_data;

private:
    IndexBuffer( const IndexBuffer& arg ) = delete;
    IndexBuffer( IndexBuffer&& arg ) = delete;
    IndexBuffer& operator=( const IndexBuffer& rhv ) = delete;
    IndexBuffer& operator=( IndexBuffer&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )