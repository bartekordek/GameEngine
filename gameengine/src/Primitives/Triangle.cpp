#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Triangle::Triangle( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy ) : IObject( "Triangle", &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    const float size = 2.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, size ) );

    m_triangleMath.vals[0] = { 0.0f, 0.0f, 0.0f };
    m_triangleMath.vals[1] = { size / 2.f, size, 0.f };
    m_triangleMath.vals[2] = { size, 0.f, 0.f };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        engine.pushPreRenderTask(
            [this]()
            {
                init();
            } );
    }
}

void Triangle::init()
{
    if( getDevice()->isLegacy() || getForceLegacy() )
    {
    }
    else
    {
        createBuffers();
        createShaders();

        setTransformation();
    }
}

void Triangle::createBuffers()
{
    LOGLW::VertexBufferData vboData;
    vboData.vertices = m_triangleMath.toVectorOfFloat();
    vboData.containsColorData = false;
    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vao = m_engine.createVAO();
    m_vao->setDisableRenderOnMyOwn( true );

    m_vao->addVertexBuffer( vboData );
}

void Triangle::createShaders()
{
    m_shaderProgram = getEngine().createProgram();

    const std::string vertexShaderSource =
#include "embedded_shaders/basic_pos.vert"
        ;

    const std::string fragmentShaderSource =
#include "embedded_shaders/basic_color.frag"
        ;

    auto fragmentShader = getEngine().createShader( "embedded_shaders/basic_color.frag", fragmentShaderSource );
    auto vertexShader = getEngine().createShader( "embedded_shaders/basic_pos.vert", vertexShaderSource );

    m_shaderProgram->attachShader( vertexShader );
    m_shaderProgram->attachShader( fragmentShader );
    m_shaderProgram->link();
    m_shaderProgram->validate();

    m_shaderProgram->enable();
}

void Triangle::render()
{
    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        getDevice()->draw( m_triangleMath, m_transformComponent->getModel(), m_color );
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

void Triangle::setTransformation()
{
    Camera& camera = m_engine.getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = m_transformComponent->getModel();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );
}

void Triangle::applyColor()
{
    m_shaderProgram->setUniform( "color", m_color.getVec4() );
}

void Triangle::setValues( const TriangleData& values )
{
    m_data = values;
}

void Triangle::setColor( const TriangleColors& colors )
{
    m_triangleColors = colors;
}

void Triangle::setColor( const ColorS& color )
{
    m_color = color;
}

Triangle::~Triangle()
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        release();
    }
    else
    {
        m_engine.pushPreRenderTask(
            [this]()
            {
                release();
            } );
    }
}

void Triangle::release()
{
    deleteBuffers();
    m_engine.removeProgram( m_shaderProgram );
    m_shaderProgram = nullptr;
}

void Triangle::deleteBuffers()
{
    delete m_vao;
    m_vao = nullptr;
}