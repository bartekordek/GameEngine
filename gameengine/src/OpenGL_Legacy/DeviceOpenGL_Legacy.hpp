#pragma once


#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IndexBuffer.hpp"

NAMESPACE_BEGIN( LOGLW )

class DeviceOpenGL_Legacy final: public IRenderDevice
{
public:
protected:
private:
    // Inherited via IRenderDevice
    virtual void* getNativeDevice() override;
    virtual bool isLegacy() override;
    virtual size_t getFrameBufferCount() const override;
    virtual void initDebugUI() override;
    virtual void resetMatrixToIdentity( const MatrixTypes matrix ) override;
    virtual void setViewport( const Viewport& viewport ) override;
    virtual void setOrthogonalPerspective( const Camera& vp ) override;
    virtual void setPerspectiveProjection( const Camera& vp ) override;
    virtual void lookAt( const Camera& vp ) override;
    virtual void lookAt( const std::array<Pos3Dd, 3>& lookAtVec ) override;
    virtual void lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up ) override;
    virtual void prepareFrame() override;
    virtual void finishFrame() override;
    virtual unsigned int createProgram() override;
    virtual void removeProgram( unsigned programId ) override;
    virtual void useProgram( int programId ) override;
    virtual void linkProgram( unsigned programId ) override;
    virtual void validateProgram( unsigned programId ) override;
    virtual int getCurrentProgram() const override;
    virtual unsigned int createShader( const IFile& shaderCode ) override;
    virtual void attachShader( unsigned programId, unsigned shaderId ) override;
    virtual void dettachShader( unsigned programId, unsigned shaderId ) override;
    virtual void removeShader( unsigned shaderId ) override;
    virtual ContextInfo initContextVersion( SDL2W::IWindow* window ) override;
    virtual void setAttribValue( int attributeLocation, float value ) override;
    virtual void setAttribValue( int attributeLocation, int value ) override;
    virtual void setAttribValue( int attributeLocation, unsigned value ) override;
    virtual void setAttribValue( int attributeLocation, bool value ) override;
    virtual void setAttribValue( int attributeLocation, const CUL::String& value ) override;
    virtual void setUniformValue( int uniformLocation, float value ) override;
    virtual void setUniformValue( int uniformLocation, int value ) override;
    virtual void setUniformValue( int uniformLocation, unsigned value ) override;
    virtual void setUniformValue( int uniformLocation, const glm::vec2& val ) override;
    virtual void setUniformValue( int uniformLocation, const glm::vec3& val ) override;
    virtual void setUniformValue( int uniformLocation, const glm::vec4& val ) override;
    virtual void setUniformValue( int uniformLocation, const glm::mat2& val ) override;
    virtual void setUniformValue( int uniformLocation, const glm::mat3& val ) override;
    virtual void setUniformValue( int uniformLocation, const glm::mat4& val ) override;
    virtual void setProjectionAndModelToIdentity() override;
    virtual void clearColorAndDepthBuffer() override;
    virtual void createQuad( float scale = 1.0f ) override;
    virtual void clearColorTo( const ColorS color ) override;
    virtual void clearBuffer( const ClearMasks mask ) override;
    virtual unsigned int generateVertexArray( const int size = 1 ) override;
    virtual void bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type ) override;
    virtual void bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type ) override;
    virtual void bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type ) override;
    virtual void bufferData( uint8_t bufferId, const float vertices[], BufferTypes type ) override;
    virtual void bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type ) override;
    virtual void bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data ) override;
    virtual void setClientState( ClientStateTypes cs, bool enabled ) override;
    virtual void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
    virtual void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
    virtual void setVertexArrayClientState( const bool enable ) override;
    virtual void setColorClientState( bool enable ) override;
    virtual unsigned int generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size = 1 ) override;
    virtual unsigned int generateAndBindBuffer( const BufferTypes bufferType, const int size = 1 ) override;
    virtual void deleteBuffer( BufferTypes bufferType, unsigned& id ) override;
    virtual void enableVertexAttribiute( unsigned programId, const String& attribName ) override;
    virtual void disableVertexAttribiute( unsigned programId, const String& attribName ) override;
    virtual int getAttribLocation( unsigned programId, const String& attribName ) override;
    virtual int getUniformLocation( unsigned programId, const String& attribName ) override;
    virtual void unbindBuffer( const BufferTypes bufferType ) override;
    virtual void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) override;
    virtual unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) override;
    virtual void drawElements( const PrimitiveType type, const std::vector<unsigned int>& data ) override;
    virtual void drawElements( const PrimitiveType type, const std::vector<float>& data ) override;
    virtual void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) override;
    virtual void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) override;
    virtual void vertexAttribPointer( const VertexAttributePtrMeta& meta ) override;
    virtual void enableVertexAttribArray( unsigned attributeId ) override;
    virtual void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) override;
    virtual std::vector<std::string> listExtensions() override;
    virtual void draw( const QuadCUL& quad, const QuadCUL& texQuad ) override;
    virtual void draw( const QuadCUL& quad, const ColorS& color ) override;
    virtual void draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color ) override;
    virtual void draw( const QuadCUL& quad, const QuadColors& color ) override;
    virtual void draw( const QuadData& quad, const ColorS& color ) override;
    virtual void draw( const QuadData& quad, const std::array<ColorS, 4>& color ) override;
    virtual void draw( const TriangleCUL& triangle, const ColorS& color ) override;
    virtual void draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color ) override;
    virtual void draw( const TriangleData& values, const std::array<ColorS, 3>& color ) override;
    virtual void draw( const LineData& values, const ColorS& color ) override;
    virtual void draw( const LineData& values, const LineColors& color ) override;
    virtual void draw( const CUL::MATH::Primitives::Line& values, const ColorS& color ) override;
    virtual void draw( const Point& position, const ColorS& color ) override;
    virtual void draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color ) override;
    virtual void draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color ) override;
    virtual void translate( const Point& point ) override;
    virtual void translate( const float x, const float y, const float z ) override;
    virtual void rotate( const CUL::MATH::Rotation& rotation ) override;
    virtual void rotate( const float angleDeg, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f ) override;
    virtual void scale( const CUL::MATH::Vector3Df& scale ) override;
    virtual void scale( const float scale ) override;
    virtual void setDepthTest( const bool enabled ) override;
    virtual void setBackfaceCUll( const bool enabled ) override;
    virtual void setTexuring( const bool enabled ) override;
    virtual unsigned generateTexture() override;
    virtual void setActiveTextureUnit( ETextureUnitIndex textureUnitIndex ) override;
    virtual void bindTexture( const unsigned int textureId ) override;
    virtual void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
    void setTextureData( uint8_t textureId, const TextureInfo& ti ) override;
    void freeTexture( unsigned int& textureId ) override;
    void matrixStackPush() override;
    void matrixStackPop() override;
    unsigned getGPUTotalAvailableMemoryKb() override;
    unsigned getGPUCurrentAvailableMemoryKb() override;
    void toggleDebugOutput( bool enable ) override;
    void checkLastCommandForErrors() override;

    const String& getName() const override;
    SDL2W::RenderTypes::RendererType getType() const override;

    String m_name = "OpenGL Legacy.";
};

NAMESPACE_END( LOGLW )