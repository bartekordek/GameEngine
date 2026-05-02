#pragma once

#include "gameengine/BasicTypes.hpp"
#include "gameengine/Defines.hpp"
#include "CUL/String/StringWrapper.hpp"
#include "CUL/Memory/DataWrapper.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class VertexArray;

struct AttributeMeta
{
    String Name;
    std::int32_t Index = 0;
    std::int32_t Size = 0;
    DataType Type = DataType::NONE;
    bool Normalized = false;
    std::int32_t StrideBytes{ 0 };
    void* DataOffset{ nullptr };

    AttributeMeta() = default;

    AttributeMeta( const String& inName, std::int32_t inIndex, std::int32_t inSize, DataType inType, bool inNormalized,
                   std::int32_t inStrideBytes, void* inDataOffset ):
        Name( inName ),
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
    LOGLW::PrimitiveType primitiveType = LOGLW::PrimitiveType::TRIANGLES;
    std::uint32_t VBO = 0u;
    std::uint32_t VAO = 0u;
    std::vector<AttributeMeta> Attributes;
    CUL::DataWrapper Data;
    CUL::DataWrapper Indices;
};

struct DataWrapper
{
    LOGLW::PrimitiveType primitiveType = LOGLW::PrimitiveType::TRIANGLES;
    std::string Name;
    std::size_t Size{ 0u };
    std::size_t Stride{ 0u };
    std::size_t Offset{ 0u };
    std::size_t Index{ 0u };
    std::uint32_t VBO = 0u;
    std::uint32_t VAO = 0u;
    std::vector<float> Data;
};

NAMESPACE_END( LOGLW )
