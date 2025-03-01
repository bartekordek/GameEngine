#include "gameengine/IndexBuffer.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "RunOnRenderThread.hpp"

using namespace LOGLW;

IndexBuffer::IndexBuffer()
{
    init();
}

void IndexBuffer::onNameChange( const String& newName )
{
    IName::onNameChange( newName );

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this, newName]()
        {
            getDevice()->setObjectName( EObjectType::BUFFER, m_id, newName );
        } );
}

void IndexBuffer::init()
{
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            m_id = getDevice()->generateBuffer( BufferTypes::ELEMENT_ARRAY_BUFFER );
        } );
}

void IndexBuffer::bind()
{
    getDevice()->bindBuffer(BufferTypes::ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::loadData( const CUL::DataWrapper& inData )
{
    m_data = inData;

    bind();

    //TODO: find if size is matching.
    //auto indicesSize = sizeof( m_data[0] ) * m_data.size();

    getDevice()->bufferData( m_id, m_data, BufferTypes::ELEMENT_ARRAY_BUFFER );
}

std::uint32_t IndexBuffer::getObjID() const
{
    return m_id;
}

const CUL::DataWrapper& IndexBuffer::getData() const
{
    return m_data;
}

IndexBuffer::~IndexBuffer()
{

}