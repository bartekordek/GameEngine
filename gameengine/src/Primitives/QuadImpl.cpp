#include "Primitives/QuadImpl.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Program.hpp"

#include "CUL/CULInterface.hpp"

using namespace LOGLW;

QuadImpl::QuadImpl( IGameEngine* engine ) : IQuad(engine)
{
    m_data[ 0 ] = m_data[ 1 ] = m_data[ 2 ] = m_data[ 3 ] = { 0.0f, 0.0f, 0.0f };

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( Pos( 2.f, 2.f, 2.f ) );
}

void QuadImpl::setValues( const QuadData& values )
{
    m_data = values;
}

void QuadImpl::init()
{
    m_shaderProgram = std::make_unique<Program>();
    m_shaderProgram->initialize();
    m_shaderProgram->enable();

    const std::string vertexShaderSource =
#include "embedded_shaders/camera.vert"
            ;

    const std::string fragmentShaderSource =
#include "embedded_shaders/camera.frag"
        ;
    auto cul = getUtility()->getCUl();
    auto fragmentShaderFile = cul->getFF()->createRegularFileRawPtr( "embedded_shaders/camera.frag" );
    fragmentShaderFile->loadFromString( fragmentShaderSource );
    auto fragmentShader = new Shader( fragmentShaderFile );

    auto vertexShaderCode = cul->getFF()->createRegularFileRawPtr( "embedded_shaders/camera.vert" );
    vertexShaderCode->loadFromString( vertexShaderSource );
    auto vertexShader = new Shader( vertexShaderCode );

    m_shaderProgram->attachShader( vertexShader );
    m_shaderProgram->attachShader( fragmentShader );
    m_shaderProgram->link();
    m_shaderProgram->validate();

    m_vao = getUtility()->generateBuffer( LOGLW::BufferTypes::VERTEX_ARRAY );
    getUtility()->bindBuffer( BufferTypes::VERTEX_ARRAY, m_vao );

    getUtility()->enableVertexAttribArray( 0 );
    getUtility()->enableVertexAttribArray( 1 );

    m_vbo = getUtility()->generateBuffer( BufferTypes::ARRAY_BUFFER );

    const Pos& size = m_transformComponent->getSize();
    float x0 = -size.x() / 2.f;
    float x1 = size.x() / 2.f;

    float y0 = -size.y() / 2.f;
    float y1 = size.y() / 2.f;

    float z0 = -size.z() / 2.f;

    std::array<std::array<float, 5>, 6> data;
    data[0] = { x0, y1, z0, 0.0f, 0.0f };
    data[1] = { x1, y1, z0, 1.0f, 0.0f };
    data[2] = { x1, y0, z0, 1.0f, 1.0f };
    data[3] = { x1, y0, z0, 1.0f, 1.0f };
    data[4] = { x0, y0, z0, 0.0f, 1.0f };
    data[5] = { x0, y1, z0, 0.0f, 0.0f };

    std::vector<float> dataVec;

    for( size_t i = 0; i < data.size(); ++i )
    {
        for( size_t j = 0; j < data[i].size(); ++j )
        {
            dataVec.push_back( data[i][j] );
        }
    }

    getUtility()->bufferData( m_vbo, dataVec, BufferTypes::ARRAY_BUFFER );

    VertexAttributePtrMeta meta;
    meta.vertexAttributeId = 0;
    meta.componentsPerVertexAttribute = 3;
    meta.vao = m_vao;
    meta.vbo = m_vbo;
    meta.dataType = DataType::FLOAT;
    meta.normalized = false;
    meta.stride = 5 * sizeof( float );

    getUtility()->vertexAttribPointer( meta );

    meta.vertexAttributeId = 1;
    meta.componentsPerVertexAttribute = 2;
    meta.offset = (void*)( 3 * sizeof( float ) );
    getUtility()->vertexAttribPointer( meta );

    getUtility()->unbindBuffer( LOGLW::BufferTypes::ARRAY_BUFFER );
    getUtility()->unbindBuffer( LOGLW::BufferTypes::ELEMENT_ARRAY_BUFFER );

    m_initialized = true;
}

void QuadImpl::render()
{
    if( !m_initialized )
    {
        init();
    }

    getUtility()->bindBuffer( BufferTypes::ARRAY_BUFFER, m_vao );
    getUtility()->bindBuffer( BufferTypes::ARRAY_BUFFER, m_vbo );

    getUtility()->drawArrays( m_vao, PrimitiveType::TRIANGLES, 0, 36 );

    getUtility()->bindBuffer( BufferTypes::ARRAY_BUFFER, 0 );
    getUtility()->bindBuffer( BufferTypes::VERTEX_ARRAY, 0 );
}

void QuadImpl::setColor(const QuadColors& colors)
{
    m_colors = colors;
}

void QuadImpl::setColor(const ColorS& color)
{
    for( auto& m_color: m_colors )
    {
        m_color = color;
    }
}

QuadImpl::~QuadImpl()
{

}