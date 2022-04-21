#include "ObjLoader.hpp"


#include "CUL/GenericUtils/SimpleAssert.hpp"

using namespace LOGLW;

ObjDef* ObjLoader::loadObj( const CUL::String& path )
{
    auto result = new ObjDef();

    std::string warn = result->warn.string();
    std::string error = result->error.string();
    CUL::Assert::simple( tinyobj::LoadObj(
        &result->attrib,
        &result->shapes,
        &result->materials,
        &warn,
        &error,
        path.cStr() ), "Cannot load obj: " + path );

    return result;
}
