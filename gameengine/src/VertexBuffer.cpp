#include "gameengine/VertexBuffer.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"
#include "gameengine/IRenderDevice.hpp"

using namespace LOGLW;

VertexBuffer::VertexBuffer( const VertexData& vertexData, IGameEngine* engine ) : IRenderable( engine )
{
    setVertexData( vertexData );
    setName( "vertex_buffer_" + CUL::String( getId() ) );
}

void VertexBuffer::setVertexData( const VertexData& vertexData )
{
    m_vertexData = vertexData;
    loadData();

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::BUFFER, m_vertexData.VBO, newName );
            } );
    };
}

void VertexBuffer::loadData()
{
    release();
    m_vertexData.VBO = getDevice()->generateBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
    updateVertexData();
    getDevice()->vertexAttribPointer( m_vertexData );

    if( m_vertexData.Indices.getIsEmpty() == false )
    {
        m_indexBuffer = new IndexBuffer();
        m_indexBuffer->loadData( m_vertexData.Indices );
    }
    else
    {
        // check the vertices count.
    }

    m_load = false;
}

void VertexBuffer::updateVertexData( const VertexData& vertexData )
{
    m_vertexData = vertexData;
    updateVertexData();
}

void VertexBuffer::updateVertexData()
{
    getDevice()->bufferData( m_vertexData.VBO, m_vertexData.Data, LOGLW::BufferTypes::ARRAY_BUFFER );
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
        getDevice()->drawArrays( m_vertexData.VAO, m_vertexData.primitiveType, 0, 3 );
    }
}

unsigned VertexBuffer::getId() const
{
    return m_vertexData.VBO;
}

int VertexBuffer::getSize() const
{
    return 0;
}

void VertexBuffer::bind()
{
    getDevice()->bindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_vertexData.VBO );
}

const VertexData& VertexBuffer::getData() const
{
    return m_vertexData;
}

VertexData& VertexBuffer::getData()
{
    return m_vertexData;
}

VertexBuffer::~VertexBuffer()
{
    release();
}

void VertexBuffer::release()
{
    getDevice()->deleteBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_vertexData.VBO );
}
