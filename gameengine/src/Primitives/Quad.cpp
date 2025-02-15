#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Camera.hpp"
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

Quad::Quad( Camera& camera, IGameEngine& engine, IObject* parent, bool forceLegacy )
    : IObject( "Quad", &engine, forceLegacy ), m_camera( camera ), m_engine( engine )
{
    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    constexpr float size = 4.f;
    m_transformComponent->setSize( CUL::MATH::Point( size, size, 0.f ) );
    // TODO: add normals
    setSize( { size, size, 0 } );
    createProgram();

    IName::AfterNameChangeCallback = [this]( const CUL::String& /*newName*/ )
    {
        RunOnRenderThread::getInstance().Run(
            [this]()
            {
                m_vao->setName( getName() + "::vao" );
            } );
    };

    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            init();
        } );
    m_transformComponent->changeSizeDelegate.addDelegate(
        []()
        {
            //m_recreateBuffers = true;
        } );


    IName::AfterNameChangeCallback = [this]( const CUL::String& /*newName*/ )
    {
        RunOnRenderThread::getInstance().Run(
            [this ]()
            {
                getProgram()->setName( getName() + "::shader_program" );
                m_vao->setName( getName() + "::vao" );
            } );
    };
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
    const auto size = m_transformComponent->getSize();

    setSize( size.toGlmVec() );

    VertexData vboData;
    std::vector<std::uint32_t> indices = {
        // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    vboData.Indices.createFrom( indices );

    vboData.Data.createFrom( m_shape.toVectorOfFloat() );
    vboData.Attributes.emplace_back( AttributeMeta( "pos", 0, 3, LOGLW::DataType::FLOAT, false, (int)CUL::MATH::Primitives::Quad::getStride(), nullptr ) );
    vboData.Attributes.emplace_back( AttributeMeta( "nor", 1, 3, LOGLW::DataType::FLOAT, false, (int)CUL::MATH::Primitives::Quad::getStride(), reinterpret_cast<void*>( 3 * sizeof( float ) ) ) );

    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;

    m_vao = m_engine.createVAO();
    setName( "quad_" + CUL::String( getId() ) );
    m_vao->setDisableRenderOnMyOwn( true );
    vboData.VAO = m_vao->getId();

    m_vao->addVertexBuffer( vboData );
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
    getProgram()->setName( getName() + "::program" );

    CUL::String errorContent;
    getProgram()->compileShader( "embedded_shaders/basic_color.frag" );
    getProgram()->compileShader( "embedded_shaders/basic_pos.vert" );
    getProgram()->link();
    getProgram()->validate();
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
        if( m_recreateBuffers )
        {
            deleteBuffers();
            createBuffers();
            m_recreateBuffers = false;
        }

        getProgram()->enable();
        setTransformation();
        applyColor();
        m_vao->render();

        getProgram()->disable();
    }
}

void Quad::setTransformation()
{
    const Camera& camera = m_engine.getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    getProgram()->setUniform( "projection", projectionMatrix );
    getProgram()->setUniform( "view", viewMatrix );
    getProgram()->setUniform( "model", model );
}

void Quad::applyColor()
{
    getProgram()->setUniform( "color", m_color.getVec4() );
}

Quad::~Quad()
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

void Quad::release()
{
    deleteBuffers();
}

void Quad::deleteBuffers()
{
    delete m_vao;
    m_vao = nullptr;
}
