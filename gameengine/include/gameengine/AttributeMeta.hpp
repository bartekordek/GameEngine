#pragma once

#include "gameengine/BasicTypes.hpp"
#include "CUL/String.hpp"
#include "CUL/Memory/DataWrapper.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class VertexArray;

struct AttributeMeta
{
    CUL::String Name;
    std::int32_t Index = 0;
    std::int32_t Size = 0;
    DataType Type = DataType::NONE;
    bool Normalized = false;
    std::int32_t StrideBytes{ 0 };
    void* DataOffset{ nullptr };

    AttributeMeta() = default;

    AttributeMeta( const CUL::String& inName, std::int32_t inIndex, std::int32_t inSize, DataType inType, bool inNormalized,
                   std::int32_t inStrideBytes,
                   void* inDataOffset )
        : Name( inName ),
          Index( inIndex ),
          Size( inSize ),
          Type( inType ),
          Normalized( inNormalized ),
          StrideBytes( inStrideBytes ),
          DataOffset( inDataOffset )
    {
    }
};

struct VertexData
{
    LOGLW::PrimitiveType primitiveType = LOGLW::PrimitiveType::NONE;
    std::uint32_t VBO = 0u;
    std::uint32_t VAO = 0u;
    std::vector<AttributeMeta> Attributes;
    CUL::DataWrapper Data;
    CUL::DataWrapper Indices;
};

NAMESPACE_END( LOGLW )
