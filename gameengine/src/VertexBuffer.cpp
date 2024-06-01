#include "gameengine/VertexBuffer.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"
#include "gameengine/IRenderDevice.hpp"

using namespace LOGLW;

VertexBuffer::VertexBuffer( const VertexData& vertexData, IGameEngine* engine ) : IRenderable( engine )
{
    m_vertexData = vertexData;
    m_vertexData.VBO = this;
    loadData();

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::BUFFER, m_vertexData.VBO->getId(), newName );
            } );
    };
    if( m_vertexData.VAO )
    {
        setName( m_vertexData.VAO->getName() + "::vertex_buffer_" + CUL::String( getId() ) );
    }
    else
    {
        setName( "vertex_buffer_" + CUL::String( getId() ) );
    }
}

void VertexBuffer::loadData()
{
    release();

    m_bufferId = getDevice()->generateBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
    getDevice()->bufferData( m_bufferId, m_vertexData.vertices, LOGLW::BufferTypes::ARRAY_BUFFER );
    getDevice()->vertexAttribPointer( m_vertexData );

    if( m_vertexData.indices.size() )
    {
        m_indexBuffer = new IndexBuffer();
        m_indexBuffer->loadData( m_vertexData.indices );
    }
    else
    {
        // check the vertices count.
    }

    m_load = false;
}

void VertexBuffer::render()
{
    bind();
    if( m_indexBuffer.get() )
    {
        m_indexBuffer->bind();
        getDevice()->drawElements( m_vertexData.primitiveType, m_indexBuffer->getData() );
        //getDevice()->drawElementsFromLastBuffer( m_vertexData.primitiveType, DataType::UNSIGNED_INT, m_indexBuffer->getData().size() );
    }
    else
    {
        // TODO! need to check if there are actual trianiangles or other types.
        getDevice()->drawArrays( m_vertexData.VAO->getId(), m_vertexData.primitiveType, 0, 3 );
    }
}

unsigned VertexBuffer::getId() const
{
    return m_bufferId;
}

int VertexBuffer::getSize() const
{
    return 0;
}

void VertexBuffer::bind()
{
    getDevice()->bindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_bufferId );
}

VertexBuffer::~VertexBuffer()
{
    release();
}

void VertexBuffer::release()
{
    getDevice()->deleteBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_bufferId );
}
