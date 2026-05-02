#include "gameengine/Cube.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Primitives/Quad.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/VertexArray.hpp"
#include "IMPORT_rapidobj.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Cube::Cube( Camera* camera, bool forceLegacy ):
    IObject( "Cube", forceLegacy ),
    m_camera( camera ),
    m_engine( getEngine() )
{
    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );

    getVao()->setProgram( getProgram() );
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
        m_initialized = true;
    }
    else
    {
        m_engine.addPreRenderTask(
            [this]()
            {
                init();
                m_initialized = true;
                m_engine.addObjectToRender( this );
            } );
    }
}

void Cube::init()
{
    rapidobj::Result result = rapidobj::ParseFile( "res/cube.obj" );
    if( result.error )
    {
        const auto errorString = result.error.code.message();
        CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Error, "res/cube.obj reading object error: %s", errorString.c_str() );
    }

    struct Vertex
    {
        float pos[3];
        float uv[2];
        float nor[3];
    };

    const std::size_t dataSize = result.attributes.positions.size();
    std::vector<float> dataVec;
    std::size_t positionId{ 0u };
    std::size_t uvId{ 0u };
    std::size_t texId{ 0u };

    const auto& positions = result.attributes.positions;
    const std::size_t pointsCount = positions.size();
    VertexData vboData;
    vboData.Data.createFrom( positions.data(), pointsCount );
    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;
    vboData.Attributes.emplace_back( AttributeMeta( "pos", 0, 3, LOGLW::DataType::FLOAT, false, 0, nullptr ) );

    std::vector<std::uint32_t> indices;
    for( const auto& indice : result.shapes[0].mesh.indices )
    {
        indices.push_back( static_cast<std::uint32_t>( indice.position_index ) );
    }
    vboData.Indices.createFrom( indices );

    vboData.VAO = getVao()->getId();
    getVao()->addVertexBuffer( vboData );

    getProgram()->setName( "%s/program", *getName() );
    getProgram()->compileShader( EExecuteType::Now, "embedded_shaders/basic_color.frag" );
    getProgram()->compileShader( EExecuteType::Now, "embedded_shaders/basic_pos.vert" );
    getProgram()->link( EExecuteType::Now );
    constexpr float initialScale = 0.1f;
    getProgram()->validate();
    getTransform()->setScale( { initialScale, initialScale, initialScale } );
    getTransform()->setPositionToParent( { 2.f, 2.f, 1.f } );
}

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
}

void Cube::setName( const CUL::StringWr& name )
{
    IObject::setName( name );
    getVao()->setName( "%s/vao", *name );
    getProgram()->setName( "%s/program", *name );
}

void Cube::render()
{
    getProgram()->enable();
    setTransformationAndColor();
    getVao()->render();

    getProgram()->disable();
}

void Cube::setTransformationAndColor()
{
    const Camera& camera = getEngine().getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    getProgram()->setUniform( EExecuteType::Now, "projection", projectionMatrix );
    getProgram()->setUniform( EExecuteType::Now, "view", viewMatrix );
    getProgram()->setUniform( EExecuteType::Now, "model", model );
    getProgram()->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
}

Cube::~Cube()
{
    m_engine.removeObjectToRender( this );
    release();
}

void Cube::release()
{
}
