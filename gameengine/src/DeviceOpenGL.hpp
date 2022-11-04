#pragma once

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IndexBuffer.hpp"

NAMESPACE_BEGIN( LOGLW )

#undef log

enum class ShaderTypes : int
{
    FRAGMENT_SHADER = 0x8B30,
    VERTEX_SHADER = 0x8B31,
    GEOMETRY_SHADER = 0x8DD9,
    INVALID = 0x0500
};

class DeviceOpenGL final: public IRenderDevice
{
public:
    DeviceOpenGL( CUL::CULInterface* culInterface, bool forceLegacy );

    ~DeviceOpenGL();

protected:
private:
    ContextInfo initContextVersion( SDL2W::IWindow* window ) override;

    void setOrthogonalPerspective( const Camera& vp );
    void setPerspectiveProjection( const Camera& vp );

    void setProjection( const Camera& rect ) override;
    void setViewport( const Viewport& viewport ) override;
    void lookAt( const Camera& vp ) override;
    void lookAt( const std::array<Pos3Dd, 3>& lookAtVec ) override;
    void lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up ) override;

    ShaderTypes getShaderType( const CUL::String& fileExtension );

    unsigned int createProgram() override;
    void removeProgram( unsigned programId ) override;
    void linkProgram( unsigned programId ) override;
    void validateProgram( unsigned programId ) override;

    unsigned int createShader( const IFile& shaderCode ) override;
    void attachShader( unsigned programId, unsigned shaderId ) override;
    void dettachShader( unsigned programId, unsigned shaderId ) override;
    void removeShader( unsigned shaderId ) override;
    void useProgram( int programId ) override;
    int getCurrentProgram() const override;
    int m_currentProgram = 0;

    void setAttribValue( int attributeLocation, float value ) override;
    void setAttribValue( int attributeLocation, int value ) override;
    void setAttribValue( int attributeLocation, unsigned value ) override;
    void setAttribValue( int attributeLocation, bool value ) override;
    void setAttribValue( int attributeLocation, const CUL::String& value ) override;

    void setProjectionAndModelToIdentity() override;
    void clearColorAndDepthBuffer() override;
    void createQuad( float scale = 1.0f ) override;
    void clearColorTo( const ColorS color ) override;
    void clearBuffer( const ClearMasks mask ) override;

    void setClientState( ClientStateTypes cs, bool enabled ) override;

    void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
    void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;

    // VBO, VAO
    void setVertexArrayClientState( const bool enable ) override;
    void setColorClientState( bool enable ) override;
    unsigned int generateVertexArray( const int size = 1 ) override;

    void bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const float vertices[], BufferTypes type ) override;

    void bufferDataImpl( const void* data, const GLenum target, const GLsizeiptr dataSize );
    void bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data ) override;

    void setUniformValue( int uniformLocation, float value ) override;
    void setUniformValue( int uniformLocation, int value ) override;
    void setUniformValue( int uniformLocation, unsigned value ) override;

    unsigned int generateAndBindBuffer( const BufferTypes bufferType, const int size = 1 ) override;
    void deleteBuffer( BufferTypes bufferType, unsigned& id ) override;
    unsigned int generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size = 1 ) override;
    void enableVertexAttribiute( unsigned programId, const String& attribName ) override;
    void disableVertexAttribiute( unsigned programId, const String& attribName ) override;
    unsigned int getAttribLocation( unsigned programId, const String& attribName ) override;
    void unbindBuffer( const BufferTypes bufferType ) override;
    // void bindBuffer( VertexArray* vao )  override;
    void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) override;
    unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) override;

    void drawElements( const PrimitiveType type, const std::vector<unsigned int>& data ) override;
    void drawElements( const PrimitiveType type, const std::vector<float>& data ) override;
    void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) override;
    void enableVertexAttribArray( unsigned attributeId ) override;
    void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) override;

    std::vector<std::string> listExtensions() override;

    void resetMatrixToIdentity( const MatrixTypes matrix ) override;

    void translate( const Point& point ) override;
    void translate( const float x, const float y, const float z ) override;

    virtual void scale( const CUL::MATH::Vector3Df& scale ) override;
    virtual void scale( const float scale ) override;

    void draw( const TriangleCUL& triangle, const ColorS& color ) override;
    void draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color ) override;
    void draw( const TriangleData& values, const std::array<ColorS, 3>& color ) override;

    void draw( const QuadCUL& quad, const QuadCUL& texQuad ) override;
    void draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color ) override;

    void draw( const QuadCUL& quad, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const std::array<ColorS, 4>& color ) override;

    void draw( const QuadData& quad, const ColorS& color ) override;
    void draw( const QuadData& quad, const QuadColors& color ) override;

    void draw( const LineData& values, const LineColors& color ) override;
    void draw( const LineData& values, const ColorS& color ) override;

    void draw( const Point& position, const ColorS& color ) override;

    void assertOnProgramError( unsigned programId, unsigned val );
    void setDepthTest( const bool enabled ) override;
    void setBackfaceCUll( const bool enabled ) override;

    void setUniformValue( int uniformLocation, const glm::vec2& val ) override;
    void setUniformValue( int uniformLocation, const glm::vec3& val ) override;
    void setUniformValue( int uniformLocation, const glm::vec4& val ) override;

    void setUniformValue( int uniformLocation, const glm::mat2& val ) override;
    void setUniformValue( int uniformLocation, const glm::mat3& val ) override;
    void setUniformValue( int uniformLocation, const glm::mat4& val ) override;

    void setActiveTextureUnit( ETextureUnitIndex textureUnitIndex ) override;

    unsigned int getUniformLocation( unsigned programId, const String& attribName ) override;

    void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) override;

    void vertexAttribPointer( const VertexAttributePtrMeta& meta ) override;

    void setTextureData( uint8_t textureId, const TextureInfo& ti ) override;

    void rotate( const CUL::MATH::Rotation& rotation ) override;
    void draw( const CUL::MATH::Primitives::Line& values, const ColorS& color ) override;
    void rotate( const float angleDeg, const float x, const float y, const float z ) override;

    // Texturing
    void setTexuring( const bool enabled ) override;
    unsigned generateTexture() override;
    void bindTexture( const unsigned int textureId ) override;
    void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
    void freeTexture( unsigned int& textureId ) override;

    void matrixStackPush() override;
    void matrixStackPop() override;

    bool m_legacy = false;

    unsigned int m_currentMatrix = 0;
    uint8_t m_lastTextureId = 0u;

    void toggleDebugOutput( bool enable ) override;
    void getLastOperationStatus();

    unsigned getGPUTotalAvailableMemoryKb() override;
    unsigned getGPUCurrentAvailableMemoryKb() override;

    bool isLegacy() override;

    void checkLastCommandForErrors() override;

    DeviceOpenGL( const DeviceOpenGL& arg ) = delete;
    DeviceOpenGL( DeviceOpenGL&& arg ) = delete;
    DeviceOpenGL& operator=( const DeviceOpenGL& rhv ) = delete;
    DeviceOpenGL& operator=( DeviceOpenGL&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )