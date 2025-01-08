#include "gameengine/IndexBuffer.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "RunOnRenderThread.hpp"

using namespace LOGLW;

IndexBuffer::IndexBuffer()
{
    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::BUFFER, m_id, newName );
            } );
    };
   
}

void IndexBuffer::bind()
{
    getDevice()->bindBuffer(BufferTypes::ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::loadData( const CUL::DataWrapper& inData )
{
    m_data = inData;

    m_id = getDevice()->generateAndBindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );
    setName( "index_buffer_" + CUL::String( getId() ) );

    //TODO: find if size is matching.
    //auto indicesSize = sizeof( m_data[0] ) * m_data.size();

    getDevice()->bufferData( m_id, m_data, BufferTypes::ELEMENT_ARRAY_BUFFER );
}

const CUL::DataWrapper& IndexBuffer::getData() const
{
    return m_data;
}

IndexBuffer::~IndexBuffer()
{

}
