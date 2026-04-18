#include "gameengine/Primitives/Quad.hpp"
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
#include "CUL/Proifling/Profiler.hpp"

#include "CUL/IMPORT_GLM.hpp"

#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Quad::Quad( Camera& /*camera*/, IObject* parent, bool forceLegacy ):
    IObject( "", forceLegacy )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    constexpr float size = 4.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );
    // TODO: add normals
    setSize( { size, size, 0 } );

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            init();
        } );
    m_transformComponent->changeSizeDelegate.addDelegate(
        [this]()
        {
            updateBuffers();
        } );
}

void Quad::onNameChange( const String& newName )
{
    IObject::onNameChange( newName );
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
        getVao()->setProgram( getProgram() );

        createBuffers();
        createShaders();

        setTransformationAndColor();
    }
    setName( "quad_default" );
}

void Quad::createBuffers()
{
    updateBuffers();
}

void Quad::updateBuffers()
{
    /*RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            updateBuffers_impl();
        } );*/
    updateBuffers_impl();
}

void Quad::updateBuffers_impl()
{
    VertexArray* vao = getVao();

    const auto size = m_transformComponent->getSize();

    setSize( size.toGlmVec() );

    std::vector<std::uint32_t> indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    vao->addIndexData( indices );

    {
        std::vector<float> verts;
        for( const glm::vec3& pos : m_vertices )
        {
            verts.push_back( pos.x );
            verts.push_back( pos.y );
            verts.push_back( pos.z );
        }
        DataWrapper dw;
        dw.Data = verts;
        dw.Size = 3u;
        dw.Name = "pos";
        getVao()->updateData( dw );
    }
    {
        std::vector<float> norms;
        for( const glm::vec3& nor : m_normals )
        {
            norms.push_back( nor.x );
            norms.push_back( nor.y );
            norms.push_back( nor.z );
        }
        DataWrapper dw;
        dw.Data = norms;
        dw.Size = 3u;
        dw.Name = "nor";
        getVao()->updateData( dw );
    }
}

void Quad::setSize( const glm::vec3& size )
{
    m_vertices[0] = { size.x, size.y, 0.f };
    m_vertices[1] = { size.x,    0.f, 0.f };
    m_vertices[2] = { 0.f,       0.f, 0.f };
    m_vertices[3] = { 0.f,    size.y, 0.f };

    m_normals[0] = { 0.f, 0.f, 1.f };
    m_normals[1] = { 0.f, 0.f, 1.f };
    m_normals[2] = { 0.f, 0.f, 1.f };
    m_normals[3] = { 0.f, 0.f, 1.f };
}

void Quad::createShaders()
{
    String errorContent;
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/basic_color.frag";
    sd.VertexShader = "embedded_shaders/basic_pos.vert";

    getProgram()->createFrom( EExecuteType::WaitForCompletion, sd );
}

void Quad::render()
{
    ProfilerScope( "Quad::render" );

    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        CUL::Assert::check( false, "Legacy mode is not supported for Quad!" );
    }
    else
    {
        setTransformationAndColor();

        getVao()->render();

        if( m_unbindBuffersAfterDraw == true )
        {
            getProgram()->disable();
        }
    }
}

void Quad::setTransformationAndColor()
{
    ProfilerScope( "Quad::setTransformationAndColor" );

    const Camera& camera = getEngine().getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    ShaderProgram* shaderProgram = getProgram();
    shaderProgram->runOnRenderingThread(
        [this, shaderProgram, projectionMatrix, viewMatrix, model]()
        {
            shaderProgram->setUniform( EExecuteType::Now, "projection", projectionMatrix );
            shaderProgram->setUniform( EExecuteType::Now, "view", viewMatrix );
            shaderProgram->setUniform( EExecuteType::Now, "model", model );
            shaderProgram->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
        } );
}

Quad::~Quad()
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        release();
    }
    else
    {
        getEngine().addPreRenderTask(
            [this]()
            {
                release();
            } );
    }
}

void Quad::release()
{
}
