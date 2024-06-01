#include "gameengine/Cube.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Program.hpp"
#include "RunOnRenderThread.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, IGameEngine* engine, IObject* parent, bool forceLegacy )
    : IObject( "Cube", engine, forceLegacy ), m_camera( camera ), m_engine( *engine )
{
    m_transformComponent = getTransform();
    m_vertexData = std::make_unique<VertexData>();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    constexpr float size = 4.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );
    // TODO: add normals
    setSize( { size, size, 0 } );

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName ) {
        RunOnRenderThread::getInstance().Run( [this, newName]() {
            m_vertexData->VAO->setName( getName() + "::vao" );
        } );
    };

    RunOnRenderThread::getInstance().Run( [this]() {
        init();
    } );
    m_transformComponent->changeSizeDelegate.addDelegate( [this]() {
        m_recreateBuffers = true;
    } );

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName ) {
        RunOnRenderThread::getInstance().Run( [this, newName]() {
            m_shaderProgram->setName( getName() + "::shader_program" );
            m_vertexData->VAO->setName( getName() + "::vao" );
        } );
    };
}

void Cube::setSize( const glm::vec3& size )
{
    //m_shape.data[0] = { size.x, size.y, 0.f, 0.f, 0.f, 1.f };
    //m_shape.data[1] = { size.x, 0.f, 0.f, 0.f, 0.f, 1.f };
    //m_shape.data[2] = { 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
    //m_shape.data[3] = { 0.f, size.y, 0.f, 0.f, 0.f, 1.f };
}

void Cube::init()
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

void Cube::setTransformation()
{
    const Camera& camera = m_engine.getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );
}

// void Cube::setImage(unsigned wallIndex, const CUL::FS::Path& imagePath, CUL::Graphics::IImageLoader* imageLoader)
void Cube::setImage( unsigned, const CUL::FS::Path&, CUL::Graphics::IImageLoader* )
{
}

void Cube::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;

    if( !m_initialized )
    {
        return;
    }

    std::lock_guard<std::mutex> renderLock( m_renderMutex );

}

void Cube::createBuffers()
{
    const TransformComponent::Pos cubeSize( 2.f, 2.f, 2.f );
    const TransformComponent::Pos quadSize( 2.f, 2.f, 0.f );

    m_transformComponent->setSize( cubeSize );
    m_transformComponent->setPivot( { 0.0f, 0.f, 0.f } );

    TransformComponent::Pos pivot = { 0.5f, 0.5f, 0.f };

    m_vertexData->VAO = getEngine().createVAO();
    m_vertexData->VAO->setName( "cube::vao" );
    m_vertexData->VAO->setDisableRenderOnMyOwn( true );
    m_vertexData->VAO->enableVertexAttributeArray( 0 );
    m_vertexData->VAO->enableVertexAttributeArray( 1 );

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    const auto verticesElementsCount = sizeof( vertices ) / sizeof( vertices[0] );
    m_vertexData->vertices.reserve( verticesElementsCount );
    m_vertexData->vertices.insert( m_vertexData->vertices.end(), &vertices[0], &vertices[verticesElementsCount] );

    const auto stride = 6 * sizeof( float );
    const auto dType = LOGLW::DataType::FLOAT;
    m_vertexData->Attributes.emplace_back( AttributeMeta( "aPos", 0, 3, dType, false, stride, nullptr ) );
    m_vertexData->Attributes.emplace_back( AttributeMeta( "aNormal", 1, 3, dType, false, stride,
                                                    reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

    m_vertexData->VBO = getEngine().createVBO( *m_vertexData.get() );
    m_vertexData->VBO->setDisableRenderOnMyOwn( true );
    m_vertexData->VAO->addVertexBuffer( *m_vertexData );

    getDevice()->bufferData(
        m_vertexData->VBO->getId(),
        m_vertexData->vertices,
        BufferTypes::ARRAY_BUFFER );
}

void Cube::createShaders()
{
    m_shaderProgram = getEngine().createProgram();
    m_shaderProgram->setName( getName() + "::program" );

    const std::string vertexShaderSource =
#include "embedded_shaders/cube.vert"
        ;

    const std::string fragmentShaderSource =
#include "embedded_shaders/cube.frag"
        ;

    const auto fragmentShader = getEngine().createShader( "embedded_shaders/cube.frag", fragmentShaderSource );
    const auto vertexShader = getEngine().createShader( "embedded_shaders/cube.vert", vertexShaderSource );

    m_shaderProgram->attachShader( vertexShader );
    m_shaderProgram->attachShader( fragmentShader );
    m_shaderProgram->link();
    m_shaderProgram->validate();

    m_shaderProgram->enable();
}

void Cube::render()
{
    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        //getDevice()->draw( m_shape, m_transformComponent->getModel(), m_color );
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
        m_shaderProgram->goThroughTasks();
        setTransformation();
        //applyColor();
        m_vertexData->VAO->render();

        m_shaderProgram->disable();
    }
}

Cube::~Cube()
{
    deleteBuffers();
    m_engine.removeObjectToRender( this );
    release();
}

void Cube::deleteBuffers()
{
    delete m_vertexData->VAO;
    m_vertexData->VAO = nullptr;
}

void Cube::release()
{

}