#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Program.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Quad::Quad( Camera& camera, IGameEngine& engine, IObject* parent ) : IObject( &engine ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( CUL::MATH::Point( 2.f, 2.f, 2.f ) );

    if( getDevice()->getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        engine.pushPreRenderTask( [this]() {
            init();
        } );
    }

    m_transformComponent->changeSizeDelegate.addDelegate( [this]() {
        m_recreateBuffers = true;
    } );
}

void Quad::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;
    glm::vec4 colorVec;
    colorVec.x = m_color.getRF();
    colorVec.y = m_color.getGF();
    colorVec.z = m_color.getBF();
    colorVec.w = m_color.getAF();
}

void Quad::init()
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

void Quad::createBuffers()
{
    LOGLW::VertexBufferData vboData;
    vboData.indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    const CUL::MATH::Point& size = m_transformComponent->getSize();

    float x0 = 0.0f;
    float x1 = size.x();

    float y0 = 0.0f;
    float y1 = size.y();

    vboData.vertices = {
        x1, y1, 0.f,  // top right
        x1, y0, 0.f,  // bottom right
        x0, y0, 0.f,  // bottom left
        x0, y1, 0.f   // top left
    };

    vboData.containsColorData = false;
    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vao = m_engine.createVAO();
    m_vao->setDisableRenderOnMyOwn( true );

    m_vao->addVertexBuffer( vboData );
}

void Quad::createShaders()
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

void Quad::render()
{
    if( getDevice()->isLegacy() )
    {
        auto size = m_transformComponent->getSize();

        QuadCUL quad;
        quad[1][0] = size.x();

        quad[2][0] = size.x();
        quad[2][1] = size.y();

        quad[3][1] = size.y();

        getDevice()->matrixStackPush();

        const auto position = m_transformComponent->getWorldPosition();
        const auto rotation = m_transformComponent->getWorldRotation();

        getDevice()->translate( position );

        getDevice()->rotate( rotation );
        getDevice()->draw( quad, m_color );

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

void Quad::setTransformation()
{
    Camera& camera = m_engine.getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = m_transformComponent->getModel();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );
}

void Quad::applyColor()
{
    m_shaderProgram->setUniform( "color", m_color.getVec4() );
}

Quad::~Quad()
{
    if( getDevice()->getCUL()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        release();
    }
    else
    {
        m_engine.pushPreRenderTask( [this]() {
            release();
        } );
    }
}

void Quad::release()
{
    deleteBuffers();
    m_engine.removeProgram( m_shaderProgram );
    m_shaderProgram = nullptr;
}

void Quad::deleteBuffers()
{
    delete m_vao;
    m_vao = nullptr;
}