#pragma once

#include "gameengine/IUtilityUser.hpp"
#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Memory/DataWrapper.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class IndexBuffer final:
    private IUtilityUser,
    public CUL::IName,
    public CUL::IRegisterdObject
{
public:
    GAME_ENGINE_API IndexBuffer();
    GAME_ENGINE_API void loadData( const CUL::DataWrapper& inData );
    GAME_ENGINE_API const CUL::DataWrapper& getData() const;
    GAME_ENGINE_API std::uint32_t getObjID() const;
    GAME_ENGINE_API void bind();

    GAME_ENGINE_API ~IndexBuffer();

protected:
    void onNameChange( const CUL::StringWr& newName ) override;

private:
    void init();
    std::uint32_t m_id = 0u;

    CUL::DataWrapper m_data;

private:
    IndexBuffer( const IndexBuffer& arg ) = delete;
    IndexBuffer( IndexBuffer&& arg ) = delete;
    IndexBuffer& operator=( const IndexBuffer& rhv ) = delete;
    IndexBuffer& operator=( IndexBuffer&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )
