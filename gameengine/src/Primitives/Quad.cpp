#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Program.hpp"

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

        //float z0 = -size.z() / denominator;

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
        m_vao->createShader( "source.vert" );
        m_vao->createShader( "yellow.frag" );

        m_vao->getProgram()->enable();
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
        m_vao->getProgram()->enable();
        setTransformation();
        m_vao->render();
        m_vao->getProgram()->disable();
    }
}

void Quad::setTransformation()
{
    Camera& camera = m_engine.getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = getTransform()->getModel();

    m_vao->getProgram()->setAttrib( "projection", projectionMatrix );
    m_vao->getProgram()->setAttrib( "view", viewMatrix );
    m_vao->getProgram()->setAttrib( "model", model );
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