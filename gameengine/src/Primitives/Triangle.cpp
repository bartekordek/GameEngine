#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Triangle::Triangle( Camera& camera, IGameEngine& engine, IObject* parent ) : IObject( &engine ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( CUL::MATH::Point( 2.f, 2.f, 2.f ) );

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    if( getUtility()->isLegacy() )
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
    vboData.indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    const CUL::MATH::Point& size = m_transformComponent->getSize();

    float x0 = size.x() / 2.f;
    float y0 = size.y();

    float x1 = size.x();
    float y1 = 0.f;

    float x2 = 0.f;
    float y2 = 0.f;

    vboData.vertices = {
        x0, y0, 0.f,  // top right
        x1, y1, 0.f,  // bottom right
        x2, y2, 0.f,  // bottom left
    };

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
    if( getUtility()->isLegacy() )
    {
        auto size = m_transformComponent->getSize();

        QuadCUL quad;
        quad[1][0] = size.x();

        quad[2][0] = size.x();
        quad[2][1] = size.y();

        quad[3][1] = size.y();

        getUtility()->matrixStackPush();

        const auto position = m_transformComponent->getWorldPosition();
        const auto rotation = m_transformComponent->getWorldRotation();

        getUtility()->translate( position );

        getUtility()->rotate( rotation );
        getUtility()->draw( quad, m_color );

        getUtility()->matrixStackPop();
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
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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