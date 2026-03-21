#include <gameengine/Primitives/Sphere.hpp>
#include <gameengine/VertexArray.hpp>
#include "gameengine/AttributeMeta.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/Components/TransformComponent.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/ExecuteType.hpp"
#include "RunOnRenderThread.hpp"
#include <CUL/CULInterface.hpp>
#include <CUL/Log/ILogger.hpp>
#include <CUL/Math/Constants.hpp>
#include <CUL/STL_IMPORTS/STD_cmath.hpp>

using namespace LOGLW;

CSphere::CSphere( IObject* parent ):
    IObject( "Sphere", false )
{
    setParent( parent );
    init( 24, 72, 2.f );
}

void CSphere::init( std::int32_t inStackCount, std::int32_t inSectorCount, float inRadius )
{
    m_stackCount = inStackCount;
    m_sectorCount = inSectorCount;

    m_transformComponent = getTransform();
    m_transformComponent->setSize( { inRadius, 0.f, 0.f } );

    fillIndices();
    fillVerticesNormalsTex();

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this]()
        {
            init();
        } );
}

void CSphere::init()
{
    VertexArray* vao = getVao();
    vao->setProgram( getProgram() );
    CUL::DataWrapper dw;
    dw.createFrom( m_vertices );

    VertexData vd;
    vd.VAO = vao->getId();
    vd.Data = dw;
    vd.primitiveType = PrimitiveType::TRIANGLES;

    AttributeMeta am;
    am.Name = "pos";
    am.Index = 0;
    am.Size = 3;
    am.Type = LOGLW::DataType::FLOAT;
    am.StrideBytes = 3 * sizeof( float );
    vd.Attributes.push_back( am );

    CUL::DataWrapper indices;
    indices.createFrom( m_indices );

    vd.Indices = indices;
    m_verticesVbo = vao->addVertexBuffer( vd );

    String errorContent;
    ShaderProgram::ShadersData sd;
    sd.FragmentShader = "embedded_shaders/basic_color.frag";
    sd.VertexShader = "embedded_shaders/basic_pos.vert";

    getProgram()->createFrom( EExecuteType::WaitForCompletion, sd );
    m_color = CUL::Graphics::ColorE::RED;

    m_transformComponent->changeSizeDelegate.addDelegate(
        [this]()
        {
            applySizeChange();
        } );
    setName( "CSphere" );
}

void CSphere::applySizeChange()
{
    fillIndices();
    fillVerticesNormalsTex();
    VertexData wd = m_verticesVbo->getData();
    wd.Data.createFrom( m_vertices );
    getVao()->bind();
    m_verticesVbo->updateVertexData( wd );
}

void CSphere::fillIndices()
{
    m_indices.clear();
    m_lineIndices.clear();

    int k1, k2;
    for( int i = 0; i < m_stackCount; ++i )
    {
        k1 = i * ( m_sectorCount + 1 );  // beginning of current stack
        k2 = k1 + m_sectorCount + 1;     // beginning of next stack

        for( int j = 0; j < m_sectorCount; ++j, ++k1, ++k2 )
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if( i != 0 )
            {
                m_indices.push_back( k1 );
                m_indices.push_back( k2 );
                m_indices.push_back( k1 + 1 );
            }

            // k1+1 => k2 => k2+1
            if( i != ( m_stackCount - 1 ) )
            {
                m_indices.push_back( k1 + 1 );
                m_indices.push_back( k2 );
                m_indices.push_back( k2 + 1 );
            }

            // store m_indices for lines
            // vertical lines for all stacks, k1 => k2
            m_lineIndices.push_back( k1 );
            m_lineIndices.push_back( k2 );
            if( i != 0 )  // horizontal lines except 1st stack, k1 => k+1
            {
                m_lineIndices.push_back( k1 );
                m_lineIndices.push_back( k1 + 1 );
            }
        }
    }
}

void CSphere::fillVerticesNormalsTex()
{
    m_vertices.clear();
    m_normals.clear();
    m_texCoords.clear();

    CUL::CULInterface::getInstance()->getLogger()->logVariable( CUL::LOG::Severity::Warn, "CSphere::fillVerticesNormalsTex." );

    const float radius = m_transformComponent->getSize().x();

    float x, y, z, xy;                            // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;  // vertex normal
    float s, t;                                   // vertex texCoord

    float sectorStep = 2 * CUL::Math::PI_F / static_cast<float>( m_sectorCount );
    float stackStep = CUL::Math::PI_F / m_stackCount;
    float sectorAngle, stackAngle;

    for( int i = 0; i <= m_stackCount; ++i )
    {
        stackAngle = CUL::Math::PI_F / 2 - i * stackStep;  // starting from pi/2 to -pi/2
        xy = radius * std::cosf( stackAngle );             // r * cos(u)
        z = radius * std::sinf( stackAngle );              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for( int j = 0; j <= m_sectorCount; ++j )
        {
            sectorAngle = j * sectorStep;  // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * std::cosf( sectorAngle );  // r * cos(u) * cos(v)
            y = xy * std::sinf( sectorAngle );  // r * cos(u) * sin(v)
            m_vertices.push_back( x );
            m_vertices.push_back( y );
            m_vertices.push_back( z );

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            m_normals.push_back( nx );
            m_normals.push_back( ny );
            m_normals.push_back( nz );

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / m_sectorCount;
            t = (float)i / m_stackCount;
            m_texCoords.push_back( s );
            m_texCoords.push_back( t );
        }
    }
}

void CSphere::render()
{
    setTransformationAndColor();
    getVao()->render();
}

void CSphere::setTransformationAndColor()
{
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

CSphere::~CSphere()
{
}
