#include "gameengine/IObject.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "RunOnRenderThread.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "CUL/Log/ILogger.hpp"

using namespace LOGLW;

IObject::IObject( const CUL::String& name, IGameEngine* engine, bool forceLegacy )
    : IRenderable( engine ), m_engine( *engine ), m_forceLegacy( forceLegacy )
{
    m_transform = new TransformComponent( this );
    addComponent( "TransformComponent", m_transform );
    setName( name );
    setObject( this );

    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            createVao();
        } );
}

void IObject::createVao()
{
    m_vao = m_engine.createVAO();
}

// Dummy
const std::vector<float> IObject::getVertices() const
{
    return std::vector<float>();
}

IObject* IObject::getOuter()
{
    IObject* outer = this;
    IObject* current = outer;
    while( current != nullptr )
    {
        current = current->m_parent;
        if( current )
        {
            outer = current;
        }
    }

    return outer;
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

void IObject::addChild( IObject* child )
{
    std::lock_guard<std::mutex> locker( m_childrenMtx );
    m_children.insert( child );
    child->addParent( this );
    child->setDisableRenderOnMyOwn( true );
}

IComponent* IObject::getComponent( const String& name )
{
    const auto it = m_components.find( name );
    if( it != m_components.end() )
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
    return m_transform;
}

void IObject::removeByParent( bool enable )
{
    m_removeByParent = enable;
}

void IObject::createProgram()
{
    m_shaderProgram = getEngine().createProgram();
}

ShaderProgram* IObject::getProgram()
{
    return m_shaderProgram;
}

VertexArray* IObject::getVao()
{
    return m_vao;
}

void IObject::setVao( VertexArray* inVao )
{
    m_vao = inVao;
}

IObject::~IObject()
{
    CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Info, "IObject::~IObject() [%s]", getName().cStr() );

    for( const auto& componentPair : m_components )
    {
        delete componentPair.second;
    }

    m_components.clear();

    if( m_parent && m_removeByParent == false )
    {
        if( m_parent )
        {
            m_parent->removeChild( this, false );
        }
    }

    std::lock_guard<std::mutex> locker( m_childrenMtx );

    for( IObject* child : m_children )
    {
        child->removeByParent( true );
        delete child;
    }
    m_children.clear();

    deleteVao();

    m_engine.removeProgram( m_shaderProgram );
    m_shaderProgram = nullptr;
}

void IObject::addParent( IObject* parent )
{
    m_parent = parent;
}

void IObject::removeChild( IObject* child, bool lock )
{
    auto removeChildImpl = [this]( IObject* child )
    {
        const auto it = m_children.find( child );
        if( it == m_children.end() )
        {
            CUL::Assert::simple( false, "Trying to remove already removed child." );
        }
        else
        {
            m_children.erase( it );
        }
    };

    if( lock )
    {
        std::lock_guard<std::mutex> locker( m_childrenMtx );
        removeChildImpl( child );
    }
    else
    {
        removeChildImpl( child );
    }
}

IGameEngine& IObject::getEngine()
{
    return m_engine;
}

bool IObject::getForceLegacy() const
{
    return m_forceLegacy;
}


void IObject::deleteVao()
{
    delete m_vao;
    m_vao = nullptr;
}
