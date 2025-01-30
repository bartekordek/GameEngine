#pragma once

#include "gameengine/BasicTypes.hpp"
#include "gameengine/Render/RendererTypes.hpp"

#include "CUL/Filesystem/IFile.hpp"
#include "CUL/Graphics/Color.hpp"
#include "CUL/Graphics/Rect2D.hpp"
#include "CUL/Graphics/Pos3D.hpp"
#include "CUL/Math/Angle.hpp"
#include "CUL/Math/Primitives/Line.hpp"
#include "CUL/Math/Primitives/Quad.hpp"
#include "CUL/Math/Primitives/Triangle.hpp"
#include "CUL/Math/Primitives/Triangle3D.hpp"
#include "CUL/Math/Rotation.hpp"
#include "CUL/Memory/DataWrapper.hpp"
#include "CUL/Graphics/SimpleSize2D.hpp"
#include "CUL/Log/ILogger.hpp"
#include "CUL/Graphics/IImage.hpp"

#include "CUL/IMPORT_GLM.hpp"

#include "CUL/GenericUtils/Version.hpp"

#include "CUL/STL_IMPORTS/STD_array.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_variant.hpp"

NAMESPACE_BEGIN( CUL )

class CULInterface;

NAMESPACE_BEGIN( LOG )
class ILogger;
NAMESPACE_END( LOG )

NAMESPACE_BEGIN( Graphics )
enum class PixelFormat : short;
NAMESPACE_END( Graphics )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )
class IWindow;
NAMESPACE_END( LOGLW )

NAMESPACE_BEGIN( LOGLW )

class Camera;
struct ShaderUnit;
struct VertexData;

using Angle = CUL::MATH::Angle;
using String = CUL::String;
using ColorS = CUL::Graphics::ColorS;
using ColorE = CUL::Graphics::ColorE;
using Pos3Dd = CUL::Graphics::Pos3Dd;
using IFile = CUL::FS::IFile;
using QuadCUL = CUL::MATH::Primitives::Quad;
using TriangleCUL = CUL::MATH::Primitives::Triangle;
using Triangle3D = CUL::MATH::Primitives::Triangle3D;
using IFile = CUL::FS::IFile;

using PointType = CUL::MATH::Point;
using TriangleData = CUL::MATH::Primitives::TriangleData;
using TriangleColors = std::array<ColorS, 3>;

using QuadData = CUL::MATH::Primitives::Quad::QuadData;
using QuadColors = std::array<ColorS, 4>;

using LineData = CUL::MATH::Primitives::Line::LineData;
using LineColors = std::array<ColorS, 2>;

using Point = CUL::MATH::Point;
using BufferDataId = std::uint32_t;

struct GAME_ENGINE_API ContextInfo
{
    void* glContext = nullptr;
    String glVersion;
};

struct GAME_ENGINE_API ShaderParameterInfo
{
    std::int32_t ID{ -1 };
    CUL::String Name;
    DataType Type{ DataType::NONE };
    String TypeName;
    std::int32_t Size{ 0 };
};

struct GAME_ENGINE_API AttributeInfo: public ShaderParameterInfo
{
};

struct GAME_ENGINE_API UniformInfo: public ShaderParameterInfo
{
};

using UniformValue =
    std::variant<float, std::int32_t, std::uint32_t, bool, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4>;

struct GAME_ENGINE_API TextureInfo
{
    bool initialized = false;

    unsigned int textureId = 0;
    int level = 0;
    CUL::Graphics::PixelFormat pixelFormat = CUL::Graphics::PixelFormat::NONE;
    int border = 0;
    DataType dataType = DataType::UNSIGNED_BYTE;
    void* data = nullptr;
    CUL::Graphics::SSize2Di size;

    const String toString() const;
};

struct TextureData2D
{
    float x = 0.f;
    float y = 0.f;

    float s = 0.f;
    float t = 0.f;
};

struct QuadSimple
{
    float top = 0.f;
    float bottom = 0.f;
    float topLeft = 0.f;
    float topRight = 0.f;
};

enum class ETextureUnitIndex : int
{
    UNIT_0 = 0x84C0,
    UNIT_1,
    UNIT_2,
    UNIT_3,
    UNIT_4,
    UNIT_5,
    UNIT_6,
    UNIT_7
};

class Viewport;

enum class EObjectType : uint8_t
{
    BUFFER = 0,
    SHADER,
    PROGRAM,
    VERTEX_ARRAY,
    QUERY,
    PROGRAM_PIPELINE,
    TRANSFORM_FEEDBACK,
    SAMPLER,
    TEXTURE,
    RENDERBUFFER,
    FRAMEBUFFER
};

class GAME_ENGINE_API IRenderDevice
{
public:
    IRenderDevice( bool forceLegacy );

    virtual void* getNativeDevice() = 0;
    virtual bool isLegacy() = 0;
    virtual size_t getFrameBufferCount() const = 0;
    virtual void initDebugUI() = 0;

    virtual void resetMatrixToIdentity( const MatrixTypes matrix );
    virtual void setViewport( const Viewport& viewport );
    virtual void setOrthogonalPerspective( const Camera& vp );
    virtual void setPerspectiveProjection( const Camera& vp );
    virtual void lookAt( const Camera& vp );
    virtual void lookAt( const std::array<Pos3Dd, 3>& lookAtVec );
    virtual void lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up );

    virtual ShaderUnit* createShaderUnit( const CUL::FS::Path& shaderPath, bool assertOnErrors, CUL::String& errorMessage );
    virtual void deleteShaderUnit( ShaderUnit* inShaderUnit );

    // General
    virtual void setObjectName( EObjectType objectType, std::uint32_t objectId, const CUL::String& name );

    virtual void prepareFrame();
    virtual void finishFrame() = 0;

    virtual std::uint32_t createProgram( const CUL::String& name );
    virtual void removeProgram( unsigned programId );
    virtual void useProgram( int programId );
    virtual void linkProgram( unsigned programId );
    virtual void validateProgram( std::uint32_t programId );

    virtual int getCurrentProgram() const;

    virtual bool attachShader( unsigned programId, unsigned shaderId );
    virtual void dettachShader( unsigned programId, unsigned shaderId );
    virtual void removeShader( unsigned shaderId );

    virtual ContextInfo initContextVersion( LOGLW::IWindow* window ) = 0;

    virtual void setAttribValue( int attributeLocation, float value );
    virtual void setAttribValue( int attributeLocation, int value );
    virtual void setAttribValue( int attributeLocation, unsigned value );
    virtual void setAttribValue( int attributeLocation, bool value );
    virtual void setAttribValue( int attributeLocation, const CUL::String& value );

    virtual void setUniformValue( int uniformLocation, float value );
    virtual void setUniformValue( int uniformLocation, int value );
    virtual void setUniformValue( int uniformLocation, unsigned value );

    virtual void setUniformValue( int uniformLocation, const glm::vec2& val ) = 0;
    virtual void setUniformValue( int uniformLocation, const glm::vec3& val ) = 0;
    virtual void setUniformValue( int uniformLocation, const glm::vec4& val ) = 0;

    virtual void setUniformValue( int uniformLocation, const glm::mat2& val ) = 0;
    virtual void setUniformValue( int uniformLocation, const glm::mat3& val ) = 0;
    virtual void setUniformValue( int uniformLocation, const glm::mat4& val ) = 0;

    virtual UniformValue getUniformValue( std::int32_t inProgramId, std::int32_t inUniformId, DataType inDataType );

    virtual void setProjectionAndModelToIdentity() = 0;
    virtual void clearColorAndDepthBuffer() = 0;
    virtual void createQuad( float scale = 1.0f ) = 0;
    virtual void clearColorTo( const ColorS color ) = 0;
    virtual void clearBuffer( const ClearMasks mask ) = 0;

    virtual unsigned int generateVertexArray( const int size = 1 ) = 0;

    virtual void bufferData( BufferDataId bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type );
    virtual void bufferData( BufferDataId bufferId, const CUL::DataWrapper& data, const BufferTypes type );
    virtual void bufferData( BufferDataId bufferId, const std::vector<float>& data, const BufferTypes type );
    virtual void bufferData( BufferDataId bufferId, const float vertices[], BufferTypes type );
    virtual void bufferData( BufferDataId bufferId, const std::vector<TextureData2D>& data, const BufferTypes type );
    virtual void bufferSubdata( BufferDataId bufferId, const BufferTypes type, std::vector<TextureData2D>& data );

    virtual void setClientState( ClientStateTypes cs, bool enabled ) = 0;
    virtual void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) = 0;
    virtual void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) = 0;

    // VAO, VBO
    virtual void setVertexArrayClientState( const bool enable ) = 0;
    virtual void setColorClientState( bool enable ) = 0;
    virtual unsigned int generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size = 1 );
    virtual unsigned int generateAndBindBuffer( const BufferTypes bufferType, const int size = 1 );
    virtual void deleteBuffer( BufferTypes bufferType, unsigned& id ) = 0;

    virtual void enableVertexAttribiute( unsigned programId, const String& attribName ) = 0;
    virtual void disableVertexAttribiute( unsigned programId, const String& attribName ) = 0;
    virtual int getAttribLocation( unsigned programId, const String& attribName ) = 0;
    virtual int getUniformLocation( unsigned programId, const String& attribName ) = 0;
    virtual void unbindBuffer( const BufferTypes bufferType ) = 0;
    virtual void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) = 0;
    // virtual void bindBuffer( VertexArray* vao )  = 0;
    virtual unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) = 0;

    virtual void drawElements( const PrimitiveType type, const CUL::DataWrapper& inData );
    virtual void drawElements( const PrimitiveType type, const std::vector<float>& data ) = 0;
    virtual void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) = 0;
    virtual void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) = 0;
    virtual void vertexAttribPointer( const VertexData& meta ) = 0;
    virtual void enableVertexAttribArray( unsigned attributeId ) = 0;
    virtual void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) = 0;

    virtual std::vector<std::string> listExtensions();

    virtual void draw( const QuadCUL& quad, const QuadCUL& texQuad ) = 0;

    virtual void draw( const QuadCUL& quad, const ColorS& color ) = 0;
    virtual void draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color ) = 0;
    virtual void draw( const QuadCUL& quad, const QuadColors& color ) = 0;

    virtual void draw( const QuadData& quad, const ColorS& color ) = 0;
    virtual void draw( const QuadData& quad, const std::array<ColorS, 4>& color ) = 0;

    virtual void draw( const TriangleCUL& triangle, const ColorS& color ) = 0;
    virtual void draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color ) = 0;
    virtual void draw( const TriangleData& values, const std::array<ColorS, 3>& color ) = 0;

    virtual void draw( const LineData& values, const ColorS& color ) = 0;
    virtual void draw( const LineData& values, const LineColors& color ) = 0;
    virtual void draw( const CUL::MATH::Primitives::Line& values, const ColorS& color ) = 0;

    virtual void draw( const Point& position, const ColorS& color ) = 0;

    virtual void draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color ) = 0;
    virtual void draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color ) = 0;

    virtual void translate( const Point& point ) = 0;
    virtual void translate( const float x, const float y, const float z ) = 0;

    virtual void rotate( const CUL::MATH::Rotation& rotation ) = 0;
    virtual void rotate( const float angleDeg, const float x = 0.0f, const float y = 0.0f, const float z = 0.0f ) = 0;
    virtual void scale( const CUL::MATH::Vector3Df& scale ) = 0;
    virtual void scale( const float scale ) = 0;
    virtual void setDepthTest( const bool enabled ) = 0;
    virtual void setBackfaceCUll( const bool enabled ) = 0;

    // Texturing
    virtual void setTexuring( const bool enabled ) = 0;
    virtual unsigned generateTexture();
    virtual void setActiveTextureUnit( ETextureUnitIndex textureUnitIndex ) = 0;
    virtual void bindTexture( const unsigned int textureId ) = 0;
    virtual void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) = 0;
    virtual void setTextureData( std::uint32_t textureId, const TextureInfo& ti );
    virtual void updateTextureData( const TextureInfo& ti, void* data ) = 0;
    virtual void freeTexture( std::uint32_t textureId );

    virtual void matrixStackPush() = 0;
    virtual void matrixStackPop() = 0;

    CUL::GUTILS::Version getVersion() const;

    virtual unsigned getGPUTotalAvailableMemoryKb();
    virtual unsigned getGPUCurrentAvailableMemoryKb();

    virtual void toggleDebugOutput( bool enable );

    virtual void checkLastCommandForErrors();
    bool getIsEmbeddedSystems() const;

    virtual const String& getName() const = 0;
    virtual LOGLW::RenderTypes::RendererType getType() const = 0;

    virtual std::vector<AttributeInfo> fetchProgramAttributeInfo( std::int32_t inProgramId ) const;
    virtual std::vector<UniformInfo> fetchProgramUniformsInfo( std::int32_t inProgramId ) const;

    virtual ~IRenderDevice();

protected:
    void log( const String& text, const CUL::LOG::Severity severity = CUL::LOG::Severity::INFO ) const;
    void customAssert( const bool value, const CUL::String& message ) const;

    bool m_forceLegacy = false;
    bool m_isEmbeddedSystems = false;

    std::map<BufferTypes, std::int32_t> m_currentBufferId;
    CUL::GUTILS::Version m_supportedVersion;
    String m_versionString;

private:
    CUL::LOG::ILogger* m_logger = nullptr;

    mutable String m_lastLog;
    CUL::LOG::Severity m_lastLogSeverity;
};

NAMESPACE_END( LOGLW )
