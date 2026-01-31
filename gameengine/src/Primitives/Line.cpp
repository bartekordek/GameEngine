#include "gameengine/Primitives/Line.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/ExecuteType.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/VertexArray.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"

#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

Line::Line( Camera& camera, IObject* parent, bool forceLegacy ):
    IObject( "Line", forceLegacy ),
    m_camera( camera )
{
    m_vertexData = std::make_unique<VertexData>();

    m_transformComponent = getTransform();
    setParent( parent );

    m_transformComponent = static_cast<TransformComponent*>( getComponent( "TransformComponent" ) );
    const float size{ 2.f };
    m_transformComponent->setSize( CUL::MATH::Point( size, size, size ) );
    m_transformComponent->setPivot( { 0.f, 0.f, 0.f } );

    m_line.data[1][0] = size;
    m_line.data[1][1] = 0.f;
    m_line.data[1][2] = 0.f;

    m_transformComponent->changeSizeDelegate.addDelegate(
        [/*this*/]()
        {
        } );

    if( getDevice() && CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        init();
    }
    else
    {
        getEngine().addPreRenderTask(
            [this]()
            {
                init();
            } );
    }
}

void Line::init()
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

void Line::createBuffers()
{
    m_vertexData->Data.createFrom( m_line.toVectorOfFloat() );
    m_vertexData->primitiveType = LOGLW::PrimitiveType::LINE_STRIP;
    m_vertexData->Attributes.push_back( AttributeMeta( "pos", 0, 3, DataType::FLOAT, false, 3 * sizeof( float ), nullptr ) );

    m_vertexData->VAO = getVao()->getId();

    getVao()->addVertexBuffer( *m_vertexData );
}

void Line::createShaders()
{
    const String name = String::createFromPrintf( "%s/shader_program", getName().getUtfChar() );
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/basic_color.frag";
    sd.VertexShader = "embedded_shaders/basic_pos.vert";
    getProgram()->createFrom( EExecuteType::Now, sd );
    getVao()->setProgram( getProgram() );
}

void Line::render()
{
    if( getDevice()->isLegacy() )
    {
        getDevice()->matrixStackPush();

        const auto position = m_transformComponent->getPositionAbsolut();
        const auto rotation = m_transformComponent->getRotationAbsolute();

        getDevice()->translate( position );

        getDevice()->rotate( rotation );
        getDevice()->draw( m_line, m_color );

        getDevice()->matrixStackPop();
    }
    else
    {
        getProgram()->enable();

        setTransformation();
        applyColor();
        getVao()->render();

        getProgram()->disable();
    }
}

void Line::setTransformation()
{
    Camera& camera = getEngine().getCamera();
    auto projectionMatrix = camera.getProjectionMatrix();
    auto viewMatrix = camera.getViewMatrix();

    glm::mat4 model = m_transformComponent->getModel();

    getProgram()->setUniform( EExecuteType::Now, "projection", projectionMatrix );
    getProgram()->setUniform( EExecuteType::Now, "view", viewMatrix );
    getProgram()->setUniform( EExecuteType::Now, "model", model );
}

void Line::applyColor()
{
    getProgram()->setUniform( EExecuteType::Now, "color", m_color.getVec4() );
}

void Line::setValues( const CUL::MATH::Primitives::Line& values )
{
    m_line = values;
}

void Line::setColor( const ColorS& color )
{
    m_color = color;
}

void Line::setLength( float length )
{
    m_transformComponent->setSize( { length, length, length } );
    m_line.data[1][0] = length;
    m_line.data[1][1] = 0.f;
    m_line.data[1][2] = 0.f;
}

Line::~Line()
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

void Line::release()
{
    deleteBuffers();
}

void Line::deleteBuffers()
{
}
