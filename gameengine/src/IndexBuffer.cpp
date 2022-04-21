#include "gameengine/IndexBuffer.hpp"

using namespace LOGLW;


IndexBuffer::IndexBuffer()
{

}

void IndexBuffer::bind()
{
    getUtility()->bindBuffer(BufferTypes::ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::loadData( DataType& data )
{
    m_data = std::move( data );

    m_id = getUtility()->generateAndBindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

    //TODO: find if size is matching.
    //auto indicesSize = sizeof( m_data[0] ) * m_data.size();

    getUtility()->bufferData( m_id, m_data, BufferTypes::ELEMENT_ARRAY_BUFFER );
}

const IndexBuffer::DataType& IndexBuffer::getData() const
{
    return m_data;
}

IndexBuffer::~IndexBuffer()
{

}