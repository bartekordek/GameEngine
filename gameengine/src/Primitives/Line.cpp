#include "gameengine/Primitives/Line.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"

#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Line::Line( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy ) : IObject( "Line", &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
    m_vertexData = std::make_unique<VertexData>();

    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    const float size = 2.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, size ) );
    m_transformComponent->setPivot( { 0.f, 0.f, 0.f } );

    m_line.data[1] = { size, 0.f, 0.f };

    m_transformComponent->changeSizeDelegate.addDelegate( [this]() {
        m_recreateBuffers = true;
    } );

    if( getDevice() && CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        engine.addPreRenderTask(
            [this]()
            {
                init();
            } );
    }
}

void Line::init()
{
    if( getDevice()->isLegacy() )
    {
    }
    else
    {
        createBuffers();
        createShaders();

        setTransformation();
    }
}

void Line::createBuffers()
{
    m_vertexData->vertices = m_line.toVectorOfFloat();
    m_vertexData->primitiveType = LOGLW::PrimitiveType::LINE_STRIP;
    m_vertexData->Attributes.push_back( AttributeMeta( "pos", 0, 3, DataType::FLOAT, false, 3 * sizeof( float ), nullptr ) );

    m_vao = m_engine.createVAO();
    m_vertexData->VAO = m_vao;
    m_vao->setDisableRenderOnMyOwn( true );

    m_vao->addVertexBuffer( *m_vertexData.get() );
}

void Line::createShaders()
{
    m_shaderProgram = getEngine().createProgram();
    m_shaderProgram->setName( getName() + "::program" );

    m_shaderProgram->loadShader( "embedded_shaders/basic_color.frag" );
    m_shaderProgram->loadShader( "embedded_shaders/basic_pos.vert" );
}

void Line::render()
{
    if( getDevice()->isLegacy() )
    {
        getDevice()->matrixStackPush();

        const auto position = m_transformComponent->getPositionAbsolut();
        const auto rotation = m_transformComponent->getRotationAbsolute();

        getDevice()->translate( position );

        getDevice()->rotate( rotation );
        getDevice()->draw( m_line, m_color );

        getDevice()->matrixStackPop();
    }
    else
    {
        if( m_recreateBuffers )
        {
            deleteBuffers();
            createBuffers();
            m_recreateBuffers = false;
        }

        m_shaderProgram->enable();

        setTransformation();
        applyColor();
        m_vao->render();

        m_shaderProgram->disable();
    }
}

void Line::setTransformation()
{
    Camera& camera = m_engine.getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = m_transformComponent->getModel();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );
}

void Line::applyColor()
{
    m_shaderProgram->setUniform( "color", m_color.getVec4() );
}

void Line::setValues( const CUL::MATH::Primitives::Line& values )
{
    m_line = values;
}

void Line::setColor( const ColorS& color )
{
    m_color = color;
}

void Line::setLength( float length )
{
    m_transformComponent->setSize( { length, length, length } );
    m_line.data[1] = { length, 0.f, 0.f };
}

Line::~Line()
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        release();
    }
    else
    {
        m_engine.addPreRenderTask(
            [this]()
            {
                release();
            } );
    }
}

void Line::release()
{
    deleteBuffers();
    m_engine.removeProgram( m_shaderProgram );
    m_shaderProgram = nullptr;
}

void Line::deleteBuffers()
{
    delete m_vao;
    m_vao = nullptr;
}