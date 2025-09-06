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
#include "CUL/IMPORT_tracy.hpp"

#include "CUL/IMPORT_GLM.hpp"

#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

constexpr const char* g_defaultQuadName =  "Quad" ;

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
    const auto size = m_transformComponent->getSize();

    setSize( size.toGlmVec() );

    std::vector<std::uint32_t> indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    m_vboData.Indices.createFrom( indices );

    m_vboData.Data.createFrom( m_shape.toVectorOfFloat() );

    m_vboData.Attributes.clear();

    AttributeMeta am;
    am.Name = "pos";
    am.Index = 0;
    am.Size = 3;
    am.Type = LOGLW::DataType::FLOAT;
    am.StrideBytes = (int)CUL::MATH::Primitives::Quad::getStride();
    m_vboData.Attributes.emplace_back( am );

    am.Name = "nor";
    am.Index = 1;
    am.Size = 3;
    am.Type = LOGLW::DataType::FLOAT;
    am.StrideBytes = (int)CUL::MATH::Primitives::Quad::getStride();
    am.DataOffset = reinterpret_cast<void*>( 3 * sizeof( float ) );
    m_vboData.Attributes.emplace_back( am );

    m_vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vboData.VAO = getVao()->getId();

    getVao()->updateVertexBuffer( m_vboData );
}

void Quad::setSize( const glm::vec3& size )
{
    m_shape.data[0] = { size.x, size.y, 0.f, 0.f, 0.f, 1.f };
    m_shape.data[1] = { size.x, 0.f,    0.f, 0.f, 0.f, 1.f };
    m_shape.data[2] = { 0.f,    0.f,    0.f, 0.f, 0.f, 1.f };
    m_shape.data[3] = { 0.f,    size.y, 0.f, 0.f, 0.f, 1.f };
}

void Quad::createShaders()
{
    CUL::String errorContent;
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/basic_color.frag";
    sd.VertexShader = "embedded_shaders/basic_pos.vert";

    getProgram()->createFrom( EExecuteType::WaitForCompletion, sd );
}

void Quad::render()
{
    ZoneScoped;
    if( getDevice()->isLegacy() || getForceLegacy() )
    {
        getDevice()->draw( m_shape, m_transformComponent->getModel(), m_color );
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
    ZoneScoped;
    const Camera& camera = getEngine().getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    ShaderProgram* shaderProgram = getProgram();
    shaderProgram->runOnRenderingThread(
        [this, shaderProgram, projectionMatrix, viewMatrix, model]()
        {
            shaderProgram->setUniform( EExecuteType::Now , "projection", projectionMatrix );
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
