#include "gameengine/IUtility.hpp"
#include "gameengine/Camera.hpp"

#include "SDL2Wrapper/IWindow.hpp"

#include "CUL/CULInterface.hpp"
#include "CUL/Threading/ThreadUtils.hpp"

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"

using namespace LOGLW;

static CUL::LOG::ILogger* g_logger = nullptr;

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#if _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4800 )
#endif // #if _MSC_VER
const String TextureInfo::toString() const
{
    String result;

    result = "textureId: " + String( textureId );
    result += ", level: " + String( level );
    result += ", pixelFormat = " + String( (unsigned)pixelFormat );
    result += ", border = " + String( border );
    result += ", dataType = " + String( (int)dataType );
    result += ", data = " + String( (void*)data );
    result += ", size = " + size.toString();

    return result;
}
#if _MSC_VER
#pragma warning( pop )
#endif // #if _MSC_VER

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                             const char*  // message
                             ,
                             const void*  // userParam
);

void APIENTRY glDebugOutput(
    GLenum source,
    GLenum type,
    unsigned int id,
    GLenum severity,
    GLsizei /*length*/,
    const char* /*message*/,
    const void* /*userParam*/
)
{
    if( id == 131185 )
    {
        return;
    }

    switch( severity )
    {
        case GL_DEBUG_SEVERITY_LOW:
            g_logger->log( "Source: API", CUL::LOG::Severity::INFO );
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            g_logger->log( "Source: API", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            g_logger->log( "Source: API", CUL::LOG::Severity::WARN );
            break;
    }

    switch( source )
    {
        case GL_DEBUG_SOURCE_API:
            g_logger->log( "Source: API", CUL::LOG::Severity::WARN);
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            g_logger->log( "Source: Window System", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            g_logger->log( "Source: Shader Compiler", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            g_logger->log( "Source: Third Party", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            g_logger->log( "Source: Application", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SOURCE_OTHER:
            g_logger->log( "Source: Other", CUL::LOG::Severity::WARN );
            break;
    }

    switch( type )
    {
        case GL_DEBUG_TYPE_ERROR:
            g_logger->log( "Type: Error", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            g_logger->log( "Type: Deprecated Behaviour", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            g_logger->log( "Type: Undefined Behaviour", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            g_logger->log( "Type: Portability", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            g_logger->log( "Type: Performance", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_MARKER:
            g_logger->log( "Type: Marker", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            g_logger->log( "Type: Push Group", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            g_logger->log( "Type: Pop Group", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_TYPE_OTHER:
            g_logger->log( "Type: Other", CUL::LOG::Severity::WARN );
            break;
    }

    switch( severity )
    {
        case GL_DEBUG_SEVERITY_HIGH:
            g_logger->log( "Severity: high", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            g_logger->log( "Severity: medium", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SEVERITY_LOW:
            g_logger->log( "Severity: low", CUL::LOG::Severity::WARN );
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            g_logger->log( "Severity: notification", CUL::LOG::Severity::WARN );
            break;
    }
}


IUtility::IUtility( CUL::CULInterface* culInterface, bool forceLegacy )
    : m_forceLegacy( forceLegacy ), m_culInterface( culInterface ), m_logger( m_culInterface->getLogger() )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        //CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glGetIntegerv( GL_MAJOR_VERSION, &m_supportedVersion.major );
    glGetIntegerv( GL_MINOR_VERSION, &m_supportedVersion.minor );

    const std::string version = (char*)glGetString( GL_VERSION );
    log( "OpenGL Version: " + version );
    g_logger = culInterface->getLogger();
}

ContextInfo IUtility::initContextVersion( SDL2W::IWindow* window )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        getCUl()->getThreadUtils().setCurrentThreadName( "RenderThread" );
        if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo("RenderThread") )
        {
            CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
        }
    }

    ContextInfo result;

    result.glContext = window->createContext();
    /*
    Context version can be only set after context creation.
    I.e. SDL: SDL_GL_DeleteContext call.
    */
    // SDL_GL_DOUBLEBUFFER
    window->toggleDoubleBuffer( true );
    window->setDepthSize( 24 );
    window->setStencilSize( 8 );

    window->setGLContextVersion( m_supportedVersion.major, m_supportedVersion.minor );
    window->setProfileMask( SDL2W::GLProfileMask::CORE );

    // Set debug otuput.
    window->setContextFlag( SDL2W::GLContextFlag::DEBUG_FLAG );
    // Possible values:
    // typedef enum
    //{
    //    SDL_GL_CONTEXT_PROFILE_CORE = 0x0001,
    //    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY = 0x0002,
    //    SDL_GL_CONTEXT_PROFILE_ES = 0x0004 /**<
    //    GLX_CONTEXT_ES2_PROFILE_BIT_EXT */
    //} SDL_GLprofile;
    const auto glStringVersion = glGetString( GL_VERSION );
    result.glVersion = glStringVersion;

    if( true )
    {
        auto error = glewInit();
        CUL::Assert::simple( GLEW_OK == error, "GLEW error: " + CUL::String( reinterpret_cast<const char*>( glewGetErrorString( error ) ) +
                                                                             result.glVersion ) );

        if( glDebugMessageCallbackARB )
        {
            glDebugMessageCallbackARB( glDebugOutput, nullptr );

            log( "Debug message enabled.", CUL::LOG::Severity::WARN );
        }
    }
    return result;
}

void IUtility::getLastOperationStatus()
{
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

        log(error);
    }
}

void IUtility::setOrthogonalPerspective( const Camera& camera )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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


void IUtility::useProgram( int programId )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( m_currentProgram != programId )
    {
        glUseProgram( static_cast<GLuint>( programId ) );
        m_currentProgram = programId;
    }
}

int IUtility::getCurrentProgram() const
{
    GLint id;
    glGetIntegerv( GL_CURRENT_PROGRAM, &id );

    return (int)id;
}

void IUtility::setUniformValue( int uniformLocation, const glm::vec2& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniform2fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::vec3& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniform3fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::vec4& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniform4fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat2& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniformMatrix2fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat3& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat4& val )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    //log( "setUniformValue" );
    glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setActiveTexture( unsigned id )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "setActiveTexture" );
    glActiveTexture(static_cast<GLenum>(id));
}

unsigned int IUtility::getUniformLocation( unsigned programId, const String& attribName )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glGetUniformLocation( " + String( programId ) + ", " + attribName + " );" );

    auto attribLocation = glGetUniformLocation( programId, attribName.cStr() );

    if( attribLocation == -1 )
    {
        log( "DID NOT FOUND!" );
    }

    const GLenum err = glGetError();
    const GLubyte* errorAsString = gluErrorString( err );

    if( err != GL_NO_ERROR )
    {
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

    return static_cast<unsigned int>( attribLocation );
}

void IUtility::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
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
    log( "drawArrays" );
    bindBuffer( BufferTypes::VERTEX_ARRAY, vaoId );
    glDrawArrays( static_cast<GLenum>( primitiveType ), static_cast<GLint>( first ), static_cast<GLsizei>( count ) );
}

void IUtility::vertexAttribPointer( const VertexAttributePtrMeta& meta )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    bindBuffer(BufferTypes::VERTEX_ARRAY, meta.vao);
    bindBuffer(BufferTypes::ARRAY_BUFFER, meta.vbo);

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
    log( "glVertexAttribPointer" );
    glVertexAttribPointer( static_cast<GLuint>( meta.vertexAttributeId ), static_cast<GLint>( meta.componentsPerVertexAttribute ),
                           static_cast<GLenum>( meta.dataType ), static_cast<GLboolean>( meta.normalized ),
                           static_cast<GLsizei>( meta.stride ),
                           meta.offset );
}

void IUtility::setTextureData( uint8_t textureId, const TextureInfo& ti )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    log( "glTexImage2D( " + ti.toString() + " )" );
    bindTexture( textureId );
    glTexImage2D( GL_TEXTURE_2D, ti.level, (GLint)ti.pixelFormat, ti.size.width, ti.size.height, ti.border, (GLenum)ti.pixelFormat,
                  (GLenum)GL_UNSIGNED_BYTE, ti.data );
}


void IUtility::rotate( const CUL::MATH::Rotation& rotation )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glRotatef( rotation.yaw.getDeg(), 0.f, -1.f, 0.f );
    glRotatef( rotation.pitch.getDeg(), 1.f, 0.f, 0.f );
    glRotatef( rotation.roll.getDeg(), 0.f, 0.f, 1.f );
}

void IUtility::rotate( const float angleDeg, const float x, const float y, const float z )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    glRotatef( angleDeg, x, y, z );
}

CUL::GUTILS::Version IUtility::getVersion() const
{
    return m_supportedVersion;
}

CUL::CULInterface* IUtility::getCUl() const
{
    return m_culInterface;
}

unsigned IUtility::getGPUTotalAvailableMemoryKb()
{
    GLint total_mem_kb = 0;
    glGetIntegerv( GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &total_mem_kb );
    return total_mem_kb;
}

unsigned IUtility::getGPUCurrentAvailableMemoryKb()
{
    GLint cur_avail_mem_kb = 0;
    glGetIntegerv( GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &cur_avail_mem_kb );
    return cur_avail_mem_kb;
}

void IUtility::toggleDebugOutput( bool enable )
{
    if( !getCUl()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        CUL::Assert::simple( false, "NOT IN THE RENDER THREAD." );
    }

    if( enable )
    {
        glEnable( GL_DEBUG_OUTPUT );
        glDebugMessageCallback( glDebugOutput, 0 );
    }
    else
    {
        glDisable( GL_DEBUG_OUTPUT );
    }
}

void IUtility::log( const String& text, const CUL::LOG::Severity severity ) const
{
    customAssert( m_logger != nullptr, "Logger utility is unninitialized inside of UtilConcrete." );

    if( m_lastLog != text )
    {
        m_logger->log( text, severity );
        m_lastLog = text;
    }
}

void IUtility::customAssert( const bool value, const CUL::String& message ) const
{
    CUL::Assert::simple( value, message );
}


IUtility::~IUtility()
{
}