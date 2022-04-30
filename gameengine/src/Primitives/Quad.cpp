#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"

#include "CUL/ThreadUtils.hpp"

using namespace LOGLW;

Quad::Quad( Camera& camera, IGameEngine& engine, IObject* parent ) : m_camera( camera ), m_engine( engine )
{
    setParent( parent );

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

void Quad::render()
{
    if( getUtility()->isLegacy() )
    {
        QuadCUL quad; 
        quad[0][0] = -2.f;
        quad[0][1] = -2.f;

        quad[1][0] = -2.f;
        quad[1][1] =  2.f;

        quad[2][0] =  2.f;
        quad[2][1] =  2.f;

        quad[3][0] =  2.f;
        quad[3][1] = -2.f;
        
        ColorS color;
        color.setRF(1.f);


        getUtility()->draw( quad, color );
    }
    else
    {
        glm::mat4 model = glm::mat4( 1.0f );
        const Pos& position = getWorldPosition();
        glm::vec3 m_pos = position.toGlmVec();
        model = glm::translate( model, m_pos );

        m_vao->getProgram()->enable();
        setTransformation();
        m_vao->getProgram()->disable();
    }
}

void Quad::setSize( float width, float height )
{
    m_width = width;
    m_height = height;
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

        float x0 = -m_width / 2.f;
        float x1 = -1.f * x0;

        float y0 = -m_height / 2.f;
        float y1 = -y0;

        vboData.vertices = {
            x1, y1, 0.0f,  // top right
            x1, y0, 0.0f,  // bottom right
            x0, y0, 0.0f,  // bottom left
            x0, y1, 0.0f   // top left
        };

        vboData.containsColorData = false;
        vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

        m_vao = m_engine.createVAO();
        m_vao->addVertexBuffer( vboData );
        m_vao->createShader( "source.vert" );
        m_vao->createShader( "yellow.frag" );

        m_vao->getProgram()->enable();
        setTransformation();
    }

    m_engine.addObjectToRender(this);
}

void Quad::setTransformation()
{
    Camera* camera = m_engine.getCamera();
    auto projectionMatrix = camera->getProjectionMatrix();
    auto viewMatrix = camera->getViewMatrix();

    m_vao->getProgram()->setAttrib( "projection", projectionMatrix );
    m_vao->getProgram()->setAttrib( "view", viewMatrix );

    glm::mat4 model = getTransform()->getModel();

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
