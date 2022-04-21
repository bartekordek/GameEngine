#pragma once

#include "gameengine/Import.hpp"

#include "CUL/String.hpp"

#include "tiny_obj_loader.h"

NAMESPACE_BEGIN( LOGLW )

struct ObjDef
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    CUL::String warn;
    CUL::String error;
};

class ObjLoader
{
public:
static ObjDef* loadObj( const CUL::String& path );
};


NAMESPACE_END( LOGLW )