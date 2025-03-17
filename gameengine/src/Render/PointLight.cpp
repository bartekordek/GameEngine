#include "gameengine/Render/PointLight.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "CUL/Log/ILogger.hpp"
#include "RunOnRenderThread.hpp"
#include "IMPORT_rapidobj.hpp"

namespace LOGLW
{

PointLight::PointLight( IObject* parent, IGameEngine* engine ):
    IObject( "PointLight", engine, false )
{
    m_transformComponent = getTransform();

    setParent( parent );
    getVao()->setProgram( getProgram() );
    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            init();
        } );
}

void PointLight::init()
{
    rapidobj::Result result = rapidobj::ParseFile( "res/sphere.obj" );
    if( result.error )
    {
        const auto errorString = result.error.code.message();
        CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Error, "res/sphere.obj reading object error: %s", errorString.c_str() );
    }

    const auto& positions = result.attributes.positions;
    const std::size_t pointsCount = positions.size();
    VertexData vboData;
    vboData.Data.createFrom( positions.data(), pointsCount );
    vboData.primitiveType = LOGLW::PrimitiveType::TRIANGLES;
    vboData.Attributes.emplace_back(
        AttributeMeta( "pos", 0, 3, LOGLW::DataType::FLOAT, false, 0, nullptr ) );

    std::vector<std::uint32_t> indices;
    for (const auto& indice : result.shapes[0].mesh.indices)
    {
        indices.push_back( static_cast<std::uint32_t>( indice.position_index ) );
    }
    vboData.Indices.createFrom( indices );

    vboData.VAO = getVao()->getId();
    getVao()->addVertexBuffer( vboData );

    getProgram()->setName( getName() + "::program" );
    CUL::String errorContent;
    getProgram()->compileShader( EExecuteType::Now, "embedded_shaders/basic_color.frag" );
    getProgram()->compileShader( EExecuteType::Now, "embedded_shaders/basic_pos.vert" );
    getProgram()->link( EExecuteType::Now );
    constexpr float initialScale = 0.1f;
    getProgram()->validate();
    getTransform()->setScale( { initialScale, initialScale, initialScale } );
    getTransform()->setPositionToParent( {2.f, 2.f, 1.f} );
}

void PointLight::render()
{
    getProgram()->enable();
    setTransformation();
    applyColor();
    getVao()->render();

    getProgram()->disable();
}

void PointLight::setTransformation()
{
    const Camera& camera = getEngine().getCamera();
    const glm::mat4 projectionMatrix = camera.getProjectionMatrix();
    const glm::mat4 viewMatrix = camera.getViewMatrix();

    const glm::mat4 model = m_transformComponent->getModel();

    getProgram()->setUniform( EExecuteType::Now, "projection", projectionMatrix );
    getProgram()->setUniform( EExecuteType::Now, "view", viewMatrix );
    getProgram()->setUniform( EExecuteType::Now, "model", model );
}

void PointLight::setColor( const CUL::Graphics::ColorS& color )
{
    m_color = color;
    glm::vec4 colorVec;
    colorVec.x = m_color.getRF();
    colorVec.y = m_color.getGF();
    colorVec.z = m_color.getBF();
    colorVec.w = m_color.getAF();
}

void PointLight::applyColor()
{
    getProgram()->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
}

PointLight::~PointLight()
{
}

}  // namespace LOGLW