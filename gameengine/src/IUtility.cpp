#include "gameengine/IUtility.hpp"

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"
#include "SDL2Wrapper/IMPORT_SDL_opengl.hpp"

using namespace LOGLW;

IUtility::IUtility()
{
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
    log( "glDrawArrays" );
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

IUtility::~IUtility()
{
}