#include "gameengine/IUtility.hpp"

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"
#include "SDL2Wrapper/IMPORT_SDL_opengl.hpp"

using namespace LOGLW;

static CUL::LOG::ILogger* g_logger = nullptr;

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length,
                             const char*  // message
                             ,
                             const void*  // userParam
);

void APIENTRY glDebugOutput( GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei  // length
                             ,
                             const char*  // message
                             ,
                             const void*  // userParam
)
{
    if( id == 131169 || id == 131185 || id == 131218 || id == 131204 )
        return;

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


IUtility::IUtility( CUL::CULInterface* culInterface ) : m_culInterface( culInterface ), m_logger(m_culInterface->getLogger())
{
    glGetIntegerv( GL_MAJOR_VERSION, &m_supportedVersion.major );
    glGetIntegerv( GL_MINOR_VERSION, &m_supportedVersion.minor );

    const std::string version = (char*)glGetString( GL_VERSION );
    log( "OpenGL Version: " + version );
    g_logger = culInterface->getLogger();
}

ContextInfo IUtility::initContextVersion( SDL2W::IWindow* window )
{
    ContextInfo result;

    result.glContext = SDL_GL_CreateContext( *window );
    /*
    Context version can be only set after context creation.
    I.e. SDL: SDL_GL_DeleteContext call.
    */
    // SDL_GL_DOUBLEBUFFER
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, SDL_TRUE );

    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, m_supportedVersion.major );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, m_supportedVersion.minor );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

    // Set debug otuput.
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG );
    // Possible values:
    // typedef enum
    //{
    //    SDL_GL_CONTEXT_PROFILE_CORE = 0x0001,
    //    SDL_GL_CONTEXT_PROFILE_COMPATIBILITY = 0x0002,
    //    SDL_GL_CONTEXT_PROFILE_ES = 0x0004 /**<
    //    GLX_CONTEXT_ES2_PROFILE_BIT_EXT */
    //} SDL_GLprofile;
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG );
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

void IUtility::setUniformValue( int uniformLocation, const glm::vec2& val )
{
    glUniform2fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::vec3& val )
{
    glUniform3fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::vec4& val )
{


    glUniform4fv( uniformLocation, 1, &val[0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat2& val )
{
    glUniformMatrix2fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat3& val )
{
    glUniformMatrix3fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setUniformValue( int uniformLocation, const glm::mat4& val )
{
    glUniformMatrix4fv( uniformLocation, 1, GL_FALSE, &val[0][0] );
}

void IUtility::setActiveTexture( unsigned id )
{
    glActiveTexture(static_cast<GLenum>(id));
}


void IUtility::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
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
    //log( "glDrawArrays" );
    bindBuffer( BufferTypes::VERTEX_ARRAY, vaoId );
    glDrawArrays( static_cast<GLenum>( primitiveType ), static_cast<GLint>( first ), static_cast<GLsizei>( count ) );
}

void IUtility::vertexAttribPointer( const VertexAttributePtrMeta& meta )
{
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

CUL::GUTILS::Version IUtility::getVersion() const
{
    return m_supportedVersion;
}

CUL::CULInterface* IUtility::getCUl() const
{
    return m_culInterface;
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