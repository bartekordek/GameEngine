#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "RunOnRenderThread.hpp"

#include "CUL/IMPORT_GLM.hpp"

#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Triangle::Triangle( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy )
    : IObject( "Triangle", &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
   constexpr float size = 4.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );
    // TODO: add normals
    setSize( { size, size, 0 } );

    m_shape.data[0][0] = 0.f;
    m_shape.data[0][1] = 0.f;
    m_shape.data[0][2] = 0.f;

    m_shape.data[1][0] = size / 2.f;
    m_shape.data[1][1] = size;
    m_shape.data[1][2] = 0.f;

    m_shape.data[2][0] = size;
    m_shape.data[2][1] = 0.f;
    m_shape.data[2][2] = 0.f;

    RunOnRenderThread::getInstance().RunWaitForResult( [this](){
            init();
        } );

    m_transformComponent->changeSizeDelegate.addDelegate(
        [this]()
        {
            m_recreateBuffers = true;
        } );

    getVao()->setProgram( getProgram() );
}

void Triangle::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;
    glm::vec4 colorVec;
    colorVec.x = m_color.getRF();
    colorVec.y = m_color.getGF();
    colorVec.z = m_color.getBF();
    colorVec.w = m_color.getAF();
}

void Triangle::init()
{
    if( getDevice()->isLegacy() )
    {
    }
    else
    {
        createBuffers();
        createShaders();
        setColorAndTransformation();
    }
}

void Triangle::onNameChange( const String& newName )
{
    IObject::onNameChange( newName );
}

void Triangle::createBuffers()
{
    const auto size = m_transformComponent->getSize();

    setSize( size.toGlmVec() );

    VertexData vboData;
    std::vector<std::uint32_t> uintData = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    vboData.Indices.createFrom( uintData ); 

    vboData.Data.createFrom( m_shape.toVectorOfFloat() );
    vboData.Attributes.emplace_back(
        AttributeMeta( "pos", 0, 3, LOGLW::DataType::FLOAT, false, (int)CUL::MATH::Primitives::Triangle::getStride(), nullptr ) );
    vboData.Attributes.emplace_back(
        AttributeMeta( "nor", 1, 3, LOGLW::DataType::FLOAT, false, (int)CUL::MATH::Primitives::Triangle::getStride(), reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    vboData.VAO = getVao()->getId();

    getVao()->addVertexBuffer( vboData );
}

void Triangle::setSize( const glm::vec3& )
{

}

void Triangle::createShaders()
{
    auto shader = getProgram();
    shader->compileShader( EExecuteType::Now, "embedded_shaders/basic_color.frag" );
    shader->compileShader( EExecuteType::Now, "embedded_shaders/basic_pos.vert" );
    shader->link( EExecuteType::Now );
    shader->validate();
}

void Triangle::render()
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

        auto shader = getProgram();
        shader->enable();

        setColorAndTransformation();
        getVao()->render();

        if( m_unbindBuffersAfterDraw == true )
        {
            shader->disable();
        }
    }
}

void Triangle::setColorAndTransformation()
{
    const Camera& camera = m_engine.getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    auto shader = getProgram();
    shader->setUniform( EExecuteType::Now, "projection", projectionMatrix );
    shader->setUniform( EExecuteType::Now, "view", viewMatrix );
    shader->setUniform( EExecuteType::Now, "model", model );
    shader->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
}

Triangle::~Triangle()
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

void Triangle::release()
{
    deleteBuffers();
}

void Triangle::deleteBuffers()
{
}
