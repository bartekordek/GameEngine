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
    std::lock_guard<std::mutex> locker( m_childrenMtx );
    m_children.insert(child);
}


IObject::~IObject()
{
    std::lock_guard<std::mutex> locker( m_childrenMtx );
    if( m_parent )
    {
        m_parent->removeChild(this);
    }

    for( const IObject* child: m_children )
    {
        delete child;
    }
    m_children.clear();
}

void IObject::removeChild( IObject* child )
{
    std::lock_guard<std::mutex> locker( m_childrenMtx );
    auto it = m_children.find(child);
    if( it == m_children.end() )
    {
        CUL::Assert::simple( false, "Trying to remove already removed child." );
    }
    else
    {
        m_children.erase(it);
    }
}