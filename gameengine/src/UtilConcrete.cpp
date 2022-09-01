#include "UtilConcrete.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Viewport.hpp"

#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

#include "CUL/STL_IMPORTS/STD_iostream.hpp"
#include "CUL/STL_IMPORTS/STD_sstream.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

using namespace LOGLW;

static CUL::CULInterface* g_interface = nullptr;

GLenum glCheckError_( const char* file, int line )
{
    GLenum errorCode;
    bool hasError = false;
    while( ( errorCode = glGetError() ) != GL_NO_ERROR )
    {
        std::string error;
        switch( errorCode )
        {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }

        g_interface->getLogger()->log( CUL::String( file ) + ":" + CUL::String( line ) + " " + error );
        error = true;
    }

    CUL::Assert::simple( hasError == false, "OPENGL ERROR ^." );

    return errorCode;
}
#define glCheckError() glCheckError_( __FILE__, __LINE__ )

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
                             const void* userParam );

CUL::String enumToString( const GLenum val );
GLuint toGluint( unsigned value );

UtilConcrete::UtilConcrete( CUL::CULInterface* culInterface, bool forceLegacy ) : IUtility( culInterface, forceLegacy )
{
}

bool UtilConcrete::isLegacy()
{
    if( m_forceLegacy )
    {
        return true;
    }

    return getVersion().major < 2;
}

// TODO: Remove:
#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif
void UtilConcrete::setProjection( const Camera& )
{
}
void UtilConcrete::setViewport( const Viewport& viewport )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    glViewport( viewport.pos.getX(), viewport.pos.getY(), viewport.size.getWidth(), viewport.size.getHeight() );
}
#if _MSC_VER
#pragma warning( pop )
#endif

void UtilConcrete::setPerspective( const Angle& angle, double widthToHeightRatio, double m_zNear, double m_zFar )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    gluPerspective( angle.getValueD( CUL::MATH::Angle::Type::DEGREE ), widthToHeightRatio, m_zNear, m_zFar );
}
#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4189 )
#endif
void UtilConcrete::setPerspectiveProjection( const Camera& projectionData )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    auto fov = projectionData.getFov();
    auto ar = projectionData.getAspectRatio();
    auto zNear = projectionData.getZnear();
    auto zFar = projectionData.getZfar();
    gluPerspective( fov, ar, zNear, zFar );
}
#ifdef _MSC_VER
#pragma warning( pop )
#endif
void UtilConcrete::lookAt( const Camera& vp )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    const auto& eye = vp.getEye();
    const auto& center = vp.getCenter();
    const auto& up = vp.getUp();
    gluLookAt( eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z );
}

void UtilConcrete::lookAt( const std::array<Pos3Dd, 3>& vec )
{
    lookAt( vec[0], vec[1], vec[2] );
}

void UtilConcrete::lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    gluLookAt( eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z );
}

unsigned int UtilConcrete::createProgram()
{
    const auto programId = static_cast<unsigned int>( glCreateProgram() );
    log( "[UtilConcrete] glCreateProgram: " + String( programId ) );

    if( 0 == programId )
    {
        const GLenum err = glGetError();
        customAssert( GL_NO_ERROR == programId, "Error creating program, error numer: " + CUL::String( err ) );
        return 0;
    }

    return programId;
}

void UtilConcrete::removeProgram( unsigned programId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }


    static std::vector<int> released;
    log( "glDeleteProgram( " + String( programId ) + " );" );
    glDeleteProgram( toGluint( programId ) );
    released.push_back( programId );
    size_t size = released.size();
    for( size_t i = 0; i < size; ++i )
    {
        log( "deleted: " + String( released[i] ) );
    }
    // TODO: find a correct way to check whether program was deleted.
    //assertOnProgramError( programId, GL_DELETE_STATUS );
    glCheckError_("UtilConcrete.cpp", 180);
}

void UtilConcrete::linkProgram( unsigned programId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glLinkProgram( static_cast<GLuint>( programId ) );
    assertOnProgramError( programId, GL_LINK_STATUS );
}

void UtilConcrete::validateProgram( unsigned programId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "[UtilConcrete] glValidateProgram( " + String( programId ) + ");" );
    glValidateProgram( programId );
    assertOnProgramError( programId, GL_VALIDATE_STATUS );
}

unsigned int UtilConcrete::createShader( const IFile& shaderCode )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto shaderType = UtilConcrete::getShaderType( shaderCode.getPath().getExtension() );
    log( "[UtilConcrete] glCreateShader( " + String( static_cast<GLenum>( shaderType ) ) + ");" );
    const auto id = static_cast<unsigned int>( glCreateShader( static_cast<GLenum>( shaderType ) ) );

    auto codeLength = static_cast<GLint>( shaderCode.getLinesCount() );
    log( "[UtilConcrete] glCreateShader( " + String( static_cast<GLenum>( shaderType ) ) + ");" );
    glShaderSource( id, codeLength, shaderCode.getContent(), nullptr );
    glCompileShader( id );

    GLint compilationResult = 0;
    glGetShaderiv( id, GL_COMPILE_STATUS, &compilationResult );
    if( GL_FALSE == compilationResult )
    {
        GLchar eLog[1024] = { 0 };
        glGetShaderInfoLog( id, sizeof( eLog ), nullptr, eLog );
        auto errorAsString = std::string( eLog );
        CUL::String shaderCompilationErrorMessage = "Error compiling shader: " + errorAsString + "\n";
        shaderCompilationErrorMessage += "Shader Path: " + shaderCode.getPath().getPath() + "\n";
        customAssert( false, shaderCompilationErrorMessage );
    }

    return id;
}

void UtilConcrete::assertOnProgramError( unsigned programId, unsigned val )
{
    GLint result = 0;
    glGetProgramiv( programId, val, &result );
    if( GL_FALSE == result )
    {
        GLchar eLog[1024] = { 0 };
        glGetProgramInfoLog( programId, sizeof( eLog ), nullptr, eLog );
        CUL::String message = "Error on " + enumToString( val ) + std::string( eLog );
        customAssert( false, message );
    }
}

CUL::String enumToString( const GLenum val )
{
    switch( val )
    {
        case GL_COMPILE_STATUS:
            return "compile";
        case GL_LINK_STATUS:
            return "link";
        case GL_VALIDATE_STATUS:
            return "validate";
        case GL_DELETE_STATUS:
            return "delete";
        default:
            return "";
    }
}

ShaderTypes UtilConcrete::getShaderType( const CUL::String& fileExtension )
{
    /*
    .vert - a vertex shader
    .tesc - a tessellation control shader
    .tese - a tessellation evaluation shader
    .geom - a geometry shader
    .frag - a fragment shader
    .comp - a compute shader
    */
    if( fileExtension.equals( "frag" ) || fileExtension.equals( ".frag" ) )
    {
        return static_cast<ShaderTypes>( GL_FRAGMENT_SHADER );
    }
    else if( fileExtension.equals( "vert" ) || fileExtension.equals( ".vert" ) )
    {
        return static_cast<ShaderTypes>( GL_VERTEX_SHADER );
    }
    else if( fileExtension.equals( "geom" ) || fileExtension.equals( ".geom" ) )
    {
        return static_cast<ShaderTypes>( GL_GEOMETRY_SHADER );
    }

    return static_cast<ShaderTypes>( GL_INVALID_ENUM );
}

void UtilConcrete::attachShader( unsigned programId, unsigned shaderId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glAttachShader( toGluint( programId ), toGluint( shaderId ) );

    if( false )
    {
    }
    else
    {
        const GLenum err = glGetError();
        const GLubyte* errorAsString = gluErrorString( err );
        customAssert( GL_NO_ERROR == err, "Error creating program, error numer: " + CUL::String( errorAsString ) );
    }
}

void UtilConcrete::dettachShader( unsigned programId, unsigned shaderId )
{
    log( "glDetachShader( " + String( programId ) + ", " + String( shaderId ) + " );" );

    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDetachShader( toGluint( programId ), toGluint( shaderId ) );
    getLastOperationStatus();
}

void UtilConcrete::removeShader( unsigned shaderId )
{
    log( "glDeleteShader( " + String( shaderId ) + " );" );

    GLuint gshaderId = shaderId;

    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDeleteShader( gshaderId );
    getLastOperationStatus();
}

GLuint toGluint( unsigned value )
{
    return static_cast<GLuint>( value );
}

void UtilConcrete::setAttribValue( int attributeLocation, float value )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1f( " + String( attributeLocation ) + ", " + String( value  ) + " );" );
    glUniform1f( static_cast<GLfloat>( attributeLocation ), value );
}

void UtilConcrete::setAttribValue( int, int )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void UtilConcrete::setAttribValue( int, unsigned )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void UtilConcrete::setAttribValue( int, bool )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void UtilConcrete::setAttribValue( int, const CUL::String& )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void UtilConcrete::setUniformValue( int uniformLocation, float value )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1f( " + String( uniformLocation ) + ", " + String( value ) + " );" );
    glUniform1f( static_cast<GLfloat>( uniformLocation ), value );
}
void UtilConcrete::setUniformValue( int uniformLocation, int value )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1i( " + String( uniformLocation ) + ", " + String( value ) + " );" );
    glUniform1i( static_cast<GLint>( uniformLocation ), value );
}
void UtilConcrete::setUniformValue( int uniformLocation, unsigned value )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1i( " + String( uniformLocation ) + ", " + String( value ) + " );" );
    glUniform1i( static_cast<GLuint>( uniformLocation ), value );
}

void UtilConcrete::setProjectionAndModelToIdentity()
{
    resetMatrixToIdentity( MatrixTypes::PROJECTION );
    resetMatrixToIdentity( MatrixTypes::MODELVIEW );
}

void UtilConcrete::resetMatrixToIdentity( const MatrixTypes matrix )
{
    glMatrixMode( static_cast<GLenum>( matrix ) );
    glLoadIdentity();
}

void UtilConcrete::translate( const Point& point )
{
    translate( point[0], point[1], point[2] );
}

void UtilConcrete::translate( const float x, const float y, const float z )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glTranslatef( x, y, z );
}

void UtilConcrete::scale( const CUL::MATH::Vector3Df& scale )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glScalef( scale.getX(), scale.getY(), scale.getZ() );
}

void UtilConcrete::scale( const float scale )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glScalef( scale, scale, scale );
}

void UtilConcrete::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glColor4f( 1.f, 1.f, 1.f, 1.f );
    glBegin( GL_QUADS );
    glTexCoord3f( texQuad[0][0], texQuad[0][1], texQuad[0][2] );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glTexCoord3f( texQuad[1][0], texQuad[1][1], texQuad[1][2] );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glTexCoord3f( texQuad[2][0], texQuad[2][1], texQuad[2][2] );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glTexCoord3f( texQuad[3][0], texQuad[3][1], texQuad[3][2] );
    glVertex3f( quad[3][0], quad[3][1], quad[3][2] );
    glEnd();
}

void UtilConcrete::draw( const QuadCUL& quad, const ColorS& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_QUADS );
    auto red = color.getRF();
    auto green = color.getGF();
    auto blue = color.getBF();
    auto alpha = color.getAF();
    glColor4f( red, green, blue, alpha );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glVertex3f( quad[3][0], quad[3][1], quad[3][2] );
    glEnd();
}

void UtilConcrete::draw( const QuadData& quad, const ColorS& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_QUADS );
    auto red = color.getRF();
    auto green = color.getGF();
    auto blue = color.getBF();
    auto alpha = color.getAF();
    glColor4f( red, green, blue, alpha );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glVertex3f( quad[3][0], quad[3][1], quad[3][2] );
    glEnd();
}

void UtilConcrete::draw( const QuadData& quad, const QuadColors& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_QUADS );
    glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glColor4f( color[2].getRF(), color[2].getGF(), color[2].getBF(), color[2].getAF() );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glColor4f( color[3].getRF(), color[3].getGF(), color[3].getBF(), color[3].getAF() );
    glVertex3f( quad[3][0], quad[3][1], quad[3][2] );
    glEnd();
}

void UtilConcrete::draw( const QuadCUL& quad, const std::array<ColorS, 4>& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_QUADS );
    glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glColor4f( color[2].getRF(), color[2].getGF(), color[2].getBF(), color[2].getAF() );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glColor4f( color[3].getRF(), color[3].getGF(), color[3].getBF(), color[3].getAF() );
    glVertex3f( quad[3][0], quad[3][1], quad[3][2] );
    glEnd();
}

void UtilConcrete::draw( const Triangle& triangle, const ColorS& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_TRIANGLES );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( triangle[0][0], triangle[0][1], triangle[0][2] );
    glVertex3f( triangle[1][0], triangle[1][1], triangle[1][2] );
    glVertex3f( triangle[2][0], triangle[2][1], triangle[2][2] );
    glEnd();
}

void UtilConcrete::draw( const Triangle& quad, const std::array<ColorS, 4>& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_TRIANGLES );
    glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
    glVertex3f( quad[0][0], quad[0][1], quad[0][2] );
    glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
    glVertex3f( quad[1][0], quad[1][1], quad[1][2] );
    glColor4f( color[2].getRF(), color[2].getGF(), color[2].getBF(), color[2].getAF() );
    glVertex3f( quad[2][0], quad[2][1], quad[2][2] );
    glEnd();
}

void UtilConcrete::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_TRIANGLES );
    glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
    glVertex3f( values[0][0], values[0][1], values[0][2] );
    glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
    glVertex3f( values[1][0], values[1][1], values[1][2] );
    glColor4f( color[2].getRF(), color[2].getGF(), color[2].getBF(), color[2].getAF() );
    glVertex3f( values[2][0], values[2][1], values[2][2] );
    glEnd();
}

void UtilConcrete::draw( const LineData& values, const LineColors& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_LINES );
    glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
    glVertex3f( values[0][0], values[0][1], values[0][2] );
    glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
    glVertex3f( values[1][0], values[1][1], values[1][2] );
    glEnd();
}

void UtilConcrete::draw( const LineData& values, const ColorS& color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_LINE );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( values[0][0], values[0][1], values[0][2] );
    glVertex3f( values[1][0], values[1][1], values[1][2] );
    glEnd();
}

void UtilConcrete::draw( const Point& position, const ColorS& color )
{
    glBegin( GL_POINTS );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( position.x(), position.y(), position.z() );
    glEnd();
}

void UtilConcrete::clearColorAndDepthBuffer()
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // glDepthFunc( GL_LEQUAL );
}

void UtilConcrete::createQuad( float scale )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_QUADS );
    glColor3f( 1.f, 1.f, 1.f );
    glVertex2f( -0.5f * scale, -0.5f * scale );
    glVertex2f( 0.5f * scale, -0.5f * scale );
    glVertex2f( 0.5f * scale, 0.5f * scale );
    glVertex2f( -0.5f * scale, 0.5f * scale );
    glEnd();
}

void UtilConcrete::clearColorTo( const ColorS color )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glClearColor( static_cast<GLclampf>( color.getRF() ), static_cast<GLclampf>( color.getGF() ), static_cast<GLclampf>( color.getBF() ),
                  static_cast<GLclampf>( color.getAF() ) );
}

void UtilConcrete::clearBuffer( const ClearMasks mask )
{
    glClear( static_cast<GLbitfield>( mask ) );
}

void UtilConcrete::setClientState( ClientStateTypes cs, bool enabled )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( enabled )
    {
        glEnableClientState( (GLenum)cs );
    }
    else
    {
        glDisableClientState( (GLenum)cs );
    }
}

void UtilConcrete::texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glTexCoordPointer( coordinatesPerElement, (GLenum)dataType, stride, pointer );

    /*
    size - Specifies the number of coordinates per array element.
    Must be 1, 2, 3, or 4. The initial value is 4.

    type -  Specifies the data type of each texture coordinate.Symbolic constants GL_SHORT,
    GL_INT, GL_FLOAT, or GL_DOUBLE are accepted.
    The initial value is GL_FLOAT.

    stride - Specifies the byte offset between consecutive texture coordinate sets.
    If stride is 0, the array elements are understood to be tightly packed.
    The initial value is 0.

    pointer - Specifies a pointer to the first coordinate of the first texture
    coordinate set in the array.The initial value is 0.
    */
}

void UtilConcrete::vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    /*
    size - Specifies the number of coordinates per array element.
    Must be 1, 2, 3, or 4. The initial value is 4.

    type -  Specifies the data type of each texture coordinate.Symbolic constants GL_SHORT,
    GL_INT, GL_FLOAT, or GL_DOUBLE are accepted.
    The initial value is GL_FLOAT.

    stride - Specifies the byte offset between consecutive texture coordinate sets.
    If stride is 0, the array elements are understood to be tightly packed.
    The initial value is 0.

    pointer - Specifies a pointer to the first coordinate of the first texture
    coordinate set in the array.The initial value is 0.
    */
    glVertexPointer( coordinatesPerElement, (GLenum)dataType, stride, pointer );
}

void UtilConcrete::setVertexArrayClientState( const bool enable )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // If enabled, the vertex array is enabled for writing and used during
    // rendering when glArrayElement,
    // glDrawArrays, glDrawElements, glDrawRangeElements glMultiDrawArrays,
    // or glMultiDrawElements is called. See glVertexPointer.
    if( enable )
    {
        glEnableClientState( GL_VERTEX_ARRAY );
    }
    else
    {
        glDisableClientState( GL_VERTEX_ARRAY );
    }
}
//

void UtilConcrete::setColorClientState( bool enable )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // If enabled, the vertex array is enabled for writing and used during
    // rendering when glArrayElement,
    // glDrawArrays, glDrawElements, glDrawRangeElements glMultiDrawArrays,
    // or glMultiDrawElements is called. See glVertexPointer.
    if( enable )
    {
        glEnableClientState( GL_COLOR_ARRAY );
    }
    else
    {
        glEnableClientState( GL_COLOR_ARRAY );
    }
}

unsigned int UtilConcrete::generateVertexArray( const int size )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    GLuint vao = 0;
    log( "glGenVertexArrays( size, &vao )" );
    glGenVertexArrays( size, &vao );
    return vao;
}

void UtilConcrete::bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type )
{
    bindBuffer( type, bufferId );
    auto dataVal = (void*)( &data.data );
    bufferDataImpl( dataVal, static_cast<GLenum>( type ), static_cast<GLsizeiptr>( 4 * sizeof( QuadCUL::PointType ) ) );
}

void UtilConcrete::bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type )
{
    bindBuffer( type, bufferId );
    bufferDataImpl( data.data(), static_cast<GLenum>( type ), static_cast<GLsizeiptr>( data.size() * sizeof( unsigned int ) ) );
}

void UtilConcrete::bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type )
{
    bindBuffer( type, bufferId );
    bufferDataImpl( data.data(), static_cast<GLenum>( type ), static_cast<GLsizeiptr>( data.size() * sizeof( float ) ) );
}

void UtilConcrete::bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer( type, bufferId );
    glBufferData( static_cast<GLenum>( type ), data.size() * sizeof( TextureData2D ), data.data(), GL_DYNAMIC_DRAW );
}

void UtilConcrete::bufferDataImpl( const void* data, const GLenum target, const GLsizeiptr dataSize )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    /*
    Creates and initializes a buffer object's data store
    */
    log( "glBufferData" );
    glBufferData( target,         // Specifies the target to which the buffer object is bound for
                                  // glBufferData.
                  dataSize,       // Specifies the size in bytes of the buffer object's new
                                  // data store.
                  data,           // Specifies a pointer to data that will be copied into the data
                                  // store for initialization, or NULL if no data is to be copied.
                  GL_STATIC_DRAW  // usage - Specifies the expected usage pattern of the
                                  // data store.
    );
    /*
    target:
    GL_ARRAY_BUFFER - Vertex attributes
    GL_ATOMIC_COUNTER_BUFFER - Atomic counter storage
    GL_COPY_READ_BUFFER - Buffer copy source
    GL_COPY_WRITE_BUFFER - Buffer copy destination
    GL_DISPATCH_INDIRECT_BUFFER - Indirect compute dispatch commands
    GL_DRAW_INDIRECT_BUFFER - Indirect command arguments
    GL_ELEMENT_ARRAY_BUFFER - Vertex array indices
    GL_PIXEL_PACK_BUFFER - Pixel read target
    GL_PIXEL_UNPACK_BUFFER - Texture data source
    GL_QUERY_BUFFER - Query result buffer
    GL_SHADER_STORAGE_BUFFER - Read-write storage for shaders
    GL_TEXTURE_BUFFER - Texture data buffer
    GL_TRANSFORM_FEEDBACK_BUFFER - Transform feedback buffer
    GL_UNIFORM_BUFFER - Uniform block storage

    usage - GL_STREAM_DRAW,
            GL_STREAM_READ,
            GL_STREAM_COPY,
            GL_STATIC_DRAW,
            GL_STATIC_READ,
            GL_STATIC_COPY,
            GL_DYNAMIC_DRAW,
            GL_DYNAMIC_READ, or
            GL_DYNAMIC_COPY.

    https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferData.xhtml:
    Description
    glBufferData and glNamedBufferData create a new data store for a buffer
    object. In case of glBufferData, the buffer object currently bound to target
    is used. For glNamedBufferData, a buffer object associated with ID specified
    by the caller in buffer will be used instead.

    While creating the new storage, any pre-existing data store is deleted. The
    new data store is created with the specified size in bytes and usage. If
    data is not NULL, the data store is initialized with data from this pointer.
    In its initial state, the new data store is not mapped, it has a NULL mapped
    pointer, and its mapped access is GL_READ_WRITE.

    usage is a hint to the GL implementation as to how a buffer object's data
    store will be accessed. This enables the GL implementation to make more
    intelligent decisions that may significantly impact buffer object
    performance. It does not, however, constrain the actual usage of the data
    store. usage can be broken down into two parts: first, the frequency of
    access (modification and usage), and second, the nature of that access. The
    frequency of access may be one of these:

    STREAM
    The data store contents will be modified once and used at most a few times.

    STATIC
    The data store contents will be modified once and used many times.

    DYNAMIC
    The data store contents will be modified repeatedly and used many times.

    The nature of access may be one of these:

    DRAW
    The data store contents are modified by the application, and used as the
    source for GL drawing and image specification commands.

    READ
    The data store contents are modified by reading data from the GL, and used
    to return that data when queried by the application.

    COPY
    The data store contents are modified by reading data from the GL, and used
    as the source for GL drawing and image specification commands.

    Notes
    If data is NULL, a data store of the specified size is still created, but
    its contents remain uninitialized and thus undefined.

    Clients must align data elements consistently with the requirements of the
    client platform, with an additional base-level requirement that an offset
    within a buffer to a datum comprising N bytes be a multiple of N.

    The GL_ATOMIC_COUNTER_BUFFER target is available only if the GL version
    is 4.2 or greater.

    The GL_DISPATCH_INDIRECT_BUFFER and GL_SHADER_STORAGE_BUFFER targets are
    available only if the GL version is 4.3 or greater.

    The GL_QUERY_BUFFER target is available only if the GL version is 4.4 or
    greater.

    Errors
    GL_INVALID_ENUM is generated by glBufferData if target is not one of the
    accepted buffer targets. GL_INVALID_ENUM is generated if usage is not
    GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW,
    GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or
    GL_DYNAMIC_COPY. GL_INVALID_VALUE is generated if size is negative
    GL_INVALID_OPERATION is generated by glBufferData if the reserved buffer
    object name 0 is bound to target. GL_INVALID_OPERATION is generated by
    glNamedBufferData if buffer is not the name of an existing buffer object.
    GL_INVALID_OPERATION is generated if the GL_BUFFER_IMMUTABLE_STORAGE flag of
    the buffer object is GL_TRUE. GL_OUT_OF_MEMORY is generated if the GL is
    unable to create a data store with the specified size

    */
}

void UtilConcrete::bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "bufferSubdata" );
    bindBuffer( type, bufferId );
    size_t dataSize = data.size() * sizeof( data.front() );
    glBufferSubData( GL_ARRAY_BUFFER, 0, dataSize, data.data() );
}

unsigned int UtilConcrete::generateAndBindBuffer( const BufferTypes bufferType, const int size )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto bufferId = generateBuffer( bufferType, size );
    bindBuffer( bufferType, bufferId );
    return bufferId;
}

unsigned int UtilConcrete::generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }


    const auto ebo = generateBuffer( BufferTypes::ELEMENT_ARRAY_BUFFER, size );
    bindBuffer( BufferTypes::ELEMENT_ARRAY_BUFFER, ebo );
    log( "glBufferData" );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>( data.size() * sizeof( unsigned int ) ), data.data(), GL_STATIC_DRAW );

    /*
glVertexAttribPointer - define an array of generic vertex attribute data
index - Specifies the index of the generic vertex attribute to be modified.
size - Specifies the number of components per generic vertex attribute. Must be
1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA is accepted by
glVertexAttribPointer. The initial value is 4. type - Specifies the data type of
each component in the array. The symbolic constants GL_BYTE, GL_UNSIGNED_BYTE,
GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT are accepted by both
functions. Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE,
GL_INT_2_10_10_10_REV, and GL_UNSIGNED_INT_2_10_10_10_REV are accepted by
glVertexAttribPointer. The initial value is GL_FLOAT.

normalized - For glVertexAttribPointer, specifies whether fixed-point data
values should be normalized (GL_TRUE) or converted directly as fixed-point
values (GL_FALSE) when they are accessed.

stride - Specifies the byte offset between consecutive generic vertex
attributes. If stride is 0, the generic vertex attributes are understood to be
tightly packed in the array. The initial value is 0.

pointer - Specifies a offset of the first component of the first generic vertex
attribute in the array in the data store of the buffer currently bound to the
GL_ARRAY_BUFFER target. The initial value is 0.
*/
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), (void*)0 );
    return ebo;
}

void UtilConcrete::bufferData( uint8_t bufferId, const float vertices[], BufferTypes bufferType )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glBufferData" );
    bindBuffer( bufferType, bufferId );
    glBufferData( GL_ARRAY_BUFFER, sizeof( *vertices ), vertices, GL_STATIC_DRAW );
}

void UtilConcrete::enableVertexAttribiute( unsigned programId, const String& attribName )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto attributeLocation = UtilConcrete::getAttribLocation( programId, attribName );
    glEnableVertexAttribArray( attributeLocation );
}

void UtilConcrete::setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // glVertexPointer — define an array of vertex data
    // glVertexPointer specifies the location and data format of an array of
    // vertex coordinates to use when rendering. size specifies the number of
    // coordinates per vertex, and must be 2, 3, or 4. type specifies the data
    // type of each coordinate, and stride specifies the byte stride from one
    // vertex to the next, allowing vertices and attributes to be packed into a
    // single array or stored in separate arrays. (Single-array storage may be
    // more efficient on some implementations; see glInterleavedArrays.)

    //  If a non-zero named buffer object is bound to the GL_ARRAY_BUFFER target
    //  (see glBindBuffer) while a vertex array is specified, pointer is treated
    //  as a byte offset into the buffer object's data store. Also, the buffer
    //  object binding (GL_ARRAY_BUFFER_BINDING) is saved as vertex array
    //  client-side state (GL_VERTEX_ARRAY_BUFFER_BINDING).

    // When a vertex array is specified, size, type, stride, and pointer are
    // saved as client-side state, in addition to the current vertex array
    // buffer object binding.

    // To enable and disable the vertex array, call glEnableClientState and
    // glDisableClientState with the argument GL_VERTEX_ARRAY. If enabled, the
    // vertex array is used when glArrayElement, glDrawArrays,
    // glMultiDrawArrays, glDrawElements, glMultiDrawElements, or
    // glDrawRangeElements is called.
    //    glVertexPointer( coordinatesPerVertex, (GLenum)dataType, stride, data
    //    );
    glVertexPointer( coordinatesPerVertex, (GLenum)dataType, stride, data );
}

void UtilConcrete::disableVertexAttribiute( unsigned programId, const String& attribName )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto attributeLocation = UtilConcrete::getAttribLocation( programId, attribName );
    glDisableVertexAttribArray( attributeLocation );
}

void UtilConcrete::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( id )
    {
        if( bufferType == BufferTypes::ARRAY_BUFFER )
        {
            glDeleteBuffers( 1, &id );
            id = 0;
        }
        else if( bufferType == BufferTypes::VERTEX_ARRAY )
        {
            glDeleteVertexArrays( 1, &id );
            id = 0;
        }
        else
        {
            CUL::Assert::simple( false, "Type " + CUL::String( (unsigned)bufferType ) + " is not implemented." );
        }
    }
}

unsigned int UtilConcrete::getAttribLocation( unsigned programId, const String& attribName )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "getAttribLocation" );
    auto attribLocation = glGetAttribLocation( programId, attribName.cStr() );
    return static_cast<unsigned int>( attribLocation );
}

void UtilConcrete::unbindBuffer( const BufferTypes bufferType )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer( bufferType, 0 );
}

void UtilConcrete::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    auto it = m_currentBufferId.find( bufferType );
    if( it == m_currentBufferId.end() || it->second != bufferId )
    {
        m_currentBufferId[bufferType] = bufferId;
    }
    else
    {
        // log( "bindBuffer already set." );
        return;
    }

    if( BufferTypes::VERTEX_ARRAY == bufferType )
    {
        /*
        glBindVertexArray binds the vertex array object with name array. array
        is the name of a vertex array object previously returned from a call to
        glGenVertexArrays, or zero to break the existing vertex array object
        binding.

        If no vertex array object with name array exists, one is created when
        array is first bound. If the bind is successful no change is made to the
        state of the vertex array object, and any previous vertex array object
        binding is broken.
        */
        // log( "glBindVertexArray( " + String( bufferId ) + String( " )" ) );
        glBindVertexArray( static_cast<GLuint>( bufferId ) );
    }
    else
    {
        /*
        glBindBuffer binds a buffer object to the specified buffer binding
        point. Calling glBindBuffer with target set to one of the accepted
        symbolic constants and buffer set to the name of a buffer object binds
        that buffer object name to the target. If no buffer object with name
        buffer exists, one is created with that name. When a buffer object is
        bound to a target, the previous binding for that target is automatically
        broken.

        Buffer object names are unsigned integers. The value zero is reserved,
        but there is no default buffer object for each buffer object target.
        Instead, buffer set to zero effectively unbinds any buffer object
        previously bound, and restores client memory usage for that buffer
        object target (if supported for that target). Buffer object names and
        the corresponding buffer object contents are local to the shared object
        space of the current GL rendering context; two rendering contexts share
        buffer object names only if they explicitly enable sharing between
        contexts through the appropriate GL windows interfaces functions.
        */
        // log( "glBindBuffer" );
        glBindBuffer( static_cast<GLenum>( bufferType ), bufferId );
    }
}

// TODO: Remove type?
unsigned int UtilConcrete::generateBuffer( const BufferTypes bufferType, const int size )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    GLuint bufferId = 0;
    if( BufferTypes::VERTEX_ARRAY == bufferType )
    {
        log( "glGenVertexArrays" );
        glGenVertexArrays( size, &bufferId );
    }
    else
    {
        log( "glGenBuffers" );
        glGenBuffers( size, &bufferId );
    }

    return bufferId;
}

void UtilConcrete::drawElements( const PrimitiveType type, const std::vector<unsigned int>& data )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // glDrawElements — render primitives from array data
    // mode
    // Specifies what kind of primitives to render.Symbolic constants GL_POINTS,
    // GL_LINE_STRIP, // GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY,
    // GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, // GL_TRIANGLE_FAN, GL_TRIANGLES,
    // GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCYand // GL_PATCHES are
    // accepted.
    // count
    // Specifies the number of elements to be rendered.
    // type
    // Specifies the type of the values in indices.Must be one of
    // GL_UNSIGNED_BYTE, / GL_UNSIGNED_SHORT,/ or GL_UNSIGNED_INT.
    // indices
    // Specifies a pointer to the location where the indices are stored.
    // Description
    // glDrawElements specifies multiple geometric primitives with very few
    // subroutine calls. Instead of calling a GL function to pass each
    // individual vertex, normal, texture coordinate, edge flag, or color, you
    // can prespecify separate arrays of vertices, normals, and so on, and use
    // them to construct a sequence of primitives with a single call to
    // glDrawElements.

    // When glDrawElements is called, it uses count sequential elements from an
    // enabled array, starting at indices to construct a sequence of geometric
    // primitives.mode specifies what kind of primitives are constructedand how
    // the array elements construct these primitives.If more than one array is
    // enabled, each is used.

    // Vertex attributes that are modified by glDrawElements have an unspecified
    // value after glDrawElements returns.Attributes that aren't modified
    // maintain their previous values.

    glDrawElements( static_cast<GLenum>( type ), static_cast<GLsizei>( data.size() ), GL_UNSIGNED_INT, 0 );
}

void UtilConcrete::drawElements( const PrimitiveType type, const std::vector<float>& data )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDrawElements( static_cast<GLenum>( type ), static_cast<GLsizei>( data.size() ), GL_FLOAT, data.data() );
}

void UtilConcrete::drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDrawElements( static_cast<GLenum>( primitiveType ), static_cast<GLsizei>( count ), static_cast<GLenum>( dataType ), nullptr );
}

void UtilConcrete::enableVertexAttribArray( unsigned attributeId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    /*
    Enable or disable a generic vertex attribute array.
    glEnableVertexAttribArray and glEnableVertexArrayAttrib enable the generic
    vertex attribute array specified by index. glEnableVertexAttribArray uses
    currently bound vertex array object for the operation, whereas
    glEnableVertexArrayAttrib updates state of the vertex array object with ID
    vaobj.
    */
    log( "glEnableVertexAttribArray" );
    glEnableVertexAttribArray( static_cast<GLuint>( attributeId ) );
}

template <typename Out>
void split( const std::string& s, char delim, Out result )
{
    std::istringstream iss( s );
    std::string item;
    while( std::getline( iss, item, delim ) )
    {
        *result++ = item;
    }
}

std::vector<std::string> split( const std::string& s, char delim )
{
    std::vector<std::string> elems;
    split( s, delim, std::back_inserter( elems ) );
    return elems;
}

std::vector<std::string> UtilConcrete::listExtensions()
{
    GLint extensionsCount = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &extensionsCount );
    const GLubyte* extensions = glGetString( GL_EXTENSIONS );
    CUL::String wat = static_cast<const unsigned char*>( extensions );
    std::vector<std::string> extensionsVec = split( wat.string(), ' ' );
    return extensionsVec;
}

void UtilConcrete::setDepthTest( const bool enabled )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    if( enabled )
    {
        glEnable( GL_DEPTH_TEST );
        // glDepthMask( GL_TRUE );
        // glClearDepth( 1.0f );
        // glDepthMask( GL_FALSE );
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LESS );
    }
    else
    {
        glDisable( GL_DEPTH_TEST );
    }
}

void UtilConcrete::setBackfaceCUll( const bool enabled )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    if( enabled )
    {
        glEnable( GL_CULL_FACE );
        glCullFace( GL_BACK );
    }
    else
    {
        glDisable( GL_CULL_FACE );
        glCullFace( GL_BACK );
    }
}

void UtilConcrete::setTexuring( const bool enabled )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    if( enabled )
    {
        glEnable( GL_TEXTURE_2D );
    }
    else
    {
        glDisable( GL_TEXTURE_2D );
    }
}

unsigned UtilConcrete::generateTexture()
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "generateTexture" );
    // glGenTextures generate texture names.
    // n - Specifies the number of texture names to be generated.
    // textures - Specifies an array in which the generated texture names are
    // stored.

    GLuint textureId = 0;
    glGenTextures( 1, &textureId );
    return textureId;
}

void UtilConcrete::bindTexture( const unsigned int textureId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( m_lastTextureId != textureId )
    {
        // log( "bindTexture..." );
        glBindTexture( GL_TEXTURE_2D, textureId );
        m_lastTextureId = textureId;
        // log( "bindTexture... DONE." );
    }
    else
    {
        // log( "bindTexture canceled - already set." );
    }
}

void UtilConcrete::setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glTexParameteri" );
    bindTexture( textureId );
    glTexParameteri( GL_TEXTURE_2D, (GLenum)type, (GLint)val );
}

void UtilConcrete::freeTexture( unsigned int& textureId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( textureId != 0 )
    {
        log( "glDeleteTextures();" );
        glDeleteTextures( 1, &textureId );
        textureId = 0;
    }
}

void UtilConcrete::matrixStackPush()
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glPopMatrix();" );
    glPushMatrix();
    ++m_currentMatrix;
}

void UtilConcrete::matrixStackPop()
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glPopMatrix();" );
    glPopMatrix();
    --m_currentMatrix;
}

UtilConcrete::~UtilConcrete()
{
    CUL::Assert::simple( 0 == m_currentMatrix, "ERROR PUSH COUNT IS NOT EQUAL TO POP COUNT." );
}
