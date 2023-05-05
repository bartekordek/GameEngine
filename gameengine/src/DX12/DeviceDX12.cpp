#include "DX12/DeviceDX12.hpp"

#if defined(GAME_ENGINE_WINDOWS)
#include "gameengine/IGameEngine.hpp"
#include "SDL2Wrapper/IWindow.hpp"
#include "SDL2Wrapper/WinSize.hpp"
#include "LOGLWAdditionalDeps/ImportImgui.hpp"

using namespace LOGLW;

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

DeviceDX12::DeviceDX12(): IRenderDevice( false )
{
}

void* DeviceDX12::getNativeDevice()
{
    return m_device.Get();
}

bool DeviceDX12::isLegacy()
{
	return false;
}

void DeviceDX12::resetMatrixToIdentity( const MatrixTypes matrix )
{
}

void DeviceDX12::setViewport( const Viewport& viewport )
{
}

void DeviceDX12::setOrthogonalPerspective( const Camera& vp )
{
}

void DeviceDX12::setPerspectiveProjection( const Camera& vp )
{
}

void DeviceDX12::lookAt( const Camera& vp )
{
}

void DeviceDX12::lookAt( const std::array<Pos3Dd, 3>& lookAtVec )
{
}

void DeviceDX12::lookAt( const Pos3Dd& eye, const Pos3Dd& center, const Pos3Dd& up )
{
}

unsigned int DeviceDX12::createProgram()
{
	return 0;
}

void DeviceDX12::removeProgram( unsigned programId )
{
}

void DeviceDX12::useProgram( int programId )
{
}

void DeviceDX12::linkProgram( unsigned programId )
{
}

void DeviceDX12::validateProgram( unsigned programId )
{
}

int DeviceDX12::getCurrentProgram() const
{
	return 0;
}

unsigned int DeviceDX12::createShader( const IFile& shaderCode )
{
	return 0;
}

void DeviceDX12::attachShader( unsigned programId, unsigned shaderId )
{
}

void DeviceDX12::dettachShader( unsigned programId, unsigned shaderId )
{
}

void DeviceDX12::removeShader( unsigned shaderId )
{
}

void ThrowIfFailed( HRESULT hr );

ContextInfo DeviceDX12::initContextVersion( SDL2W::IWindow* window )
{
    m_window = window;
    setViewportSize();
    setScissorRect();
    m_aspectRatio = static_cast<float>( m_viewport.Width ) / static_cast<float>( m_viewport.Height );
    initInterfaces();
    createDXDevice();
    createCommandQueue();
    createSwapChain();
    createDescriptorHeaps();
    createRenderTargetViews();
    createRootSignature();
    createShaders();

    m_mainCommandWrapper.create( m_device.Get() );

    createVertexBuffer();

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed( m_device->CreateFence( 0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS( &m_fence ) ) );
        m_fenceValue = 1;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent( nullptr, FALSE, FALSE, nullptr );
        if( m_fenceEvent == nullptr )
        {
            ThrowIfFailed( HRESULT_FROM_WIN32( GetLastError() ) );
        }

        // Wait for the command list to execute; we are reusing the same command
        // list in our main loop but for now, we just want to wait for setup to
        // complete before continuing.

    }

    WaitForPreviousFrame();

    ContextInfo result;


    return result;
}

void DeviceDX12::setViewportSize()
{
    m_viewport.TopLeftX = 0;
    m_viewport.TopLeftY = 0;
    m_viewport.MinDepth = 0;
    m_viewport.MaxDepth = 1;
    m_viewport.Width = m_window->getSize().getWidth();
    m_viewport.Height = m_window->getSize().getHeight();
}

void DeviceDX12::setScissorRect()
{
    m_scissorRect.bottom = m_viewport.Height;
    m_scissorRect.left = 0;
    m_scissorRect.right = m_viewport.Width;
    m_scissorRect.top = 0;
}

void DeviceDX12::initInterfaces()
{
    UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if( SUCCEEDED( D3D12GetDebugInterface( IID_PPV_ARGS( &debugController ) ) ) )
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
    ThrowIfFailed( CreateDXGIFactory2( dxgiFactoryFlags, IID_PPV_ARGS( m_factory.ReleaseAndGetAddressOf() ) ) );
}

void DeviceDX12::createDXDevice()
{
    if( m_useWarpDevice )
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed( m_factory->EnumWarpAdapter( IID_PPV_ARGS( &warpAdapter ) ) );

        ThrowIfFailed( D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS( &m_device )
        ) );
    }
    else
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
        m_dxgiAdapter = hardwareAdapter;
        GetHardwareAdapter( m_factory.Get(), &hardwareAdapter );

        ThrowIfFailed( D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS( &m_device )
        ) );
    }
}

void DeviceDX12::createCommandQueue()
{
	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed( m_device->CreateCommandQueue( &queueDesc, IID_PPV_ARGS( &m_commandQueue ) ) );
}

void DeviceDX12::createSwapChain()
{
	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = m_window->getSize().getWidth();
	swapChainDesc.Height = m_window->getSize().getHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed( m_factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		m_window->getHWND(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	) );

	// This sample does not support fullscreen transitions.
	ThrowIfFailed( m_factory->MakeWindowAssociation( m_window->getHWND(), DXGI_MWA_NO_ALT_ENTER ) );

	ThrowIfFailed( swapChain.As( &m_swapChain ) );
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void DeviceDX12::createDescriptorHeaps()
{
    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed( m_device->CreateDescriptorHeap( &rtvHeapDesc, IID_PPV_ARGS( m_rtvHeap.ReleaseAndGetAddressOf() ) ) );

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed( m_device->CreateDescriptorHeap( &desc, IID_PPV_ARGS( m_srvDescHeap.ReleaseAndGetAddressOf() ) ) );
    }
}

void DeviceDX12::createRenderTargetViews()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( m_rtvHeap->GetCPUDescriptorHandleForHeapStart() );

    // Create a RTV for each frame.
    for( UINT n = 0; n < FrameCount; n++ )
    {
        ThrowIfFailed( m_swapChain->GetBuffer( n, IID_PPV_ARGS( &m_renderTargets[n] ) ) );
        m_device->CreateRenderTargetView( m_renderTargets[n].Get(), nullptr, rtvHandle );
        rtvHandle.Offset( 1, m_rtvDescriptorSize );
    }
}

void DeviceDX12::createRootSignature()
{
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init( 0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT );

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed( D3D12SerializeRootSignature( &rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error ) );
    ThrowIfFailed( m_device->CreateRootSignature( 0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS( &m_rootSignature ) ) );
}

void DeviceDX12::createShaders()
{
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr ) );
    ThrowIfFailed( D3DCompileFromFile( L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr ) );

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof( inputElementDescs ) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE( vertexShader.Get() );
    psoDesc.PS = CD3DX12_SHADER_BYTECODE( pixelShader.Get() );
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC( D3D12_DEFAULT );
    psoDesc.BlendState = CD3DX12_BLEND_DESC( D3D12_DEFAULT );
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    ThrowIfFailed( m_device->CreateGraphicsPipelineState( &psoDesc, IID_PPV_ARGS( m_mainCommandWrapper.PipelineState.ReleaseAndGetAddressOf() ) ) );
}

void DeviceDX12::createVertexBuffer()
{
    // Define the geometry for a triangle.
    Vertex triangleVertices[] =
    {
        { { 0.0f, 0.25f * m_aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.25f, -0.25f * m_aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };

    const UINT vertexBufferSize = sizeof( triangleVertices );

    // Note: using upload heaps to transfer static data like vert buffers is not
    // recommended. Every time the GPU needs it, the upload heap will be marshalled
    // over. Please read up on Default Heap usage. An upload heap is used here for
    // code simplicity and because there are very few verts to actually transfer.
    ThrowIfFailed( m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES( D3D12_HEAP_TYPE_UPLOAD ),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer( vertexBufferSize ),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS( &m_vertexBuffer ) ) );

    // Copy the triangle data to the vertex buffer.
    UINT8* pVertexDataBegin;
    CD3DX12_RANGE readRange( 0, 0 );        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed( m_vertexBuffer->Map( 0, &readRange, reinterpret_cast<void**>( &pVertexDataBegin ) ) );
    memcpy( pVertexDataBegin, triangleVertices, sizeof( triangleVertices ) );
    m_vertexBuffer->Unmap( 0, nullptr );

    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof( Vertex );
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
}

void DeviceDX12::enableDebugLayers()
{
#if defined(_DEBUG)
    // Always enable the debug layer before doing anything DX12 related
    // so all possible errors generated while creating DX12 objects
    // are caught by the debug layer.
    Microsoft::WRL::ComPtr<ID3D12Debug> debugInterface;
    WindowsUtils::ThrowIfFailed( D3D12GetDebugInterface( IID_PPV_ARGS( &debugInterface ) ) );
    debugInterface->EnableDebugLayer();
#endif
}

void DeviceDX12::GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter,
    bool requestHighPerformanceAdapter )
{
    *ppAdapter = nullptr;

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

    Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;
    if( SUCCEEDED( pFactory->QueryInterface( IID_PPV_ARGS( &factory6 ) ) ) )
    {
        for(
            UINT adapterIndex = 0;
            SUCCEEDED( factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS( &adapter ) ) );
            ++adapterIndex )
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1( &desc );

            if( desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE )
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if( SUCCEEDED( D3D12CreateDevice( adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof( ID3D12Device ), nullptr ) ) )
            {
                break;
            }
        }
    }

    if( adapter.Get() == nullptr )
    {
        for( UINT adapterIndex = 0; SUCCEEDED( pFactory->EnumAdapters1( adapterIndex, &adapter ) ); ++adapterIndex )
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1( &desc );

            if( desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE )
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if( SUCCEEDED( D3D12CreateDevice( adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof( ID3D12Device ), nullptr ) ) )
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

void DeviceDX12::WaitForPreviousFrame()
{
    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
    // sample illustrates how to use fences for efficient resource usage and to
    // maximize GPU utilization.

    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed( m_commandQueue->Signal( m_fence.Get(), fence ) );
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if( m_fence->GetCompletedValue() < fence )
    {
        ThrowIfFailed( m_fence->SetEventOnCompletion( fence, m_fenceEvent ) );
        WaitForSingleObject( m_fenceEvent, INFINITE );
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void DeviceDX12::prepareFrame()
{
    m_mainCommandWrapper.reset();

    m_mainCommandWrapper.CommandList->SetGraphicsRootSignature( m_rootSignature.Get() );
    m_mainCommandWrapper.CommandList->RSSetViewports( 1, &m_viewport );
    m_mainCommandWrapper.CommandList->RSSetScissorRects( 1, &m_scissorRect );
}

void DeviceDX12::update()
{
    // Indicate that the back buffer will be used as a render target.
    m_mainCommandWrapper.CommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET ) );

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize );
    m_mainCommandWrapper.CommandList->OMSetRenderTargets( 1, &rtvHandle, FALSE, nullptr );

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_mainCommandWrapper.CommandList->ClearRenderTargetView( rtvHandle, clearColor, 0, nullptr );
    m_mainCommandWrapper.CommandList->IASetPrimitiveTopology( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    m_mainCommandWrapper.CommandList->IASetVertexBuffers( 0, 1, &m_vertexBufferView );
    m_mainCommandWrapper.CommandList->DrawInstanced( 3, 1, 0, 0 );

    // Indicate that the back buffer will now be used to present.
    m_mainCommandWrapper.CommandList->ResourceBarrier( 1, &CD3DX12_RESOURCE_BARRIER::Transition( m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT ) );

    ThrowIfFailed( m_mainCommandWrapper.CommandList->Close() );


    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_mainCommandWrapper.CommandList.Get() };
    m_commandQueue->ExecuteCommandLists( _countof( ppCommandLists ), ppCommandLists );

}

Microsoft::WRL::ComPtr<ID3D12Device2> DeviceDX12::CreateDevice( Microsoft::WRL::ComPtr<IDXGIAdapter4>& adapter )
{
    Microsoft::WRL::ComPtr<ID3D12Device2> d3d12Device2;
    WindowsUtils::ThrowIfFailed( D3D12CreateDevice( adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS( &d3d12Device2 ) ) );
    //    NAME_D3D12_OBJECT(d3d12Device2);

        // Enable debug messages in debug mode.
#if defined(_DEBUG)
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> pInfoQueue;
    if( SUCCEEDED( d3d12Device2.As( &pInfoQueue ) ) )
    {
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE );
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_ERROR, TRUE );
        pInfoQueue->SetBreakOnSeverity( D3D12_MESSAGE_SEVERITY_WARNING, TRUE );

        // Suppress whole categories of messages
        //D3D12_MESSAGE_CATEGORY Categories[] = {};

        // Suppress messages based on their severity level
        D3D12_MESSAGE_SEVERITY Severities[] =
        {
            D3D12_MESSAGE_SEVERITY_INFO
        };

        // Suppress individual messages by their ID
        D3D12_MESSAGE_ID DenyIds[] = {
            D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
        };

        D3D12_INFO_QUEUE_FILTER NewFilter = {};
        //NewFilter.DenyList.NumCategories = _countof(Categories);
        //NewFilter.DenyList.pCategoryList = Categories;
        NewFilter.DenyList.NumSeverities = _countof( Severities );
        NewFilter.DenyList.pSeverityList = Severities;
        NewFilter.DenyList.NumIDs = _countof( DenyIds );
        NewFilter.DenyList.pIDList = DenyIds;

        WindowsUtils::ThrowIfFailed( pInfoQueue->PushStorageFilter( &NewFilter ) );
    }
#endif

    return d3d12Device2;
}

void DeviceDX12::finishFrame()
{
    update();

    // Present the frame.
    ThrowIfFailed( m_swapChain->Present( 1, 0 ) );

    WaitForPreviousFrame();
}

void DeviceDX12::setAttribValue( int attributeLocation, float value )
{
}

void DeviceDX12::setAttribValue( int attributeLocation, int value )
{
}

void DeviceDX12::setAttribValue( int attributeLocation, unsigned value )
{
}

void DeviceDX12::setAttribValue( int attributeLocation, bool value )
{
}

void DeviceDX12::setAttribValue( int attributeLocation, const CUL::String& value )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, float value )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, int value )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, unsigned value )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::vec2& val )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::vec3& val )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::vec4& val )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::mat2& val )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::mat3& val )
{
}

void DeviceDX12::setUniformValue( int uniformLocation, const glm::mat4& val )
{
}

void DeviceDX12::setProjectionAndModelToIdentity()
{
}

void DeviceDX12::clearColorAndDepthBuffer()
{
}

void DeviceDX12::createQuad( float scale )
{
}

void DeviceDX12::clearColorTo( const ColorS color )
{
}

void DeviceDX12::clearBuffer( const ClearMasks mask )
{
}

unsigned int DeviceDX12::generateVertexArray( const int size )
{
	return 0;
}

void DeviceDX12::bufferData( uint8_t bufferId, const CUL::MATH::Primitives::Quad& data, const BufferTypes type )
{
}

void DeviceDX12::bufferData( uint8_t bufferId, const std::vector<unsigned int>& data, const BufferTypes type )
{
}

void DeviceDX12::bufferData( uint8_t bufferId, const std::vector<float>& data, const BufferTypes type )
{
}

void DeviceDX12::bufferData( uint8_t bufferId, const float vertices[], BufferTypes type )
{
}

void DeviceDX12::bufferData( uint8_t bufferId, const std::vector<TextureData2D>& data, const BufferTypes type )
{
}

void DeviceDX12::bufferSubdata( uint8_t bufferId, const BufferTypes type, std::vector<TextureData2D>& data )
{
}

void DeviceDX12::setClientState( ClientStateTypes cs, bool enabled )
{
}

void DeviceDX12::texCoordPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceDX12::vertexPointer( int coordinatesPerElement, DataType dataType, int stride, void* pointer )
{
}

void DeviceDX12::setVertexArrayClientState( const bool enable )
{
}

void DeviceDX12::setColorClientState( bool enable )
{
}

unsigned int DeviceDX12::generateElementArrayBuffer( const std::vector<unsigned int>& data, const int size )
{
	return 0;
}

unsigned int DeviceDX12::generateAndBindBuffer( const BufferTypes bufferType, const int size )
{
	return 0;
}

void DeviceDX12::deleteBuffer( BufferTypes bufferType, unsigned& id )
{
}

void DeviceDX12::enableVertexAttribiute( unsigned programId, const String& attribName )
{
}

void DeviceDX12::disableVertexAttribiute( unsigned programId, const String& attribName )
{
}

int DeviceDX12::getAttribLocation( unsigned programId, const String& attribName )
{
	return 0;
}

int DeviceDX12::getUniformLocation( unsigned programId, const String& attribName )
{
	return 0;
}

void DeviceDX12::unbindBuffer( const BufferTypes bufferType )
{
}

void DeviceDX12::bindBuffer( const BufferTypes bufferType, unsigned bufferId )
{
}

unsigned int DeviceDX12::generateBuffer( const BufferTypes type, const int size )
{
	return 0;
}

void DeviceDX12::drawElements( const PrimitiveType type, const std::vector<unsigned int>& data )
{
}

void DeviceDX12::drawElements( const PrimitiveType type, const std::vector<float>& data )
{
}

void DeviceDX12::drawElementsFromLastBuffer( const PrimitiveType primitiveType, const DataType dataType, unsigned count )
{
}

void DeviceDX12::drawArrays( unsigned vaoId, const PrimitiveType primitiveType, unsigned first, unsigned count )
{
}

void DeviceDX12::vertexAttribPointer( const VertexData& )
{
}

void DeviceDX12::enableVertexAttribArray( unsigned )
{
}

void DeviceDX12::setVertexPointer( int coordinatesPerVertex, DataType dataType, int stride, const void* data )
{
}

std::vector<std::string> DeviceDX12::listExtensions()
{
	return std::vector<std::string>();
}

void DeviceDX12::draw( const QuadCUL& quad, const QuadCUL& texQuad )
{
}

void DeviceDX12::draw( const QuadCUL& quad, const ColorS& color )
{
}

void DeviceDX12::draw( const QuadCUL& quad, const Point& translation, const CUL::MATH::Rotation& rotation, const ColorS& color )
{
}

void DeviceDX12::draw( const QuadCUL& quad, const QuadColors& color )
{
}

void DeviceDX12::draw( const QuadData& quad, const ColorS& color )
{
}

void DeviceDX12::draw( const QuadData& quad, const std::array<ColorS, 4>& color )
{
}

void DeviceDX12::draw( const TriangleCUL& triangle, const ColorS& color )
{
}

void DeviceDX12::draw( const TriangleCUL& quad, const std::array<ColorS, 4>& color )
{
}

void DeviceDX12::draw( const TriangleData& values, const std::array<ColorS, 3>& color )
{
}

void DeviceDX12::draw( const LineData& values, const ColorS& color )
{
}

void DeviceDX12::draw( const LineData& values, const LineColors& color )
{
}

void DeviceDX12::draw( const CUL::MATH::Primitives::Line& values, const ColorS& color )
{
}

void DeviceDX12::draw( const Point& position, const ColorS& color )
{
}

void DeviceDX12::draw( const QuadCUL& quad, const glm::mat4& model, const ColorS& color )
{
}

void DeviceDX12::draw( const TriangleCUL& triangle, const glm::mat4& model, const ColorS& color )
{
}

void DeviceDX12::translate( const Point& point )
{
}

void DeviceDX12::translate( const float x, const float y, const float z )
{
}

void DeviceDX12::rotate( const CUL::MATH::Rotation& rotation )
{
}

void DeviceDX12::rotate( const float angleDeg, const float x, const float y, const float z )
{
}

void DeviceDX12::scale( const CUL::MATH::Vector3Df& scale )
{
}

void DeviceDX12::scale( const float scale )
{
}

void DeviceDX12::setDepthTest( const bool enabled )
{
}

void DeviceDX12::setBackfaceCUll( const bool enabled )
{
}

void DeviceDX12::setTexuring( const bool enabled )
{
}

unsigned DeviceDX12::generateTexture()
{
	return 0;
}

void DeviceDX12::setActiveTextureUnit( ETextureUnitIndex textureUnitIndex )
{
}

void DeviceDX12::bindTexture( const unsigned int textureId )
{
}

void DeviceDX12::setTextureParameter( uint8_t textureId, const TextureParameters type, const TextureFilterType val )
{
}

void DeviceDX12::setTextureData( uint8_t textureId, const TextureInfo& ti )
{
}

void DeviceDX12::freeTexture( unsigned int& textureId )
{
}

void DeviceDX12::matrixStackPush()
{
}

void DeviceDX12::matrixStackPop()
{
}

unsigned DeviceDX12::getGPUTotalAvailableMemoryKb()
{
	return 0;
}

unsigned DeviceDX12::getGPUCurrentAvailableMemoryKb()
{
	return 0;
}

void DeviceDX12::toggleDebugOutput( bool enable )
{
}

void DeviceDX12::checkLastCommandForErrors()
{
}

size_t DeviceDX12::getFrameBufferCount() const
{
    return FrameCount;
}

const String& DeviceDX12::getName() const
{
    return m_name;
}

void DeviceDX12::initDebugUI()
{
    IMGUI_CHECKVERSION();
    m_imguiContext = ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForD3D( m_window->getSDLWindow() );
    ImGui_ImplDX12_Init( m_device.Get(),
                         FrameCount,
                         DXGI_FORMAT_R8G8B8A8_UNORM,
                         m_srvDescHeap.Get(),
                         m_srvDescHeap->GetCPUDescriptorHandleForHeapStart(),
                         m_srvDescHeap->GetGPUDescriptorHandleForHeapStart() );
}

SDL2W::RenderTypes::RendererType DeviceDX12::getType() const
{
    return SDL2W::RenderTypes::RendererType::DIRECTX_12;
}

void ThrowIfFailed( HRESULT hr )
{
    if( FAILED( hr ) )
    {

        char str[64] = {};
        sprintf_s( str, "**ERROR** Fatal Error with HRESULT of %08X\n", static_cast< unsigned int >( hr ) );
        throw;
    }
}

#endif // GAME_ENGINE_WINDOWS