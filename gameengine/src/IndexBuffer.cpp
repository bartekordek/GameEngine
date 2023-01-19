#include "gameengine/IndexBuffer.hpp"
#include "gameengine/IRenderDevice.hpp"

using namespace LOGLW;


IndexBuffer::IndexBuffer()
{

}

void IndexBuffer::bind()
{
    getDevice()->bindBuffer(BufferTypes::ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::loadData( std::vector<unsigned>& data )
{
    m_data = std::move( data );

    m_id = getDevice()->generateAndBindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

    //TODO: find if size is matching.
    //auto indicesSize = sizeof( m_data[0] ) * m_data.size();

    getDevice()->bufferData( m_id, m_data, BufferTypes::ELEMENT_ARRAY_BUFFER );
}

const std::vector<unsigned>& IndexBuffer::getData() const
{
    return m_data;
}

IndexBuffer::~IndexBuffer()
{

}