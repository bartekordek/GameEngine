#include "gameengine/IObject.hpp"

using namespace LOGLW;

IObject::IObject()
{
}

// Dummy
void IObject::addShader( const CUL::FS::Path&, IShaderFactory* )
{
}

// Dummy
const std::vector<float> IObject::getVertices() const
{
    return std::vector<float>();
}

void IObject::setParent(IObject* parent)
{
    m_parent = parent;
}

const std::set<IObject*>& IObject::getChildren() const
{
    return m_children;
}

void IObject::addChild(IObject* child)
{
    m_children.insert(child);
}


IObject::~IObject()
{
    for( const IObject* child: m_children )
    {
        delete child;
    }
    m_children.clear();
}