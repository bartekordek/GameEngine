#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IRenderDevice.hpp"

struct ImGuiContext;

NAMESPACE_BEGIN( LOGLW )

class DeviceDX09 final: public IRenderDevice
{
public:
    DeviceDX09();
    ~DeviceDX09();

protected:
private:

    void* getNativeDevice() override;

    bool isLegacy() override;

    // Inherited via IRenderDevice
    void resetMatrixToIdentity( const MatrixTypes matrixIn ) override;
    void setViewport( const Viewport& viewport ) override;
    void setOrthogonalPerspective( const Camera& vp ) override;
    void setPerspectiveProjection( const Camera& vp ) override;
    std::uint32_t createProgram( const CUL::String& name ) override;
    void removeProgram( unsigned programId ) override;
    void useProgram( int programId ) override;
    void linkProgram( unsigned programId ) override;
    void validateProgram( unsigned programId ) override;
    int getCurrentProgram() const override;
    void attachShader( unsigned programId, unsigned shaderId ) override;
    void dettachShader( unsigned programId, unsigned shaderId ) override;
    void removeShader( unsigned shaderId ) override;
    ContextInfo initContextVersion( SDL2W::IWindow* window ) override;

    void createDescriptorHeaps();
    void createRenderTargetViews();
    void createRootSignature();
    void createShaders();
    void createVertexBuffer();
    void createSwapChain();

    void createCommandQueue();

    void setViewportSize();
    void setScissorRect();
    void initInterfaces();
    void createDXDevice();
    void setAttribValue( int attributeLocation, float value ) override;
    void setAttribValue( int attributeLocation, int value ) override;
    void setAttribValue( int attributeLocation, unsigned value ) override;
    void setAttribValue( int attributeLocation, bool value ) override;
    void setAttribValue( int attributeLocation, const CUL::String& value ) override;
    void setUniformValue( int uniformLocation, float value ) override;
    void setUniformValue( int uniformLocation, int value ) override;
    void setUniformValue( int uniformLocation, unsigned value ) override;
    void setUniformValue( int uniformLocation, const glm::vec2& val ) override;
    void setUniformValue( int uniformLocation, const glm::vec3& val ) override;
    void setUniformValue( int uniformLocation, const glm::vec4& val ) override;
    void setUniformValue( int uniformLocation, const glm::mat2& val ) override;
    void setUniformValue( int uniformLocation, const glm::mat3& val ) override;
    void setUniformValue( int uniformLocation, const glm::mat4& val ) override;
    void setProjectionAndModelToIdentity() override;
    void clearColorAndDepthBuffer() override;
    void createQuad( float scale = 1.0f ) override;
    void clearColorTo( const ColorS color ) override;
    void clearBuffer( const ClearMasks mask ) override;
    unsigned int generateVertexArray( const int size = 1 ) override;
    void setClientState( ClientStateTypes cs, bool enabled ) override;
    void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
    void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
    void setVertexArrayClientState( const bool enable ) override;
    void setColorClientState( bool enable ) override;
    void enableVertexAttribiute( unsigned programId, const String& attribName ) override;
    void disableVertexAttribiute( unsigned programId, const String& attribName ) override;
    int getAttribLocation( unsigned programId, const String& attribName ) override;
    int getUniformLocation( unsigned programId, const String& attribName ) override;
    void unbindBuffer( const BufferTypes bufferType ) override;
    void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) override;
    unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) override;
    void drawElements( const PrimitiveType type, const std::vector<unsigned int>& data ) override;
    void drawElements( const PrimitiveType type, const std::vector<float>& data ) override;
    void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) override;
    void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) override;
    void vertexAttribPointer( const VertexData& meta ) override;
    void enableVertexAttribArray( unsigned attributeId ) override;
    void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) override;
    void draw( const QuadCUL& quad, const QuadCUL& texQuad ) override;
    void draw( const QuadCUL& quad, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const QuadColors& color ) override;
    void draw( const QuadData& quad, const ColorS& color ) override;
    void draw( const QuadData& quad, const std::array<ColorS, 4>& color ) override;
    void draw( const TriangleCUL& triangle, const ColorS& color ) override;
    void draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color ) override;
    void draw( const TriangleData& values, const std::array<ColorS, 3>& color ) override;
    void draw( const LineData& values, const ColorS& color ) override;
    void draw( const LineData& values, const LineColors& color ) override;
    void draw( const CUL::MATH::Primitives::Line& values, const ColorS& color ) override;
    void draw( const Point& position, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color ) override;
    void draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color ) override;
    void translate( const Point& point ) override;
    void translate( const float x, const float y, const float z ) override;
    void rotate( const CUL::MATH::Rotation& rotation ) override;
    void rotate( const float angleDeg, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f ) override;
    void scale( const CUL::MATH::Vector3Df& scale ) override;
    void scale( const float scale ) override;
    void setDepthTest( const bool enabled ) override;
    void setBackfaceCUll( const bool enabled ) override;
    void setTexuring( const bool enabled ) override;
    void setActiveTextureUnit( ETextureUnitIndex textureUnitIndex ) override;
    void bindTexture( const unsigned int textureId ) override;
    void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
    void updateTextureData( const TextureInfo& ti, void* data ) override;
    void matrixStackPush() override;
    void matrixStackPop() override;
    void toggleDebugOutput( bool enable ) override;
    void checkLastCommandForErrors() override;

    void enableDebugLayers();

private:
    void initDebugUI() override;
    bool m_useWarpDevice = false;
    SDL2W::IWindow* m_window = nullptr;

    void WaitForPreviousFrame();
    void prepareFrame() override;
    void finishFrame() override;
    size_t getFrameBufferCount() const override;


    ImGuiContext* m_imguiContext = nullptr;

    const String& getName() const override;
    SDL2W::RenderTypes::RendererType getType() const override;

    String m_name = "DirectX 09.";
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS