#include "gameengine/VertexBuffer.hpp"
#include "gameengine/VertexArray.hpp"

using namespace LOGLW;
#include "gameengine/IUtility.hpp"

VertexBuffer::VertexBuffer( VertexBufferData& vertexData, IGameEngine* engine )
    : IRenderable( engine ), m_vertexData( std::move( vertexData ) )
{
    loadData();
}

void VertexBuffer::loadData()
{
    release();

    if( m_vertexData.vao )
    {
        m_vertexData.vao->bind();
    }

    m_vboId = getUtility()->generateBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
    getUtility()->bufferData( m_vboId, m_vertexData.vertices, LOGLW::BufferTypes::ARRAY_BUFFER );

    unsigned attribIndex = 0;
    unsigned numberOfComponents = 3;
    //int stride = 0;  // 3 * sizeof( m_vertices.at( 0 ) );

    VertexAttributePtrMeta meta;
    if( m_vertexData.containsColorData )
    {
        if( m_vertexData.containsTextureCoords )
        {
            meta.componentsPerVertexAttribute = 3;
            meta.dataType = DataType::FLOAT;
            meta.normalized = false;
            meta.stride = 8 * sizeof( float );
            meta.vao = 0;
            meta.vbo = m_vboId;
            meta.vertexAttributeId = attribIndex++;

            // position attribute
            getUtility()->vertexAttribPointer( meta );
            getUtility()->enableVertexAttribArray( attribIndex );
            ++attribIndex;

            // color attribute
            meta.offset = (void*) (3 * sizeof( float ));
            meta.vertexAttributeId = attribIndex++;
            getUtility()->vertexAttribPointer( meta );
            getUtility()->enableVertexAttribArray( attribIndex );
            ++attribIndex;

            // texture coord attribute
            meta.offset = (void*)( 6 * sizeof( float ) );
            meta.vertexAttributeId = attribIndex++;
            getUtility()->vertexAttribPointer( meta );
            getUtility()->enableVertexAttribArray( attribIndex );

            //// position attribute
            //glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void* ) 0 );
            //glEnableVertexAttribArray( 0 );
            //// color attribute
            //glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void* ) ( 3 * sizeof( float ) ) );
            //glEnableVertexAttribArray( 1 );
            //// texture coord attribute
            //glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof( float ), ( void* ) ( 6 * sizeof( float ) ) );
            //glEnableVertexAttribArray( 2 );
        }
        else
        {
            if( m_vertexData.vao )
            {
                meta.componentsPerVertexAttribute = numberOfComponents;
                meta.dataType = DataType::FLOAT;
                meta.normalized = false;
                meta.stride = 6 * sizeof( float );
                meta.vao = m_vertexData.vao->getId();
                meta.vbo = m_vboId;
                meta.vertexAttributeId = attribIndex;

                getUtility()->vertexAttribPointer( meta );
                getUtility()->enableVertexAttribArray( attribIndex++ );

                meta.vertexAttributeId = attribIndex;
                meta.offset = (void*)( 3 * sizeof( float ) );
                getUtility()->vertexAttribPointer( meta );
                getUtility()->enableVertexAttribArray( attribIndex++ );
            }

        }
    }
    else
    {
        meta.componentsPerVertexAttribute = numberOfComponents;
        meta.dataType = DataType::FLOAT;
        meta.normalized = false;
        meta.stride = 0;
        if( m_vertexData.vao )
        {
            meta.vao = m_vertexData.vao->getId();
        }
        else
        {
            meta.vao = 0;
        }

        meta.vbo = m_vboId;
        meta.vertexAttributeId = attribIndex++;
        getUtility()->vertexAttribPointer( meta );
        getUtility()->enableVertexAttribArray( meta.vertexAttributeId );
    }

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
        getUtility()->drawElements( m_vertexData.primitiveType, m_indexBuffer->getData() );
        //getUtility()->drawElementsFromLastBuffer( m_vertexData.primitiveType, DataType::UNSIGNED_INT, m_indexBuffer->getData().size() );
    }
    else
    {
        // TODO! need to check if there are actual trianiangles or other types.
        getUtility()->drawArrays( m_vertexData.vao->getId(), m_vertexData.primitiveType, 0, 3 );
    }
}

unsigned VertexBuffer::getId() const
{
    return m_vboId;
}

int VertexBuffer::getSize() const
{
    return 0;
}

void VertexBuffer::bind()
{
    getUtility()->bindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_vboId );
}

VertexBuffer::~VertexBuffer()
{
    release();
}

void VertexBuffer::release()
{
    getUtility()->deleteBuffer( LOGLW::BufferTypes::ARRAY_BUFFER, m_vboId );
}
