#pragma once

#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IndexBuffer.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"

NAMESPACE_BEGIN( LOGLW )

#undef log

class VertexArray;
class VertexBuffer;

enum class ShaderTypes : int
{
    FRAGMENT_SHADER = 0x8B30,
    VERTEX_SHADER = 0x8B31,
    GEOMETRY_SHADER = 0x8DD9,
    INVALID = 0x0500
};

struct PrimitivePacket
{
    PrimitiveType PT = PrimitiveType::NONE;
    std::vector<std::uint8_t> Data;
    VertexArray* VAO = nullptr;
    VertexBuffer* VBO = nullptr;
};

class DeviceOpenGL final: public IRenderDevice
{
public:
    DeviceOpenGL( bool forceLegacy );

    ~DeviceOpenGL();

protected:
private:
    ContextInfo initContextVersion( LOGLW::IWindow* window ) override;
    void initDebugUI() override;

    void setOrthogonalPerspective( const Camera& vp ) override;
    void setPerspectiveProjection( const Camera& vp ) override;

    void setViewport( const Viewport& viewport ) override;
    void lookAt( const Camera& vp ) override;
    void lookAt( const std::array<Pos3Dd, 3>& lookAtVec ) override;
    void lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up ) override;

    static ShaderTypes getShaderType( const CUL::String& fileExtension );

    std::uint32_t createProgram( const CUL::String& name ) override;
    void removeProgram( unsigned programId ) override;
    void linkProgram( unsigned programId ) override;
    void validateProgram( std::uint32_t programId ) override;

    ShaderUnit* createShaderUnit( const CUL::FS::Path& shaderPath, bool assertOnErrors, CUL::String& errorMessage ) override;
    ShaderUnit* createShaderUnitForce( const CUL::FS::Path& shaderPath, bool assertOnErrors, CUL::String& errorMessage ) override;
    void deleteShaderUnit( ShaderUnit* inShaderUnit ) override;
    bool attachShader( unsigned programId, unsigned shaderId ) override;
    void dettachShader( unsigned programId, unsigned shaderId ) override;
    void removeShader( unsigned shaderId ) override;
    void useProgram( int programId ) override;
    int getCurrentProgram() const override;

    void setAttribValue( int attributeLocation, int value ) override;
    void setAttribValue( int attributeLocation, unsigned value ) override;
    void setAttribValue( int attributeLocation, bool value ) override;
    void setAttribValue( int attributeLocation, const CUL::String& value ) override;

    UniformValue getUniformValue( std::int32_t inProgramId, std::int32_t inUniformId, DataType inDataType ) override;

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

    void bufferData( BufferDataId bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type ) override;
    void bufferData( BufferDataId bufferId, const CUL::DataWrapper& data, const BufferTypes type ) override;
    void bufferData( BufferDataId bufferId, const std::vector<float>& data, const BufferTypes type ) override;
    void bufferData( BufferDataId bufferId, const std::vector<TextureData2D>& data, const BufferTypes type ) override;
    void bufferData( BufferDataId bufferId, const float vertices[], BufferTypes type ) override;
    void bufferDataImpl( const void* data, const GLenum target, const GLsizeiptr dataSize );
    void bufferSubdata( BufferDataId bufferId, const BufferTypes type, std::vector<TextureData2D>& data ) override;
    void updateTextureData( const TextureInfo& ti, void* data ) override;

    void setUniformValue( int uniformLocation, float value ) override;
    void setUniformValue( int uniformLocation, int value ) override;
    void setUniformValue( int uniformLocation, unsigned value ) override;

    unsigned int generateAndBindBuffer( const BufferTypes bufferType, const int size = 1 ) override;
    void deleteBuffer( BufferTypes bufferType, unsigned& id ) override;
    unsigned int generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size = 1 ) override;
    void enableVertexAttribiute( unsigned programId, const String& attribName ) override;
    void disableVertexAttribiute( unsigned programId, const String& attribName ) override;
    int getAttribLocation( unsigned programId, const String& attribName ) override;
    void unbindBuffer( const BufferTypes bufferType ) override;
    // void bindBuffer( VertexArray* vao )  override;
    void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) override;
    unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) override;

    void drawElements( const PrimitiveType type, const CUL::DataWrapper& inData ) override;
    void drawElements( const PrimitiveType type, const std::vector<float>& data ) override;
    void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) override;
    void enableVertexAttribArray( unsigned attributeId ) override;
    void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) override;

    std::vector<std::string> listExtensions() override;

    void resetMatrixToIdentity( const MatrixTypes matrix ) override;

    void translate( const Point& point ) override;
    void translate( const float x, const float y, const float z ) override;

    void scale( const CUL::MATH::Vector3Df& scale ) override;
    void scale( const float scale ) override;

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

    void draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color ) override;

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

    int getUniformLocation( unsigned programId, const String& attribName ) override;

    void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) override;

    void vertexAttribPointer( const VertexData& meta ) override;

    void setTextureData( std::uint32_t textureId, const TextureInfo& ti ) override;

    void rotate( const CUL::MATH::Rotation& rotation ) override;
    void draw( const CUL::MATH::Primitives::Line& values, const ColorS& color ) override;

    void rotate( const float angleDeg, const float x, const float y, const float z ) override;

    // Texturing
    void setTexuring( const bool enabled ) override;
    unsigned generateTexture() override;
    void bindTexture( const unsigned int textureId ) override;
    void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
    void freeTexture( std::uint32_t textureId ) override;

    void matrixStackPush() override;
    void matrixStackPop() override;

    bool m_legacy = false;

    unsigned int m_currentMatrix = 0;
    uint8_t m_lastTextureId = 0u;

    void toggleDebugOutput( bool enable ) override;
    bool getLastOperationStatus();

    unsigned getGPUTotalAvailableMemoryKb() override;
    unsigned getGPUCurrentAvailableMemoryKb() override;

    void* getNativeDevice() override;
    bool isLegacy() override;

    void checkLastCommandForErrors() override;
    void prepareFrame() override;
    void finishFrame() override;

    void setObjectName( EObjectType objectType, std::uint32_t objectId, const CUL::String& name ) override;

    DeviceOpenGL( const DeviceOpenGL& arg ) = delete;
    DeviceOpenGL( DeviceOpenGL&& arg ) = delete;
    DeviceOpenGL& operator=( const DeviceOpenGL& rhv ) = delete;
    DeviceOpenGL& operator=( DeviceOpenGL&& rhv ) = delete;

    size_t getFrameBufferCount() const override;

    const String& getName() const override;
    LOGLW::RenderTypes::RendererType getType() const override;

    String m_name = "OpenGL Modern.";

private:
    ShaderUnit* findShader( const CUL::FS::Path& shaderPath ) const;
    std::vector<AttributeInfo> fetchProgramAttributeInfo( std::int32_t inProgramId ) const override;
    std::vector<UniformInfo> fetchProgramUniformsInfo( std::int32_t inProgramId ) const override;
    std::unordered_map<String, std::unique_ptr<ShaderUnit>, CUL::StringHash> m_shadersUnits;
    mutable std::mutex m_shadersMtx;
};

NAMESPACE_END( LOGLW )
