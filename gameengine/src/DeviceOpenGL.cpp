#include "DeviceOpenGL.hpp"

#include "gameengine/IGameEngine.hpp"
#include "gameengine/Camera.hpp"
#include "gameengine/Viewport.hpp"
#include "gameengine/AttributeMeta.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "gameengine/Shaders/ShaderUnit.hpp"
#include "gameengine/Windowing/IWindow.hpp"

#include "RunOnRenderThread.hpp"
#include "CUL/CULInterface.hpp"
#include "CUL/GenericUtils/SimpleAssert.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/Filesystem/FileFactory.hpp"

#include "CUL/STL_IMPORTS/STD_iostream.hpp"
#include "CUL/STL_IMPORTS/STD_sstream.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

#include "CUL/IMPORT_tracy.hpp"

using namespace LOGLW;

static CUL::CULInterface* g_interface = nullptr;
static CUL::LOG::ILogger* g_loggerOGL = nullptr;

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                             const char*  // message
                             ,
                             const void*  // userParam
);

const char* toString( GLenum type )
{
    /*
    GL_UNSIGNED_INT_VEC2,
    GL_UNSIGNED_INT_VEC3,
    GL_UNSIGNED_INT_VEC4,
    GL_DOUBLE_VEC2,
    GL_DOUBLE_VEC3,
    GL_DOUBLE_VEC4,
    GL_DOUBLE_MAT2,
    GL_DOUBLE_MAT3,
    GL_DOUBLE_MAT4,
    GL_DOUBLE_MAT2x3,
    GL_DOUBLE_MAT2x4,
    GL_DOUBLE_MAT3x2,
    GL_DOUBLE_MAT3x4,
    GL_DOUBLE_MAT4x2,
    GL_DOUBLE_MAT4x3*/

    switch( type )
    {
        case GL_BYTE:
            return "GLbyte";
        case GL_INT_VEC2:
            return "GL_INT_VEC2";
        case GL_INT_VEC3:
            return "GL_INT_VEC3";
        case GL_INT_VEC4:
            return "GL_INT_VEC4";
        case GL_UNSIGNED_BYTE:
            return "GLubyte";
        case GL_SHORT:
            return "GLshort";
        case GL_UNSIGNED_SHORT:
            return "GLushort";
        case GL_INT:
            return "GLint";
        case GL_UNSIGNED_INT:
            return "GLuint";
        case GL_FLOAT:
            return "GLfloat";
        case GL_FLOAT_MAT2x3:
            return "GL_FLOAT_MAT2x3";
        case GL_FLOAT_MAT2x4:
            return "GL_FLOAT_MAT2x4";
        case GL_FLOAT_MAT2:
            return "GLfloat_mat2";
        case GL_FLOAT_MAT3x2:
            return "GL_FLOAT_MAT3x2";
        case GL_FLOAT_MAT3x4:
            return "GL_FLOAT_MAT3x4";
        case GL_FLOAT_MAT3:
            return "GLfloat_mat3";
        case GL_FLOAT_MAT4x2:
            return "GL_FLOAT_MAT4x2";
        case GL_FLOAT_MAT4:
            return "GLfloat_mat4";
        case GL_FLOAT_MAT4x3:
            return "GL_FLOAT_MAT4x3";
        case GL_FLOAT_VEC2:
            return "GLFloat_vec2";
        case GL_FLOAT_VEC3:
            return "GLFloat_vec3";
        case GL_FLOAT_VEC4:
            return "GLFloat_vec4";
        case GL_2_BYTES:
            return "GL_2_BYTES";
        case GL_3_BYTES:
            return "GL_3_BYTES";
        case GL_4_BYTES:
            return "GL_4_BYTES";
        case GL_DOUBLE:
            return "GLdouble";
        case GL_SAMPLER_1D:
            return "Sampler 1D";
        case GL_SAMPLER_2D:
            return "Sampler 2D";
        case GL_SAMPLER_3D:
            return "Sampler 3D";
        default:
            return "not a GLenum type";
    }
}

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
                             const void* userParam
)
{
    if( id == 131185 )
    {
        // Buffer detailed info: Buffer object [x] (bound to GL_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) will use VIDEO memory as the
        // source for buffer object operations.
        // https://stackoverflow.com/questions/62248552/opengl-debug-context-warning-will-use-video-memory-as-the-source-for-buffer-o
        // can be safely ignored.
        return;
    }

    String messageString = "glDebugOutput Severity: ";

    switch( severity )
    {
        case GL_DEBUG_SEVERITY_HIGH:
            messageString += "HIGH";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            messageString += "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            messageString += "LOW";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            messageString += "NOTIFICATION";
            break;
    }

    messageString += ", Source: ";
    switch( source )
    {
        case GL_DEBUG_SOURCE_API:
            messageString += "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            messageString += "WINDOW SYSTEM";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            messageString += "SHADER COMPILER";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            messageString += "THIRD PARTY";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            messageString += "APPLICATION";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            messageString += "OTHER";
            break;
    }

    messageString += ", Type: ";
    switch( type )
    {
        case GL_DEBUG_TYPE_ERROR:
            messageString += "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            messageString += "DEPRECATED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            messageString += "UNDEFINED BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            messageString += "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            messageString += "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_MARKER:
            messageString += "MARKER";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            messageString += "PUSH GROUP";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            messageString += "POP GROUP";
            break;
        case GL_DEBUG_TYPE_OTHER:
            messageString += "OTHER";
            break;
        default:
            messageString += "UNKOWN";
    }
    messageString += ", Message: ";
    messageString += message;
    g_loggerOGL->log( messageString, CUL::LOG::Severity::Warn );

    return;
}

GLenum glCheckError_( const char* file, int line )
{
    GLenum errorCode;
    bool hasError{ false };
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

        CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Error, "File: %s, line: %s, error: %s", file, line, error );
        hasError = true;
    }

    CUL::Assert::simple( hasError == false, "OPENGL ERROR ^." );

    return errorCode;
}
#define glCheckError() glCheckError_( __FILE__, __LINE__ )

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message,
                             const void* userParam );

CUL::String enumToString( const GLenum val );
GLuint toGluint( unsigned value );

DeviceOpenGL::DeviceOpenGL( bool forceLegacy ) : IRenderDevice( forceLegacy )
{
    log( "DeviceOpenGL::DeviceOpenGL();" );

    g_loggerOGL = CUL::CULInterface::getInstance()->getLogger();
}

ContextInfo DeviceOpenGL::initContextVersion( LOGLW::IWindow* window )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::CULInterface::getInstance()->getThreadUtils().setThreadName( "RenderThread" );
        if( !RunOnRenderThread::getInstance().getIsRenderThread() )
        {
            CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
        }
    }

    ContextInfo result;
    window->setGLContextVersion( 4, 6 );
    window->setProfileMask( LOGLW::GLProfileMask::CORE );
    window->setContextFlag( LOGLW::GLContextFlag::DEBUG_FLAG );
    window->toggleDoubleBuffer( true );
    window->setStencilSize( 8 );
    result.glContext = window->createContext();

    auto error = glewInit();

    if( error != 0 )
    {
        CUL::String errorContent( reinterpret_cast<const char*>( glewGetErrorString( error ) ) );

        CUL::Assert::simple( GLEW_OK == error, "GLEW error: " + errorContent + result.glVersion );
    }

    if( glDebugMessageCallbackARB )
    {
        glDebugMessageCallbackARB( glDebugOutput, nullptr );
        glEnable( GL_DEBUG_OUTPUT );
        glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        checkLastCommandForErrors();
        log( "Debug message enabled.", CUL::LOG::Severity::Warn );

        // glEnable( GL_DEBUG_OUTPUT );
        // checkLastCommandForErrors();

        // glDebugMessageCallback( glDebugOutput, 0 );
        ////checkLastCommandForErrors();

        // glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    }

    checkLastCommandForErrors();
    /*
    Context version can be only set after context creation.
    I.e. SDL: SDL_GL_DeleteContext call.
    */
    // SDL_GL_DOUBLEBUFFER

    glGetIntegerv( GL_MAJOR_VERSION, &m_supportedVersion.major );
    checkLastCommandForErrors();
    glGetIntegerv( GL_MINOR_VERSION, &m_supportedVersion.minor );

    // Set debug otuput.

    // Possible values:
    // typedef enum
    //{
    //    SDL_GL_CONTEXT_PROFILE_CORE = 0x0001,
    //    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY = 0x0002,
    //    SDL_GL_CONTEXT_PROFILE_ES = 0x0004 /**<
    //    GLX_CONTEXT_ES2_PROFILE_BIT_EXT */
    //} SDL_GLprofile;
    const auto glStringVersion = glGetString( GL_VERSION );
    std::string s( reinterpret_cast<char const*>( glStringVersion ) );
    result.glVersion = s;
    checkLastCommandForErrors();

    m_versionString = (char*)glGetString( GL_VERSION );
    checkLastCommandForErrors();
    log( "OpenGL Version: " + m_versionString );
    const auto lower = m_versionString.toLowerR();
    if( lower.contains( "es" ) )
    {
        m_isEmbeddedSystems = true;
    }

    return result;
}

void DeviceOpenGL::setOrthogonalPerspective( const Camera& camera )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto left = camera.getLeft();
    const auto right = camera.getRight();

    const auto bottom = camera.getBottom();
    const auto top = camera.getTop();

    const auto zNear = camera.getZnear();
    const auto zFar = camera.getZfar();

    // glOrtho - multiply the current matrix with an orthographic matrix
    // left, right
    //    Specify the coordinates for the leftand right vertical clipping
    //    planes.

    // bottom, top
    //    Specify the coordinates for the bottomand top horizontal clipping
    //    planes.

    // nearVal, farVal
    //    Specify the distances to the nearerand farther depth clipping
    //    planes.These values are negative if the plane is to be behind the
    //    viewer.

    // glOrtho describes a transformation that produces a parallel
    // projection.The current matrix( see glMatrixMode ) is multiplied by this
    // matrixand the result replaces the current matrix, as if glMultMatrix were
    // called with the following matrix as its argument :

    // 2 right - left 0 0 t x 0 2 top - bottom 0 t y 0 0 - 2 farVal - nearVal t
    // z 0 0 0 1
    //    where

    //    t x = -right + left right - left
    //    t y = -top + bottom top - bottom
    //    t z = -farVal + nearVal farVal - nearVal
    //    Typically, the matrix mode is GL_PROJECTION, and left bottom - nearVal
    //    and right top - nearVal specify the points on the near clipping plane
    //    that are mapped to the lower left and upper right corners of the
    //    window, respectively, assuming that the eye is located at( 0, 0, 0 ).
    //    - farVal specifies the location of the far clipping plane.Both nearVal
    //    and farVal can be either positive or negative.

    //    Use glPushMatrix and glPopMatrix to save and restore the current
    //    matrix stack.
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml
    glOrtho( left,    // left
             right,   // right
             bottom,  // bottom
             top,     // top
             zNear,   // near
             zFar     // far
    );
}

void DeviceOpenGL::setPerspectiveProjection( const Camera& projectionData )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    auto fov = projectionData.getFov();
    auto ar = projectionData.getAspectRatio();
    auto zNear = projectionData.getZnear();
    auto zFar = projectionData.getZfar();
    // if( isLegacy() )
    {
        resetMatrixToIdentity( MatrixTypes::PROJECTION );
        gluPerspective( fov, ar, zNear, zFar );
    }
}

// TODO: Remove:
#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4100 )
#endif

void DeviceOpenGL::setViewport( const Viewport& viewport )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    glViewport( viewport.pos.getX(), viewport.pos.getY(), viewport.size.getWidth(), viewport.size.getHeight() );
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif
void DeviceOpenGL::lookAt( const Camera& vp )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    const auto& eye = vp.getEye();
    const auto& center = vp.getCenter();
    const auto& up = vp.getUp();
    // if( isLegacy() )
    {
        gluLookAt( eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z );
    }
}

void DeviceOpenGL::lookAt( const std::array<Pos3Dd, 3>& vec )
{
    lookAt( vec[0], vec[1], vec[2] );
}

void DeviceOpenGL::lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    gluLookAt( eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z );
}

std::uint32_t DeviceOpenGL::createProgram( const CUL::String& name )
{
    const auto programId = static_cast<unsigned int>( glCreateProgram() );
    log( "[DeviceOpenGL] glCreateProgram: " + String( programId ) );

    if( 0 == programId )
    {
        const GLenum err = glGetError();
        customAssert( GL_NO_ERROR == programId, "Error creating program, error numer: " + CUL::String( err ) );
        return 0;
    }

    glObjectLabel( GL_PROGRAM, programId, -1, name.cStr() );

    return programId;
}

void DeviceOpenGL::removeProgram( unsigned programId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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
    // assertOnProgramError( programId, GL_DELETE_STATUS );
    //glCheckError_( "DeviceOpenGL.cpp", 180 );
}

void DeviceOpenGL::linkProgram( unsigned programId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glLinkProgram( static_cast<GLuint>( programId ) );
    assertOnProgramError( programId, GL_LINK_STATUS );
}

void DeviceOpenGL::validateProgram( std::uint32_t programId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "[DeviceOpenGL] glValidateProgram( " + String( programId ) + ");" );
    glValidateProgram( programId );
    assertOnProgramError( programId, GL_VALIDATE_STATUS );
}

void DeviceOpenGL::assertOnProgramError( unsigned programId, unsigned val )
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

ShaderTypes DeviceOpenGL::getShaderType( const CUL::String& fileExtension )
{
    /*
    .vert - a vertex shader
    .tesc - a tessellation control shader
    .tese - a tessellation evaluation shader
    .geom - a geometry shader
    .frag - a fragment shader
    .comp - a compute shader
    */
    if( fileExtension.equals( "frag" ) || fileExtension.equals( ".frag" ) || fileExtension.equals( ".fs" ) ||
        fileExtension.equals( "fs" ) )
    {
        return static_cast<ShaderTypes>( GL_FRAGMENT_SHADER );
    }
    else if( fileExtension.equals( "vert" ) || fileExtension.equals( ".vert" ) || fileExtension.equals( ".vs" ) ||
             fileExtension.equals( "vs" ) )
    {
        return static_cast<ShaderTypes>( GL_VERTEX_SHADER );
    }
    else if( fileExtension.equals( "geom" ) || fileExtension.equals( ".geom" ) )
    {
        return static_cast<ShaderTypes>( GL_GEOMETRY_SHADER );
    }

    return static_cast<ShaderTypes>( GL_INVALID_ENUM );
}

bool DeviceOpenGL::attachShader( unsigned programId, unsigned shaderId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const GLuint programIdConverted = toGluint( programId );
    glAttachShader( programIdConverted, toGluint( shaderId ) );

    return true;
}

void DeviceOpenGL::dettachShader( unsigned programId, unsigned shaderId )
{
    log( "glDetachShader( " + String( programId ) + ", " + String( shaderId ) + " );" );

    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDetachShader( toGluint( programId ), toGluint( shaderId ) );
    getLastOperationStatus();
}

void DeviceOpenGL::removeShader( unsigned shaderId )
{
    log( "glDeleteShader( " + String( shaderId ) + " );" );

    GLuint gshaderId = shaderId;

    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDeleteShader( gshaderId );
    getLastOperationStatus();
}

void DeviceOpenGL::useProgram( int programId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glUseProgram( static_cast<GLuint>( programId ) );
}

int DeviceOpenGL::getCurrentProgram() const
{
    GLint id;
    glGetIntegerv( GL_CURRENT_PROGRAM, &id );

    return (int)id;
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::vec2& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    glUniform2fv( uniformLocation, 1, &val[0] );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::vec3& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    glUniform3fv( uniformLocation, 1, &val[0] );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::vec4& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    glUniform4fv( uniformLocation, 1, &val[0] );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::mat2& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    glUniformMatrix2fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::mat3& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, const glm::mat4& val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "setUniformValue" );
    // //https://stackoverflow.com/questions/17630313/rotation-around-a-pivot-point-with-opengl
    glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void DeviceOpenGL::setActiveTextureUnit( ETextureUnitIndex textureUnitIndex )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( String( "setActiveTextureUnit( " ) + String( (int)textureUnitIndex ) + String( " );" ) );

    GLenum textureId = static_cast<GLenum>( textureUnitIndex );

    glActiveTexture( textureId );
}

int DeviceOpenGL::getUniformLocation( unsigned programId, const String& attribName )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    // log( "glGetUniformLocation( " + String( programId ) + ", " + attribName + " );" );

    auto attribLocation = glGetUniformLocation( programId, attribName.cStr() );

    if( attribLocation == -1 )
    {
        // log( "DID NOT FOUND!" );
    }

    const GLenum err = glGetError();

    if( err != GL_NO_ERROR )
    {
        const GLubyte* errorAsString = gluErrorString( err );
        std::string errorAsSTDString = (char*)errorAsString;
        switch( err )
        {
            case GL_INVALID_OPERATION:
                log( "Error! GL_INVALID_OPERATION: " + errorAsSTDString );
                break;
            default:
                break;
        }
    }

    return attribLocation;
}

void DeviceOpenGL::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    /*
    glDrawArrays - render primitives from array data.
    mode - Specifies what kind of primitives to render. Symbolic constants
    GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_LINE_STRIP_ADJACENCY,
    GL_LINES_ADJACENCY, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES,
    GL_TRIANGLE_STRIP_ADJACENCY, GL_TRIANGLES_ADJACENCY and GL_PATCHES are
    accepted.
    first - Specifies the starting index in the enabled arrays.
    count - Specifies the number of indices to be rendered.
    */
    // log( "drawArrays" );
    bindBuffer( BufferTypes::VERTEX_ARRAY, vaoId );

    static std::size_t elementsCount{ 0u };
    if( primitiveType == PrimitiveType::TRIANGLES )
    {
        elementsCount = count / 9u;
    }
    else if( primitiveType == PrimitiveType::LINE_STRIP )
    {
        elementsCount = count / 4u;
    }
    else
    {
        CUL::Assert::simple( false, "TODO: Write formula." );
    }

    glDrawArrays( static_cast<GLenum>( primitiveType ), static_cast<GLint>( first ), static_cast<GLsizei>( elementsCount ) );
}

void DeviceOpenGL::vertexAttribPointer( const VertexData& meta )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer( BufferTypes::VERTEX_ARRAY, meta.VAO );
    bindBuffer( BufferTypes::ARRAY_BUFFER, meta.VBO );

    // Define an array of generic vertex attribute data
    /*
    GLuint index - specifies the index of the generic vertex attribute to be modified.
    GLint size - specifies the number of components per generic vertex
    attribute. Must be 1, 2, 3, 4. Additionally, the symbolic constant GL_BGRA
    is accepted by glVertexAttribPointer. The initial value is 4.

    GLenum type -
    specifies the data type of each component in the array. The symbolic
    constants GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT,
    and GL_UNSIGNED_INT are accepted by glVertexAttribPointer and
    glVertexAttribIPointer. Additionally GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE,
    GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and
    GL_UNSIGNED_INT_10F_11F_11F_REV are accepted by glVertexAttribPointer.
    GL_DOUBLE is also accepted by glVertexAttribLPointer and is the only token
    accepted by the type parameter for that function. The initial value is
    GL_FLOAT. GLboolean normalized - for glVertexAttribPointer, specifies
    whether fixed-point data values should be normalized (GL_TRUE) or converted
    directly as fixed-point values (GL_FALSE) when they are accessed. GLsizei
    stride - specifies the byte offset between consecutive generic vertex
    attributes. If stride is 0, the generic vertex attributes are understood to
    be tightly packed in the array. The initial value is 0. const void * pointer
    - specifies a offset of the first component of the first generic vertex
    attribute in the array in the data store of the buffer currently bound to
    the GL_ARRAY_BUFFER target. The initial value is 0.
    */

    auto& attributes = meta.Attributes;
    for( const auto& attribute : attributes )
    {
        const auto index = static_cast<GLuint>( attribute.Index );
        const auto size = static_cast<GLint>( attribute.Size );
        const auto type = static_cast<GLenum>( attribute.Type );
        const auto normalized = static_cast<GLboolean>( attribute.Normalized );
        const auto strideByte = static_cast<GLsizei>( attribute.StrideBytes );
        const auto ptr = attribute.DataOffset;
        glVertexAttribPointer( index, size, type, normalized, strideByte, ptr );
        glEnableVertexAttribArray( index );
    }
}

void DeviceOpenGL::setTextureData( std::uint32_t textureId, const TextureInfo& ti )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glTexImage2D( " + ti.toString() + " )" );
    bindTexture( textureId );
    glTexImage2D( GL_TEXTURE_2D, ti.level, (GLint)ti.pixelFormat, ti.size.width, ti.size.height, ti.border, (GLenum)ti.pixelFormat,
                  (GLenum)GL_UNSIGNED_BYTE, ti.data );
}

void DeviceOpenGL::rotate( const CUL::MATH::Rotation& rotation )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glRotatef( rotation.Pitch.getDeg(), 1.f, 0.f, 0.f );
    glRotatef( rotation.Yaw.getDeg(), 0.f, -1.f, 0.f );
    glRotatef( rotation.Roll.getDeg(), 0.f, 0.f, 1.f );
}

void DeviceOpenGL::draw( const CUL::MATH::Primitives::Line& values, const ColorS& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_LINES );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( values[0][0], values[0][1], values[0][2] );
    glVertex3f( values[1][0], values[1][1], values[1][2] );
    glEnd();
}

void DeviceOpenGL::rotate( const float angleDeg, const float x, const float y, const float z )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glRotatef( angleDeg, x, y, z );
}

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

unsigned DeviceOpenGL::getGPUTotalAvailableMemoryKb()
{
    GLint total_mem_kb = 0;
    glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb );
    return total_mem_kb;
}

unsigned DeviceOpenGL::getGPUCurrentAvailableMemoryKb()
{
    GLint cur_avail_mem_kb = 0;
    glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb );
    return cur_avail_mem_kb;
}

void DeviceOpenGL::checkLastCommandForErrors()
{
    const GLenum err = glGetError();
    const GLubyte* errorAsString = gluErrorString( err );

    if( GL_NO_ERROR != err )
    {
        customAssert( GL_NO_ERROR == err, "Error creating program, error numer: " + CUL::String( errorAsString ) );
    }
}

GLuint toGluint( unsigned value )
{
    return static_cast<GLuint>( value );
}

void DeviceOpenGL::setAttribValue( int, int )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void DeviceOpenGL::setAttribValue( int, unsigned )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void DeviceOpenGL::setAttribValue( int, bool )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

void DeviceOpenGL::setAttribValue( int, const CUL::String& )
{
    CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
}

UniformValue DeviceOpenGL::getUniformValue( std::int32_t inProgramId, std::int32_t inUniformId, DataType inDataType )
{
    UniformValue result;
    if( inDataType == DataType::FLOAT )
    {
        float value;
        glGetUniformfv( inProgramId, inUniformId, &value );
        result = value;
    }
    else if( inDataType == DataType::INT )
    {
        std::int32_t value;
        glGetUniformiv( inProgramId, inUniformId, &value );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_VEC2 )
    {
        glm::vec2 value;
        glGetnUniformfv( inProgramId, inUniformId, 2, &value[0] );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_VEC3 )
    {
        glm::vec3 value;
        glGetnUniformfv( inProgramId, inUniformId, 3, &value[0] );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_VEC4 )
    {
        glm::vec4 value;
        glGetnUniformfv( inProgramId, inUniformId, 4, &value[0] );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_MAT2 )
    {
        glm::mat2 value;
        glGetnUniformfv( inProgramId, inUniformId, 4, &value[0].x );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_MAT3 )
    {
        glm::mat3 value;
        glGetnUniformfv( inProgramId, inUniformId, 9, &value[0].x );
        result = value;
    }
    else if( inDataType == DataType::FLOAT_MAT4 )
    {
        constexpr std::size_t elementsCount = 16;
        constexpr std::size_t sizeOfFloat = sizeof( float );
        constexpr std::size_t bufferSize = sizeOfFloat * elementsCount;

        glm::mat4 value;
        float value2[16];
        glGetnUniformfv( inProgramId, inUniformId, bufferSize, &value[0].x );
        glGetnUniformfv( inProgramId, inUniformId, bufferSize, &value2[0] );
        result = value;
    }
    else if( inDataType == DataType::SAMPLER_2D )
    {
        std::int32_t value{ -1 };
        glGetUniformiv( inProgramId, inUniformId, &value );
        result = value;
    }
    else
    {
        CUL::Assert::simple( false, "NOT YET IMPLEMENTED." );
    }

    return result;
}

void DeviceOpenGL::setUniformValue( int uniformLocation, float value )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glUniform1f( static_cast<GLfloat>( uniformLocation ), value );
}

void DeviceOpenGL::setUniformValue( int uniformLocation, int value )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1i( " + String( uniformLocation ) + ", " + String( value ) + " );" );
    glUniform1i( static_cast<GLint>( uniformLocation ), value );
}
void DeviceOpenGL::setUniformValue( int uniformLocation, unsigned value )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glUniform1i( " + String( uniformLocation ) + ", " + String( value ) + " );" );
    glUniform1i( static_cast<GLuint>( uniformLocation ), value );
}

void DeviceOpenGL::setProjectionAndModelToIdentity()
{
    resetMatrixToIdentity( MatrixTypes::PROJECTION );
    resetMatrixToIdentity( MatrixTypes::MODELVIEW );
}

void DeviceOpenGL::resetMatrixToIdentity( const MatrixTypes matrix )
{
    glMatrixMode( static_cast<GLenum>( matrix ) );
    glLoadIdentity();
}

void DeviceOpenGL::translate( const Point& point )
{
    translate( point[0], point[1], point[2] );
}

void DeviceOpenGL::translate( const float x, const float y, const float z )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glTranslatef( x, y, z );
}

void DeviceOpenGL::scale( const CUL::MATH::Vector3Df& scale )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glScalef( scale.getX(), scale.getY(), scale.getZ() );
}

void DeviceOpenGL::scale( const float scale )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glScalef( scale, scale, scale );
}

void DeviceOpenGL::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color )
{
    matrixStackPush();

    // https://stackoverflow.com/questions/17630313/rotation-around-a-pivot-point-with-opengl

    translate( translation );
    rotate( rotation );
    draw( quad, color );

    matrixStackPop();
}

void DeviceOpenGL::draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color )
{
    matrixStackPush();

    glMultMatrixf( glm::value_ptr( model ) );
    draw( quad, color );

    matrixStackPop();
}

void DeviceOpenGL::draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color )
{
    matrixStackPush();

    glMultMatrixf( glm::value_ptr( model ) );
    draw( triangle, color );

    matrixStackPop();
}

void DeviceOpenGL::draw( const QuadCUL& quad, const ColorS& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const QuadData& quad, const ColorS& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const QuadData& quad, const QuadColors& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const QuadCUL& quad, const std::array<ColorS, 4>& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const TriangleCUL& triangle, const ColorS& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::draw( const LineData& values, const LineColors& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( getIsEmbeddedSystems() )
    {
    }
    else
    {
        glBegin( GL_LINES );
        glColor4f( color[0].getRF(), color[0].getGF(), color[0].getBF(), color[0].getAF() );
        glVertex3f( values[0][0], values[0][1], values[0][2] );
        glColor4f( color[1].getRF(), color[1].getGF(), color[1].getBF(), color[1].getAF() );
        glVertex3f( values[1][0], values[1][1], values[1][2] );
        glEnd();
    }
}

void DeviceOpenGL::draw( const LineData& values, const ColorS& color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBegin( GL_LINE );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( values[0][0], values[0][1], values[0][2] );
    glVertex3f( values[1][0], values[1][1], values[1][2] );
    glEnd();
}

void DeviceOpenGL::draw( const Point& position, const ColorS& color )
{
    glBegin( GL_POINTS );
    glColor4f( color.getRF(), color.getGF(), color.getBF(), color.getAF() );
    glVertex3f( position.x(), position.y(), position.z() );
    glEnd();
}

void DeviceOpenGL::clearColorAndDepthBuffer()
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    // glDepthFunc( GL_LEQUAL );
}

void DeviceOpenGL::createQuad( float scale )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::clearColorTo( const ColorS color )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glClearColor( static_cast<GLclampf>( color.getRF() ), static_cast<GLclampf>( color.getGF() ), static_cast<GLclampf>( color.getBF() ),
                  static_cast<GLclampf>( color.getAF() ) );
}

void DeviceOpenGL::clearBuffer( const ClearMasks mask )
{
    glClear( static_cast<GLbitfield>( mask ) );
}

void DeviceOpenGL::setClientState( ClientStateTypes cs, bool enabled )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::setVertexArrayClientState( const bool enable )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::setColorClientState( bool enable )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

unsigned int DeviceOpenGL::generateVertexArray( const int size )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    GLuint vao = 0;
    // log( "glGenVertexArrays( size, &vao )" );
    glGenVertexArrays( size, &vao );
    return vao;
}

void DeviceOpenGL::bufferData( BufferDataId bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type )
{
    bindBuffer( type, bufferId );
    auto dataVal = (void*)( &data.data );
    bufferDataImpl( dataVal, static_cast<GLenum>( type ), static_cast<GLsizeiptr>( 4 * sizeof( QuadCUL::PointType ) ) );
}

void DeviceOpenGL::bufferData( BufferDataId bufferId, const CUL::DataWrapper& data, const BufferTypes type )
{
    constexpr std::size_t size_gl_int = sizeof( GLuint );
    constexpr std::size_t size_normal32Size = sizeof( std::uint32_t );
    constexpr std::size_t size_un = sizeof( unsigned );

    bindBuffer( type, bufferId );
    bufferDataImpl( data.getData(), static_cast<GLenum>( type ), static_cast<GLsizeiptr>( data.getSize() ) );
}

void DeviceOpenGL::bufferData( BufferDataId bufferId, const std::vector<float>& data, const BufferTypes type )
{
    bindBuffer( type, bufferId );
    bufferDataImpl( data.data(), static_cast<GLenum>( type ), static_cast<GLsizeiptr>( data.size() * sizeof( float ) ) );
}

void DeviceOpenGL::bufferData( BufferDataId bufferId, const std::vector<TextureData2D>& data, const BufferTypes type )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer( type, bufferId );
    glBufferData( static_cast<GLenum>( type ), data.size() * sizeof( TextureData2D ), data.data(), GL_DYNAMIC_DRAW );
}

void DeviceOpenGL::bufferDataImpl( const void* data, const GLenum target, const GLsizeiptr dataSize )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::bufferSubdata( BufferDataId bufferId, const BufferTypes type, std::vector<TextureData2D>& data )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "bufferSubdata" );
    bindBuffer( type, bufferId );
    size_t dataSize = data.size() * sizeof( data.front() );
    glBufferSubData( GL_ARRAY_BUFFER, 0, dataSize, data.data() );
}

unsigned int DeviceOpenGL::generateAndBindBuffer( const BufferTypes bufferType, const int size )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto bufferId = generateBuffer( bufferType, size );
    bindBuffer( bufferType, bufferId );
    return bufferId;
}

unsigned int DeviceOpenGL::generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::bufferData( BufferDataId bufferId, const float vertices[], BufferTypes bufferType )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glBufferData" );
    bindBuffer( bufferType, bufferId );
    glBufferData( GL_ARRAY_BUFFER, sizeof( *vertices ), vertices, GL_STATIC_DRAW );
}

void DeviceOpenGL::enableVertexAttribiute( unsigned programId, const String& attribName )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto attributeLocation = DeviceOpenGL::getAttribLocation( programId, attribName );
    glEnableVertexAttribArray( attributeLocation );
}

void DeviceOpenGL::setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::disableVertexAttribiute( unsigned programId, const String& attribName )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    const auto attributeLocation = DeviceOpenGL::getAttribLocation( programId, attribName );
    glDisableVertexAttribArray( attributeLocation );
}

void DeviceOpenGL::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( id )
    {
        if( bufferType == BufferTypes::ARRAY_BUFFER )
        {
            log( "DeviceOpenGL::deleteBuffer BufferTypes::ARRAY_BUFFER: " + String( id ) );
            glDeleteBuffers( 1, &id );
            m_currentBufferId[bufferType] = -1;
            id = 0;
        }
        else if( bufferType == BufferTypes::VERTEX_ARRAY )
        {
            log( "DeviceOpenGL::deleteBuffer BufferTypes::VERTEX_ARRAY: " + String( id ) );
            glDeleteVertexArrays( 1, &id );
            m_currentBufferId[bufferType] = -1;
            id = 0;
        }
        else
        {
            CUL::Assert::simple( false, "Type " + CUL::String( (unsigned)bufferType ) + " is not implemented." );
        }
    }
}

int DeviceOpenGL::getAttribLocation( unsigned programId, const String& attribName )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "getAttribLocation" );
    auto attribLocation = glGetAttribLocation( programId, attribName.cStr() );
    return static_cast<unsigned int>( attribLocation );
}

void DeviceOpenGL::unbindBuffer( const BufferTypes bufferType )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer( bufferType, 0 );
}

void DeviceOpenGL::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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
unsigned int DeviceOpenGL::generateBuffer( const BufferTypes bufferType, const int size )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    GLuint bufferId = 0;
    if( BufferTypes::VERTEX_ARRAY == bufferType )
    {
        glGenVertexArrays( size, &bufferId );
        CUL::LOG::ILogger::getInstance().logVariable( CUL::LOG::Severity::Info, "glGenVertexArrays id: %d", bufferId );
    }
    else
    {
        glGenBuffers( size, &bufferId );
        log( "glGenBuffers id: " + String( bufferId ) );
    }

    return bufferId;
}

void DeviceOpenGL::drawElements( const PrimitiveType type, const CUL::DataWrapper& inData )
{
    CUL::Assert::simple( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) == true,
                         "NOT IN THE RENDER THREAD." );

    const std::size_t elementsCount = inData.getElementCount();

    if( inData.getType() == CUL::ETypes::Float )
    {
        glDrawElements( static_cast<GLenum>( type ), static_cast<GLsizei>( elementsCount ), GL_FLOAT, inData.getData() );
    }
    else if( inData.getType() == CUL::ETypes::Uint32 )
    {
        glDrawElements( static_cast<GLenum>( type ), static_cast<GLsizei>( elementsCount ), GL_UNSIGNED_INT, 0 );
    }
    else
    {
        CUL::Assert::simple( false, "Unkown type!" );
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
}

void DeviceOpenGL::drawElements( const PrimitiveType type, const std::vector<float>& data )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDrawElements( static_cast<GLenum>( type ), static_cast<GLsizei>( data.size() ), GL_FLOAT, data.data() );
}

void DeviceOpenGL::drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDrawElements( static_cast<GLenum>( primitiveType ), static_cast<GLsizei>( count ), static_cast<GLenum>( dataType ), nullptr );
}

void DeviceOpenGL::enableVertexAttribArray( unsigned attributeId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

std::vector<std::string> DeviceOpenGL::listExtensions()
{
    std::vector<std::string> extensionsVec;
    GLint extensionsCount = 0;
    glGetIntegerv( GL_NUM_EXTENSIONS, &extensionsCount );
    const GLubyte* extensions = glGetString( GL_EXTENSIONS );
    if( extensions )
    {
        CUL::String wat( reinterpret_cast<const char*>( extensions ) );
        extensionsVec = split( wat.string(), ' ' );
    }

    return extensionsVec;
}

void DeviceOpenGL::setDepthTest( const bool enabled )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }
    if( enabled )
    {
        glEnable( GL_DEPTH_TEST );
        // glDepthMask( GL_TRUE );
        // glClearDepth( 1.0f );
        // glDepthMask( GL_FALSE );
        glDepthFunc( GL_LESS );
    }
    else
    {
        glDisable( GL_DEPTH_TEST );
    }
}

void DeviceOpenGL::setBackfaceCUll( const bool enabled )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::setTexuring( const bool enabled )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( !getIsEmbeddedSystems() )
    {
        if( enabled )
        {
            // glEnable( GL_TEXTURE_2D );
        }
        else
        {
            // glDisable( GL_TEXTURE_2D );
        }
        checkLastCommandForErrors();
    }
}

unsigned DeviceOpenGL::generateTexture()
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
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

void DeviceOpenGL::bindTexture( const unsigned int textureId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glBindTexture( GL_TEXTURE_2D, textureId );
}

void DeviceOpenGL::setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glTexParameteri" );
    bindTexture( textureId );
    glTexParameteri( GL_TEXTURE_2D, (GLenum)type, (GLint)val );
}

void DeviceOpenGL::freeTexture( std::uint32_t textureId )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( textureId != 0 )
    {
        log( "glDeleteTextures();" );
        glDeleteTextures( 1, &textureId );
    }
}

void DeviceOpenGL::matrixStackPush()
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glPopMatrix();" );
    glPushMatrix();
    ++m_currentMatrix;
}

void DeviceOpenGL::matrixStackPop()
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glPopMatrix();" );
    glPopMatrix();
    --m_currentMatrix;
}

DeviceOpenGL::~DeviceOpenGL()
{
    CUL::Assert::simple( 0 == m_currentMatrix, "ERROR PUSH COUNT IS NOT EQUAL TO POP COUNT." );
}

void* DeviceOpenGL::getNativeDevice()
{
    return nullptr;
}

bool DeviceOpenGL::isLegacy()
{
    ZoneScoped;
    if( m_forceLegacy )
    {
        return true;
    }

    return getVersion().major < 2;
}

void DeviceOpenGL::prepareFrame()
{
}

void DeviceOpenGL::finishFrame()
{
}

bool DeviceOpenGL::getLastOperationStatus()
{
    bool result{ true };
    GLenum errorCode;
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

        log( error );
        result = false;
    }
    return result;
}

void DeviceOpenGL::toggleDebugOutput( bool enable )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( enable )
    {
        // glEnable( GL_DEBUG_OUTPUT );
        // checkLastCommandForErrors();

        // glDebugMessageCallback( glDebugOutput, 0 );
        ////checkLastCommandForErrors();

        // glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
        ////checkLastCommandForErrors();
    }
    else
    {
        glDisable( GL_DEBUG_OUTPUT );
    }
}

size_t DeviceOpenGL::getFrameBufferCount() const
{
    throw std::logic_error( "The method or operation is not implemented." );
}

const String& DeviceOpenGL::getName() const
{
    return m_name;
}

void DeviceOpenGL::initDebugUI()
{
    throw std::logic_error( "The method or operation is not implemented." );
}

ShaderUnit* DeviceOpenGL::createShaderUnit( const CUL::FS::Path& shaderPath, bool assertOnErrors, CUL::String& errorMessage )
{
    ShaderUnit* result = findShader( shaderPath );
    if( result != nullptr )
    {
        return result;
    }

    return createShaderUnitForce( shaderPath, assertOnErrors, errorMessage );
}

ShaderUnit* DeviceOpenGL::createShaderUnitForce( const CUL::FS::Path& shaderPath, bool assertOnErrors, CUL::String& errorMessage )
{
    CUL::Assert::simple( RunOnRenderThread::getInstance().getIsRenderThread() == true, "NOT IN THE RENDER THREAD." );

    std::unique_ptr<ShaderUnit> newShader = std::make_unique<ShaderUnit>();
    newShader->File.reset( CUL::CULInterface::getInstance()->getFF()->createRegularFileRawPtr( shaderPath ) );

    if( shaderPath == "embedded_shaders/basic_color.frag" )
    {
        const std::string fragmentShaderSource =
#include "embedded_shaders/basic_color.frag"
            ;
        newShader->File->loadFromStringNoEmptyLines( fragmentShaderSource, true );
    }
    else if( shaderPath == "embedded_shaders/basic_pos.vert" )
    {
        const std::string vertexShaderSource =
#include "embedded_shaders/basic_pos.vert"
            ;
        newShader->File->loadFromStringNoEmptyLines( vertexShaderSource, true );
    }
    else if( shaderPath == "embedded_shaders/camera.frag" )
    {
        const std::string fragmentShaderSource =
#include "embedded_shaders/camera.frag"
            ;
        newShader->File->loadFromStringNoEmptyLines( fragmentShaderSource, true );
    }
    else if( shaderPath == "embedded_shaders/camera.vert" )
    {
        const std::string vertexShaderSource =
#include "embedded_shaders/camera.vert"
            ;
        newShader->File->loadFromStringNoEmptyLines( vertexShaderSource, true );
    }
    else
    {
        CUL::Assert::simple( shaderPath.exists(), "NOT IN THE RENDER THREAD." );
        newShader->File->load( true, true );
    }

    const CUL::FS::Path filePath = newShader->File->getPath();
    const auto extension = filePath.getExtension();
    newShader->Type = CShaderTypes::getShaderType( extension );

    const auto oglShaderType = getShaderType( extension );
    const auto id = static_cast<unsigned int>( glCreateShader( static_cast<GLenum>( oglShaderType ) ) );

    auto& shaderCode = *newShader->File;
    const auto codeLength = static_cast<GLint>( shaderCode.getLinesCount() );
    log( "[DeviceOpenGL] glCreateShader( " + String( static_cast<GLenum>( oglShaderType ) ) + ");" );
    glShaderSource( id, codeLength, shaderCode.getContent(), nullptr );
    glCompileShader( id );

    GLint compilationResult = 0;
    glGetShaderiv( id, GL_COMPILE_STATUS, &compilationResult );
    if( GL_FALSE == compilationResult )
    {
        GLchar eLog[1024] = { 0 };
        glGetShaderInfoLog( id, sizeof( eLog ), nullptr, eLog );
        auto errorAsString = std::string( eLog );
        errorMessage = "Error compiling shader: " + errorAsString + "\n";
        errorMessage += "Shader Path: " + shaderCode.getPath().getPath() + "\n";
        if( assertOnErrors )
        {
            customAssert( false, errorMessage );
        }

        newShader->State = EShaderUnitState::Error;
    }
    else
    {
        newShader->State = EShaderUnitState::Loaded;
    }

    newShader->ID = id;

    static std::int32_t shaderCounter{ 0 };

    constexpr std::size_t bufferSize{ 64u };
    char tableName[bufferSize];
    snprintf( tableName, bufferSize, "%s/%d", *filePath.getPath(), shaderCounter++ );

    setObjectName( LOGLW::EObjectType::SHADER, id, tableName );

    m_shadersUnits[shaderPath.getPath()] = std::move( newShader );
    return m_shadersUnits[shaderPath.getPath()].get();
}

void DeviceOpenGL::deleteShaderUnit( ShaderUnit* inShaderUnit )
{
    if( !RunOnRenderThread::getInstance().getIsRenderThread() )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glDeleteShader( inShaderUnit->ID );

    const auto shaderPath = inShaderUnit->File->getPath();
    const auto shaderPathString = shaderPath.getPath();

    const auto it = m_shadersUnits.find( shaderPathString );
    CUL::Assert::simple( it != m_shadersUnits.end(), "NOT IN THE RENDER THREAD." );
    m_shadersUnits.erase( it );
}

ShaderUnit* DeviceOpenGL::findShader( const CUL::FS::Path& shaderPath ) const
{
    std::lock_guard<std::mutex> locker( m_shadersMtx );
    const auto it = m_shadersUnits.find( shaderPath );
    if( it == m_shadersUnits.end() )
    {
        return nullptr;
    }

    return it->second.get();
}

LOGLW::RenderTypes::RendererType DeviceOpenGL::getType() const
{
    return LOGLW::RenderTypes::RendererType::OPENGL_MODERN;
}

void DeviceOpenGL::updateTextureData( const TextureInfo& ti, void* data )
{
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, ti.size.width, ti.size.height, GL_RGBA, GL_UNSIGNED_BYTE, data );
}

std::vector<AttributeInfo> DeviceOpenGL::fetchProgramAttributeInfo( std::int32_t inProgramId ) const
{
    std::vector<AttributeInfo> result;

    GLint size{ 0 };  // size of the variable

    constexpr GLsizei bufSize = 64;  // maximum name length
    GLchar name[bufSize];        // variable name in GLSL

    GLint count{ 0 };
    glGetProgramiv( inProgramId, GL_ACTIVE_ATTRIBUTES, &count );

    for( GLint i = 0; i < count; ++i )
    {
        GLsizei nameLength;
        GLenum variableType;
        glGetActiveAttrib( inProgramId, (GLuint)i, bufSize, &nameLength, &size, &variableType, name );

        AttributeInfo info;
        info.ID = i;
        info.Name = name;
        info.Size = size;
        info.TypeName = toString( variableType );
        info.Type = (DataType)variableType;

        result.push_back( info );
    }

    return result;
}

std::vector<UniformInfo> DeviceOpenGL::fetchProgramUniformsInfo( std::int32_t inProgramId ) const
{
    std::vector<UniformInfo> result;

    GLint size{ 0 };  // size of the variable

    constexpr GLsizei bufSize = 64;  // maximum name length
    GLchar name[bufSize];        // variable name in GLSL

    GLint count{ 0 };
    glGetProgramiv( inProgramId, GL_ACTIVE_UNIFORMS, &count );

    for( GLint i = 0; i < count; ++i )
    {
        GLsizei nameLength;
        GLenum variableType;
        glGetActiveUniform( inProgramId, (GLuint)i, bufSize, &nameLength, &size, &variableType, name );

        UniformInfo info;
        info.ID = i;
        info.Name = name;
        info.Size = size;
        info.TypeName = toString( variableType );
        info.Type = (DataType)variableType;

        result.push_back( info );
    }

    return result;
}

bool DeviceOpenGL::fetchUniformInfo( UniformInfo& inOutUniformInfo, std::int32_t inProgramId, const CUL::String& inUniformName )
{
    GLint size{ 0 };  // size of the variable

    constexpr GLsizei bufSize = 64;  // maximum name length
    GLchar name[bufSize];        // variable name in GLSL

    GLint count{ 0 };
    glGetProgramiv( inProgramId, GL_ACTIVE_UNIFORMS, &count );

    for( GLint i = 0; i < count; ++i )
    {
        GLsizei nameLength;
        GLenum variableType;
        glGetActiveUniform( inProgramId, (GLuint)i, bufSize, &nameLength, &size, &variableType, name );

        if( inUniformName == name )
        {
            inOutUniformInfo.ID = i;
            inOutUniformInfo.Name = name;
            inOutUniformInfo.Size = size;
            inOutUniformInfo.TypeName = toString( variableType );
            inOutUniformInfo.Type = (DataType)variableType;
            return true;
        }
    }
    return false;
}

void DeviceOpenGL::setObjectName( EObjectType objectType, std::uint32_t objectId, const CUL::String& name )
{
    GLenum oglType = 0;
    switch( objectType )
    {
        case EObjectType::BUFFER:
            oglType = GL_BUFFER;
            log( "glObjectLabel GL_BUFFER id: " + String( objectId ) + ", name: " + name );
            bindBuffer( BufferTypes::ARRAY_BUFFER, objectId );
            break;
        case EObjectType::ELEMENT_ARRAY_BUFFER:
            oglType = GL_ELEMENT_ARRAY_BUFFER;
            log( "glObjectLabel GL_ELEMENT_ARRAY_BUFFER id: " + String( objectId ) + ", name: " + name );
            bindBuffer( BufferTypes::ELEMENT_ARRAY_BUFFER, objectId );
            break;
        case EObjectType::SHADER:
            oglType = GL_SHADER;
            log( "glObjectLabel GL_SHADER id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::PROGRAM:
            oglType = GL_PROGRAM;
            log( "glObjectLabel GL_PROGRAM id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::VERTEX_ARRAY:
            oglType = GL_VERTEX_ARRAY;
            bindBuffer( BufferTypes::VERTEX_ARRAY, objectId );
            log( "glObjectLabel VERTEX_ARRAY id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::QUERY:
            oglType = GL_QUERY;
            log( "glObjectLabel GL_QUERY id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::PROGRAM_PIPELINE:
            oglType = GL_PROGRAM_PIPELINE;
            log( "glObjectLabel GL_PROGRAM_PIPELINE id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::TRANSFORM_FEEDBACK:
            oglType = GL_TRANSFORM_FEEDBACK;
            log( "glObjectLabel GL_TRANSFORM_FEEDBACK id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::SAMPLER:
            oglType = GL_SAMPLER;
            log( "glObjectLabel GL_SAMPLER id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::TEXTURE:
            oglType = GL_TEXTURE;
            bindTexture( objectId );
            log( "glObjectLabel GL_TEXTURE id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::RENDERBUFFER:
            oglType = GL_RENDERBUFFER;
            log( "glObjectLabel GL_RENDERBUFFER id: " + String( objectId ) + ", name: " + name );
            break;
        case EObjectType::FRAMEBUFFER:
            oglType = GL_FRAMEBUFFER;
            log( "glObjectLabel GL_FRAMEBUFFER id: " + String( objectId ) + ", name: " + name );
            break;
        default:
            oglType = 0;
    }

    glObjectLabel( oglType, objectId, -1, name.cStr() );
}
