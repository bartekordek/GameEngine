#include "gameengine/IObject.hpp"

#include "gameengine/Components/TransformComponent.hpp"

using namespace LOGLW;

IObject::IObject(IGameEngine* engine): IRenderable(engine)
{
    auto transofrmComponent = new TransformComponent(*this);
    addComponent( "TransformComponent", transofrmComponent );
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

IObject* IObject::getParent()
{
    return m_parent;
}

void IObject::setParent( IObject* parent )
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

IComponent* IObject::getComponent(const String& name)
{
    const auto it = m_components.find(name);
    if (it != m_components.end())
    {
        return it->second;
    }
    return nullptr;
}

void IObject::addComponent(const String& name, IComponent* component)
{
    m_components[name] = component;
}

TransformComponent* IObject::getTransform()
{
    return static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
}

IObject::~IObject()
{
    for( const auto& componentPair: m_components)
    {
        delete componentPair.second;
    }

    m_components.clear();

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