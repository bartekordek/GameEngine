#pragma once

#include "gameengine/Import.hpp"
#include "CUL/STL_IMPORTS/STD_stddef.hpp"
#include "CUL/STL_IMPORTS/STD_cstdint.hpp"

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

enum class DataType : std::uint16_t
{
    NONE = 0,
    BYTE = 0x1400,
    UNSIGNED_BYTE = 0x1401,
    SHORT = 0x1402,
    UNSIGNED_SHORT = 0x1403,
    INT = 0x1404,
    UNSIGNED_INT = 0x1405,
    FLOAT = 0x1406,       // 5126
    DOUBLE = 0x140A,      // 5130
    FLOAT_VEC2 = 0x8B50,  // 35664
    FLOAT_VEC3 = 0x8B51,  // 35665
    FLOAT_VEC4 = 0x8B52,  // 35666
    FLOAT_MAT2 = 0x8B5A,  // 35674
    FLOAT_MAT3 = 0x8B5B,  // 35675
    FLOAT_MAT4 = 0x8B5C,  // 35676
    SAMPLER_1D = 0x8B5D,
    SAMPLER_2D = 0x8B5E,
    SAMPLER_3D = 0x8B5F
};

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

class GAME_ENGINE_API BasicTypes final
{
public:
    static const char* toChar( PrimitiveType inType );
    static const char* toChar( DataType inDataType );
    static std::size_t getSize( DataType inDataType );

protected:
private:
};

NAMESPACE_END( LOGLW )
