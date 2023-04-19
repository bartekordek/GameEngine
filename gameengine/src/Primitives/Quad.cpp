#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Program.hpp"

#include "CUL/IMPORT_GLM.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Quad::Quad( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy ) : IObject( "Quad", &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    constexpr float size = 2.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );

    m_shape.data[0] = { size, size, 0.f };
    m_shape.data[1] = { size, 0.f, 0.f };
    m_shape.data[2] = { 0.f, 0.f, 0.f };
    m_shape.data[3] = { 0.f, size, 0.f };


    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    VertexBufferData vboData;
    vboData.indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    vboData.vertices = m_shape.toVectorOfFloat();

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

    const auto fragmentShader = getEngine().createShader( "embedded_shaders/basic_color.frag", fragmentShaderSource );
    const auto vertexShader = getEngine().createShader( "embedded_shaders/basic_pos.vert", vertexShaderSource );

    m_shaderProgram->attachShader( vertexShader );
    m_shaderProgram->attachShader( fragmentShader );
    m_shaderProgram->link();
    m_shaderProgram->validate();

    m_shaderProgram->enable();
}

void Quad::render()
{
    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        getDevice()->draw( m_shape, m_transformComponent->getModel(), m_color );
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
    const Camera& camera = m_engine.getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

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
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
