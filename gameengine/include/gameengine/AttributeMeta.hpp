#pragma once

#include "gameengine/BasicTypes.hpp"
#include "CUL/String.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

NAMESPACE_BEGIN( LOGLW )

class VertexArray;
class VertexBuffer;

struct AttributeMeta
{
    CUL::String Name;
    int Index = 0;
    int Size = 0;
    DataType Type = DataType::NONE;
    bool Normalized = false;
    int Stride = 0;
    void* Pointer = nullptr;

    AttributeMeta( const CUL::String& inName, int inIndex, int inSize, DataType inType, bool inNormalized, int inStride, void* inPointer )
        : Name( inName ),
          Index( inIndex ),
          Size( inSize ),
          Type( inType ),
          Normalized( inNormalized ),
          Stride( inStride ),
          Pointer( inPointer )
    {
    }

    AttributeMeta() = delete;
};

struct VertexData
{
    LOGLW::PrimitiveType primitiveType = LOGLW::PrimitiveType::NONE;
    VertexArray* VAO = nullptr;
    VertexBuffer* VBO = nullptr;
    std::vector<AttributeMeta> Attributes;
    std::vector<float> vertices;
    std::vector<unsigned> indices;
};


NAMESPACE_END( LOGLW )