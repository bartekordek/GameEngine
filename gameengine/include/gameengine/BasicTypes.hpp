#pragma once

#include "gameengine/Import.hpp"

NAMESPACE_BEGIN( LOGLW )

enum class MatrixTypes : int
{
    MODELVIEW = 5888,
    PROJECTION = 5889,
    INVALID = 0x0500
};

enum class ClearMasks : unsigned
{
    COLOR_BUFFER_BIT = 0x00004000
};

enum class BufferTypes : unsigned
{
    NONE = 0,
    VERTEX_ARRAY = 1,              // VAO
    ARRAY_BUFFER = 0x8892,         // VBO
    ELEMENT_ARRAY_BUFFER = 0x8893  // Index buffer
};

enum class PrimitiveType : unsigned
{
    NONE = 0,
    LINE_STRIP = 0x0003,
    TRIANGLES = 0x0004,
    TRIANGLE_STRIP = 0x0005,
    TRIANGLE_FAN = 0x0006,
    QUADS = 0x0007,
    QUAD_STRIP = 0x0008
};

enum class DataType : int
{
    NONE = 0,
    BYTE = 0x1400,
    UNSIGNED_BYTE = 0x1401,
    SHORT = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT = 0x1404,
    UNSIGNED_INT = 0x1405,
    FLOAT = 0x1406,
    DOUBLE = 0x140A
};

inline const char* DataTypeToName( DataType inDataType )
{
    if( inDataType == DataType::BYTE )
    {
        return "byte";
    }

    if( inDataType == DataType::UNSIGNED_BYTE )
    {
        return "unsigned byte";
    }

    if( inDataType == DataType::SHORT )
    {
        return "short";
    }

    if( inDataType == DataType::UNSIGNED_SHORT )
    {
        return "unsigned short";
    }

    if( inDataType == DataType::INT )
    {
        return "int";
    }

    if( inDataType == DataType::UNSIGNED_INT )
    {
        return "unsigned int";
    }

    if( inDataType == DataType::FLOAT )
    {
        return "float";
    }

    if( inDataType == DataType::DOUBLE )
    {
        return "double";
    }

    return "none";
}

enum class TextureFilterType : short
{
    NONE = 0,
    NEAREST = 0x2600,
    LINEAR = 0x2601
};

enum class TextureParameters : short
{
    NONE = 0,
    MAG_FILTER = 0x2800,
    MIN_FILTER = 0x2801,
    WRAP_S = 0x2802,
    WRAP_T = 0x2803
};

enum class ClientStateTypes : unsigned
{
    VERTEX_ARRAY = 0x8074,
    COLOR_ARRAY = 0x8076,
    TEXTURE_COORD_ARRAY = 0x8078
};

enum class TextureTypes : short
{
    T_2D = 0,
    T_PROXY_TEXTURE_2D,
    T_TEXTURE_1D_ARRAY,
    T_PROXY_TEXTURE_1D_ARRAY
};

NAMESPACE_END( LOGLW )
