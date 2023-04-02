#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IRenderDevice.hpp"
#include "DX12/CommandWrapper.hpp"

struct ImGuiContext;

NAMESPACE_BEGIN( LOGLW )

class DeviceDX12 final: public IRenderDevice
{
public:
	DeviceDX12();
protected:
private:

	struct Float3
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
	};

	struct Float4
	{
		float x = 0.f;
		float y = 0.f;
		float z = 0.f;
		float w = 0.f;
	};

	struct Vertex
	{
		Float3 position;
		Float4 color;
	};

	void* getNativeDevice() override;

	bool isLegacy() override;

	// Inherited via IRenderDevice
	void resetMatrixToIdentity( const MatrixTypes matrixIn ) override;
	void setProjection( const Camera& rect ) override;
	void setViewport( const Viewport& viewport ) override;
	void setOrthogonalPerspective( const Camera& vp ) override;
	void setPerspectiveProjection( const Camera& vp ) override;
	void lookAt( const Camera& vp ) override;
	void lookAt( const std::array<Pos3Dd, 3>& lookAtVec ) override;
	void lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up ) override;
	unsigned int createProgram() override;
	void removeProgram( unsigned programId ) override;
	void useProgram( int programId ) override;
	void linkProgram( unsigned programId ) override;
	void validateProgram( unsigned programId ) override;
	int getCurrentProgram() const override;
	unsigned int createShader( const IFile& shaderCode ) override;
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
	void bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type ) override;
	void bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type ) override;
	void bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type ) override;
	void bufferData( uint8_t bufferId, const float vertices[], BufferTypes type ) override;
	void bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type ) override;
	void bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data ) override;
	void setClientState( ClientStateTypes cs, bool enabled ) override;
	void texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
	void vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer ) override;
	void setVertexArrayClientState( const bool enable ) override;
	void setColorClientState( bool enable ) override;
	unsigned int generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size = 1 ) override;
	unsigned int generateAndBindBuffer( const BufferTypes bufferType, const int size = 1 ) override;
	void deleteBuffer( BufferTypes bufferType, unsigned& id ) override;
	void enableVertexAttribiute( unsigned programId, const String& attribName ) override;
	void disableVertexAttribiute( unsigned programId, const String& attribName ) override;
	unsigned int getAttribLocation( unsigned programId, const String& attribName ) override;
	unsigned int getUniformLocation( unsigned programId, const String& attribName ) override;
	void unbindBuffer( const BufferTypes bufferType ) override;
	void bindBuffer( const BufferTypes bufferType, unsigned bufferId ) override;
	unsigned int generateBuffer( const BufferTypes type, const int size = 1 ) override;
	void drawElements( const PrimitiveType type, const std::vector<unsigned int>& data ) override;
	void drawElements( const PrimitiveType type, const std::vector<float>& data ) override;
	void drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count ) override;
	void drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count ) override;
	void vertexAttribPointer( const VertexAttributePtrMeta& meta ) override;
	void enableVertexAttribArray( unsigned attributeId ) override;
	void setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data ) override;
	std::vector<std::string> listExtensions() override;
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
	unsigned generateTexture() override;
	void setActiveTextureUnit( ETextureUnitIndex textureUnitIndex ) override;
	void bindTexture( const unsigned int textureId ) override;
	void setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val ) override;
	void setTextureData( uint8_t textureId, const TextureInfo& ti ) override;
	void freeTexture( unsigned int& textureId ) override;
	void matrixStackPush() override;
	void matrixStackPop() override;
	unsigned getGPUTotalAvailableMemoryKb() override;
	unsigned getGPUCurrentAvailableMemoryKb() override;
	void toggleDebugOutput( bool enable ) override;
	void checkLastCommandForErrors() override;

	void enableDebugLayers();


private:
	void initDebugUI() override;
	void GetHardwareAdapter(
		IDXGIFactory1* pFactory,
		IDXGIAdapter1** ppAdapter,
		bool requestHighPerformanceAdapter = false );
	void update();
	bool m_useWarpDevice = false;
	SDL2W::IWindow* m_window = nullptr;
	static const UINT FrameCount = 2;

	Microsoft::WRL::ComPtr<ID3D12Device2> CreateDevice( Microsoft::WRL::ComPtr<IDXGIAdapter4>& adapter );
	void WaitForPreviousFrame();
	void prepareFrame() override;
	void finishFrame() override;
	size_t getFrameBufferCount() const override;


	Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
	Microsoft::WRL::ComPtr<IDXGIAdapter1> m_dxgiAdapter;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_factory;

	CommandWrapper m_mainCommandWrapper;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandListUI;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvDescriptorSize = 0u;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, FrameCount>  m_renderTargets;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvDescHeap;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;


	Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	float m_aspectRatio;

	UINT m_frameIndex = 0u;
	UINT64 m_fenceValue = 0u;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	HANDLE m_fenceEvent;

	CD3DX12_VIEWPORT m_viewport;
	CD3DX12_RECT m_scissorRect;

	ImGuiContext* m_imguiContext = nullptr;



	const String& getName() const override;

    String m_name = "DirectX 12.";
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS