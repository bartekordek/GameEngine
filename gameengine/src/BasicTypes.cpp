#include "gameengine/BasicTypes.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

const char* BasicTypes::toChar( PrimitiveType inType )
{
    if( inType == PrimitiveType::LINE_STRIP )
    {
        return "Line Strip";
    }

    if( inType == PrimitiveType::TRIANGLES )
    {
        return "Triangles";
    }

    if( inType == PrimitiveType::TRIANGLE_STRIP )
    {
        return "Triangles Strip";
    }

    if( inType == PrimitiveType::TRIANGLE_FAN )
    {
        return "Triangles Fan";
    }

    if( inType == PrimitiveType::QUADS )
    {
        return "Quads";
    }

    if( inType == PrimitiveType::QUAD_STRIP )
    {
        return "Quad Strip";
    }

    return "None";
}

const char* BasicTypes::toChar( DataType inDataType )
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

std::size_t BasicTypes::getSize( DataType inDataType )
{
    switch( inDataType )
    {
        case DataType::BYTE:
        case DataType::UNSIGNED_BYTE:
            return 1u;
        case DataType::SHORT:
        case DataType::UNSIGNED_SHORT:
            return 2u;
        case DataType::INT:
        case DataType::UNSIGNED_INT:
        case DataType::FLOAT:
            return 4u;
        case DataType::DOUBLE:
            return 8u;
        case DataType::FLOAT_VEC2:
            return sizeof( float ) * 2u;
        case DataType::FLOAT_VEC3:
            return sizeof( float ) * 3u;
        case DataType::FLOAT_VEC4:
            CUL::Assert::check( false, "DataType::FLOAT_VEC4 not implemented yet." );
            return 0u;
        case DataType::FLOAT_MAT2:
            return sizeof( float ) * 4u;
        case DataType::FLOAT_MAT3:
            return sizeof( float ) * 9u;
        case DataType::FLOAT_MAT4:
            CUL::Assert::check( false, "DataType::FLOAT_MAT4 not implemented yet." );
            return 0u;
        case DataType::SAMPLER_1D:
            CUL::Assert::check( false, "DataType::SAMPLER_1D not implemented yet." );
            return 0u;
        case DataType::SAMPLER_2D:
            CUL::Assert::check( false, "DataType::SAMPLER_2D not implemented yet." );
            return 0u;
        case DataType::SAMPLER_3D:
            CUL::Assert::check( false, "DataType::SAMPLER_3D not implemented yet." );
            return 0u;
        case DataType::NONE:
            CUL::Assert::check( false, "DATA TYPE NONE!" );
        default:
            return 0u;
    }
}