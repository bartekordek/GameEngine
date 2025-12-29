#include "gameengine/VertexBuffer.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "CUL/Proifling/Profiler.hpp"

using namespace LOGLW;

VertexBuffer::VertexBuffer( const VertexData& vertexData ):
    m_vertexData( vertexData )
{
    init();
}

void VertexBuffer::init()
{
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            createVboBuffer();
            loadData();
        } );
}

void VertexBuffer::onNameChange( const String& newName )
{
    IName::onNameChange( newName );

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this, newName]()
        {
            getDevice()->setObjectName( EObjectType::BUFFER, m_vertexData.VBO, newName );

            if( m_indexBuffer )
            {
                constexpr std::size_t bufferSize{ 1024u };
                char buffer[bufferSize];
                snprintf( buffer, bufferSize, "%s/index_buffer", *newName );
                getDevice()->setObjectName( EObjectType::BUFFER, m_indexBuffer->getObjID(), buffer );
            }
        } );
}

void VertexBuffer::createVboBuffer()
{
    CUL::Assert::check( m_vertexData.VBO == 0u, "VERTEX DATA ALREADY CREATED!" );
    m_vertexData.VBO = getDevice()->generateBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
}

void VertexBuffer::setVertexData( const VertexData& vertexData )
{
    m_vertexData.Attributes = vertexData.Attributes;
    m_vertexData.Data = vertexData.Data;
    m_vertexData.Indices = vertexData.Indices;

    m_bufferTasks.addTask(
        [this]()
        {
            loadData();
        } );
}

void VertexBuffer::loadData()
{
    updateVertexData( true );
    getDevice()->vertexAttribPointer( m_vertexData );

    if( m_vertexData.Indices.getIsEmpty() == false )
    {
        if( m_indexBuffer == nullptr )
        {
            m_indexBuffer = std::make_unique<IndexBuffer>();
        }

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
    m_vertexData.Attributes = vertexData.Attributes;
    m_vertexData.Data = vertexData.Data;
    m_vertexData.Indices = vertexData.Indices;
    updateVertexData( false );
}

void VertexBuffer::updateVertexData( bool isRenderThread )
{
    if( isRenderThread )
    {
        getDevice()->bufferData( m_vertexData.VBO, m_vertexData.Data, LOGLW::BufferTypes::ARRAY_BUFFER );
    }
    else
    {
        RunOnRenderThread::getInstance().RunWaitForResult(
            [this]()
            {
                getDevice()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vertexData.VAO );
                getDevice()->bufferData( m_vertexData.VBO, m_vertexData.Data, LOGLW::BufferTypes::ARRAY_BUFFER );
            } );
    }
}

void VertexBuffer::render()
{
    ProfilerScope( "VertexBuffer::render" );

    bind();
    if( m_indexBuffer.get() )
    {
        m_indexBuffer->bind();
        getDevice()->drawElements( m_vertexData.primitiveType, m_indexBuffer->getData() );
        // getDevice()->drawElementsFromLastBuffer( m_vertexData.primitiveType, DataType::UNSIGNED_INT, m_indexBuffer->getData().size() );
    }
    else
    {
        // TODO! need to check if there are actual trianiangles or other types.
        getDevice()->drawArrays( m_vertexData.VAO, m_vertexData.primitiveType, 0,
                                 static_cast<std::uint32_t>( m_vertexData.Data.getSize() ) );
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
    m_bufferTasks.executeAll();
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
