#pragma once

#include "CUL/Log/ILogger.hpp"
#include "IMPORT_glew.hpp"
#include "ImportFreeglut.hpp"
#include "SDL2Wrapper/IMPORT_SDL_opengl.hpp"
#include "gameengine/IUtility.hpp"
#include "gameengine/IndexBuffer.hpp"

NAMESPACE_BEGIN( LOGLW )

#undef log

class UtilConcrete final: public IUtility
{
public:
    UtilConcrete( CUL::CULInterface* culInterface, bool legacy );

    ~UtilConcrete();

protected:
private:
    bool isLegacy()  override;

    void setProjection( const ProjectionData& rect )  override;
    void setViewport( const Viewport& viewport )  override;
    void setPerspective( const Angle& angle, double widthToHeightRatio,
                         double m_zNear, double m_zFar )  override;
    void setOrthogonalPerspective( const ProjectionData& vp )  override;
    void setPerspectiveProjection( const ProjectionData& vp )  override;
    void lookAt( const ProjectionData& vp )  override;
    void lookAt( const std::array<Pos3Dd, 3>& lookAtVec )  override;
    void lookAt( const Pos3Dd& eye, const Pos3Dd& center,
                 const Pos3Dd& up )  override;

    ShaderTypes getShaderType(
        const CUL::String& fileExtension )  override;

    unsigned int createProgram() override;
    void removeProgram( unsigned programId )  override;
    void useProgram( unsigned programId )  override;
    void linkProgram( unsigned programId )  override;
    void validateProgram( unsigned programId )  override;

    unsigned int createShader( const IFile& shaderCode ) override;
    void attachShader( unsigned programId, unsigned shaderId )  override;
    void dettachShader( unsigned programId, unsigned shaderId )  override;
    void removeShader( unsigned shaderId )  override;

    ContextInfo initContextVersion( SDL2W::IWindow* window, unsigned major,
                                    unsigned minor )  override;
    void destroyContext( ContextInfo& context ) override;
    void setAttribValue( int attributeLocation, float value )  override;
    void setAttribValue( int attributeLocation, int value )  override;
    void setAttribValue( int attributeLocation, unsigned value )  override;
    void setAttribValue( int attributeLocation, bool value )  override;
    void setAttribValue( int attributeLocation,
                         const CUL::String& value )  override;

    void setProjectionAndModelToIdentity()  override;
    void clearColorAndDepthBuffer()  override;
    void createQuad( float scale = 1.0f )  override;
    void clearColorTo( const ColorS color )  override;
    void clearBuffer( const ClearMasks mask )  override;

    void setClientState( ClientStateTypes cs, bool enabled )  override;

    void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )  override;
    void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )  override;

    // VBO, VAO
    void setVertexArrayClientState( const bool enable )  override;
    void setColorClientState( bool enable )  override;
    unsigned int generateVertexArray( const int size = 1 )  override;

    void bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data,
                     const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<unsigned int>& data,
                     const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<float>& data,
                     const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type ) override;

    void bufferData( uint8_t bufferId, const float vertices[], BufferTypes type ) override;

    void bufferDataImpl( const void* data, const GLenum target,
                         const GLsizeiptr dataSize ) ;
    void bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data ) override;

    void setUniformValue( int uniformLocation, float value )  override;
    void setUniformValue( int uniformLocation, int value )  override;
    void setUniformValue( int uniformLocation, unsigned value )  override;

    unsigned int generateAndBindBuffer( const BufferTypes bufferType,
                                        const int size = 1 ) override;
    void deleteBuffer( BufferTypes bufferType, unsigned& id )  override;
    unsigned int generateElementArrayBuffer(
        const std::vector<unsigned int>& data,
        const int size = 1 ) override;
    void enableVertexAttribiute( unsigned programId,
                                 const String& attribName )  override;
    void disableVertexAttribiute( unsigned programId,
                                  const String& attribName )  override;
    unsigned int getAttribLocation( unsigned programId,
                                    const String& attribName )  override;
    unsigned int getUniformLocation( unsigned programId,
                                     const String& attribName )  override;
    void unbindBuffer( const BufferTypes bufferType ) override;
    // void bindBuffer( VertexArray* vao )  override;
    void bindBuffer( const BufferTypes bufferType,
                     unsigned bufferId ) override;
    unsigned int generateBuffer( const BufferTypes type,
                                 const int size = 1 )  override;

    void drawElements( const PrimitiveType type,
                       const std::vector<unsigned int>& data )  override;
    void drawElements( const PrimitiveType type,
                       const std::vector<float>& data )  override;
    void drawElementsFromLastBuffer( const PrimitiveType primitiveType,
                                     const DataType dataType,
                                     unsigned count )  override;
    void enableVertexAttribArray( unsigned attributeId )  override;
    void setVertexPointer( int coordinatesPerVertex, DataType dataType,
                           int stride, const void* data )  override;

    std::vector<std::string> listExtensions() override;

    void resetMatrixToIdentity( const MatrixTypes matrix )  override;

    void translate( const Point& point ) override;
    void translate( const float x, const float y, const float z ) override;
    void rotate( const float angle, const float x = 0.0f, const float y = 0.0f,
                 const float z = 0.0f ) override;

    virtual void scale( const CUL::MATH::Vector3Df& scale )  override;
    virtual void scale( const float scale )  override;

    void draw( const Triangle& triangle, const ColorS& color ) override;
    void draw( const Triangle& quad,
               const std::array<ColorS, 4>& color ) override;
    void draw( const TriangleData& values,
               const std::array<ColorS, 3>& color ) override;

    void draw( const QuadCUL& quad, const QuadCUL& texQuad ) override;

    void draw( const QuadCUL& quad, const ColorS& color ) override;
    void draw( const QuadCUL& quad, const std::array<ColorS, 4>& color ) override;

    void draw( const QuadData& quad, const ColorS& color ) override;
    void draw( const QuadData& quad, const QuadColors& color ) override;

    void draw( const LineData& values, const LineColors& color ) override;
    void draw( const LineData& values, const ColorS& color ) override;

    void draw( const Point& position, const ColorS& color ) override;

    void assertOnProgramError( unsigned programId, unsigned val ) ;
    void log( const String& text, const CUL::LOG::Severity severity =
                                      CUL::LOG::Severity::INFO ) override;
    void customAssert( const bool value, const CUL::String& message ) ;
    void setDepthTest( const bool enabled )  override;
    void setBackfaceCUll( const bool enabled )  override;

    // Texturing
    void setTexuring( const bool enabled )  override;
    unsigned generateTexture()  override;
    void bindTexture( const unsigned int textureId ) override;
    void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
    void setTextureData( uint8_t textureId, const TextureInfo& ti ) override;
    void freeTexture( unsigned int& textureId )  override;

    void matrixStackPush() override;
    void matrixStackPop() override;

    CUL::CULInterface* getCUl() override;

    bool m_legacy = false;

    CUL::CULInterface* m_culInterface = nullptr;
    CUL::LOG::ILogger* m_logger = nullptr;
    unsigned int m_currentMatrix = 0;
    uint8_t m_lastTextureId = 0u;
    String m_lastLog;
    CUL::LOG::Severity m_lastLogSeverity;

    UtilConcrete( const UtilConcrete& arg ) = delete;
    UtilConcrete( UtilConcrete&& arg ) = delete;
    UtilConcrete& operator=( const UtilConcrete& rhv ) = delete;
    UtilConcrete& operator=( UtilConcrete&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )