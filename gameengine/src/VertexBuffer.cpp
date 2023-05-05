#include "gameengine/VertexBuffer.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/AttributeMeta.hpp"

using namespace LOGLW;
#include "gameengine/IRenderDevice.hpp"

VertexBuffer::VertexBuffer( const VertexData& vertexData, IGameEngine* engine )
    : IRenderable( engine )
{
    m_vertexData = vertexData;
    loadData();
}

void VertexBuffer::loadData()
{
    release();

    m_vertexData.VBO = getDevice()->generateBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
    getDevice()->bufferData( m_vertexData.VBO, m_vertexData.vertices, LOGLW::BufferTypes::ARRAY_BUFFER );
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

VertexBuffer::~VertexBuffer()
{
    release();
}

void VertexBuffer::release()
{
    getDevice()->deleteBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_vertexData.VBO );
}
