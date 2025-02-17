#include "gameengine/Primitives/Triangle.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IObject.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/AttributeMeta.hpp"

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

    m_shape.data[0] = { 0.0f, 0.0f, 0.0f };
    m_shape.data[1] = { size / 2.f, size, 0.f };
    m_shape.data[2] = { size, 0.f, 0.f };

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        engine.addPreRenderTask(
            [this]()
            {
                init();
            } );
    }

    m_transformComponent->changeSizeDelegate.addDelegate(
        [this]()
        {
            m_recreateBuffers = true;
        } );
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

GAME_ENGINE_API ShaderProgram* Triangle::getProgram() const
{
    return m_shaderProgram;
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

        setTransformation();
    }
}

void Triangle::setName( const CUL::String& name )
{
    IObject::setName( name );
    if( m_vao )
    {
        m_vao->setName( getName() + "::vao" );
    }
    if( m_shaderProgram )
    {
        m_shaderProgram->setName( getName() + "::shader_program" );
    }
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

    m_vao = m_engine.createVAO();
    m_vao->setDisableRenderOnMyOwn( true );
    vboData.VAO = m_vao->getId();

    m_vao->addVertexBuffer( vboData );
}

void Triangle::setSize( const glm::vec3& )
{

}

void Triangle::createShaders()
{
    m_shaderProgram = getEngine().createProgram();
    m_shaderProgram->setName( getName() + "::program" );

    m_shaderProgram->compileShader( "embedded_shaders/basic_color.frag" );
    m_shaderProgram->compileShader( "embedded_shaders/basic_pos.vert" );
    m_shaderProgram->link();
    m_shaderProgram->validate();
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

        m_shaderProgram->enable();

        setTransformation();
        applyColor();
        m_vao->render();

        m_shaderProgram->disable();
    }
}

void Triangle::setTransformation()
{
    const Camera& camera = m_engine.getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    m_shaderProgram->setUniform( "projection", projectionMatrix );
    m_shaderProgram->setUniform( "view", viewMatrix );
    m_shaderProgram->setUniform( "model", model );
}

void Triangle::applyColor()
{
    m_shaderProgram->setUniform( "color", m_color.getVec4() );
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
    m_engine.removeProgram( m_shaderProgram );
    m_shaderProgram = nullptr;
}

void Triangle::deleteBuffers()
{
    delete m_vao;
    m_vao = nullptr;
}
