#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Program.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

#include "CUL/Threading/ThreadUtils.hpp"

using namespace LOGLW;

Quad::Quad( Camera& camera, IGameEngine& engine, IObject* parent ) : IObject( &engine ), m_camera( camera ), m_engine( engine )
{
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    m_transformComponent->setSize( Pos( 2.f, 2.f, 2.f ) );

    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        engine.pushPreRenderTask( [this]() {
            init();
        } );
    }
}

void Quad::init()
{
    if( getUtility()->isLegacy() )
    {
    }
    else
    {
        LOGLW::VertexBufferData vboData;
        vboData.indices = {
            // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
        };

        const Pos& size = m_transformComponent->getSize();

        static float denominator = 2.f;

        float x0 = -size.x() / denominator;
        float x1 = size.x() / denominator;

        float y0 = -size.y() / denominator;
        float y1 = size.y() / denominator;

        // float z0 = -size.z() / denominator;

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

        m_shaderProgram = getEngine().createProgram();

        const std::string vertexShaderSource =
#include "embedded_shaders/basic_pos.vert"
            ;

        const std::string fragmentShaderSource =
#include "embedded_shaders/basic_color.frag"
            ;

        auto fragmentShaderFile = getEngine().getCul()->getFF()->createRegularFileRawPtr( "embedded_shaders/basic_color.frag" );
        fragmentShaderFile->loadFromString( fragmentShaderSource );
        auto fragmentShader = new Shader( getEngine(), fragmentShaderFile );

        auto vertexShaderCode = getEngine().getCul()->getFF()->createRegularFileRawPtr( "embedded_shaders/basic_pos.vert" );
        vertexShaderCode->loadFromString( vertexShaderSource );
        auto vertexShader = new Shader( getEngine(), vertexShaderCode );

        m_shaderProgram->attachShader( vertexShader );
        m_shaderProgram->attachShader( fragmentShader );
        m_shaderProgram->link();
        m_shaderProgram->validate();

        m_shaderProgram->enable();

        setTransformation();
    }
}

void Quad::render()
{
    if( getUtility()->isLegacy() )
    {
        QuadCUL quad;
        static float size = 1.f;
        quad[0][0] = -size;
        quad[0][1] = -size;

        quad[1][0] = -size;
        quad[1][1] =  size;

        quad[2][0] =  size;
        quad[2][1] =  size;

        quad[3][0] =  size;
        quad[3][1] = -size;

        ColorS color;
        color.setRF(1.f);

        getUtility()->matrixStackPush();

        const auto position = getTransform()->getWorldPosition();
        const auto rotation = getTransform()->getWorldRotation();

        getUtility()->translate( position );

        //static const auto type = CUL::MATH::Angle::Type::DEGREE;
        getUtility()->rotate( rotation );
        getUtility()->draw( quad, color );

        getUtility()->matrixStackPop();
    }
    else
    {
        m_shaderProgram->enable();

        setTransformation();
        m_vao->render();

        m_shaderProgram->disable();
    }
}

void Quad::setTransformation()
{
    Camera& camera = m_engine.getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = getTransform()->getModel();

    m_shaderProgram->setAttrib( "projection", projectionMatrix );
    m_shaderProgram->setAttrib( "view", viewMatrix );
    m_shaderProgram->setAttrib( "model", model );
}

Quad::~Quad()
{
    if( getUtility()->getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    m_engine.removeObjectToRender(this);
    delete m_vao;
    m_vao = nullptr;
}