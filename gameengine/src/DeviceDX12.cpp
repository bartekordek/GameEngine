#include "DeviceDX12.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "SDL2Wrapper/IWindow.hpp"

using namespace LOGLW;

DeviceDX12::DeviceDX12( CUL::CULInterface* culInterface ): IRenderDevice( culInterface, false )
{
}

bool DeviceDX12::isLegacy()
{
	return false;
}

void DeviceDX12::resetMatrixToIdentity( const MatrixTypes matrix )
{
}

void DeviceDX12::setProjection( const Camera& rect )
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
    ContextInfo result;

    enableDebugLayers();

    m_dxgiAdapter = GetAdapter( false );
    if( m_dxgiAdapter )
    {
    	m_d3d12Device = CreateDevice( m_dxgiAdapter );
    }

    ThrowIfFailed( m_d3d12Device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( m_commandListAllocator.ReleaseAndGetAddressOf() ) ) );
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed( m_d3d12Device->CreateCommandQueue( &commandQueueDesc, IID_PPV_ARGS( m_commandQueue.ReleaseAndGetAddressOf() ) ) );

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferCount = g_bbCount;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = window->getHWND();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;
    swapChainDesc.Flags = 0; //DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    //swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;


    IDXGIFactory2* dxgiFactory = nullptr;
    ThrowIfFailed( CreateDXGIFactory2( 0, __uuidof( IDXGIFactory2 ), ( void** ) &dxgiFactory ) );
    ThrowIfFailed( dxgiFactory->CreateSwapChain( m_commandQueue.Get(), &swapChainDesc, ( IDXGISwapChain** ) m_swapChain.GetAddressOf() ) );

    if( swapChainDesc.Flags & DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT )
    {
        m_swapChain->SetMaximumFrameLatency( g_bbCount );
    }

    dxgiFactory->Release();

    //mRTVDescriptorHeap.Create( mDevice.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, g_bbCount );

    return result;
}

Microsoft::WRL::ComPtr<ID3D12Device2> DeviceDX12::CreateDevice( Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter )
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

Microsoft::WRL::ComPtr<IDXGIAdapter4> DeviceDX12::GetAdapter( bool bUseWarp )
{
    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    UINT createFactoryFlags = 0;
#if defined(_DEBUG)
    createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

    WindowsUtils::ThrowIfFailed( CreateDXGIFactory2( createFactoryFlags, IID_PPV_ARGS( &dxgiFactory ) ) );

    Microsoft::WRL::ComPtr<IDXGIAdapter1> dxgiAdapter1;
    Microsoft::WRL::ComPtr<IDXGIAdapter4> dxgiAdapter4;

    if( bUseWarp )
    {
        WindowsUtils::ThrowIfFailed( dxgiFactory->EnumWarpAdapter( IID_PPV_ARGS( &dxgiAdapter1 ) ) );
        WindowsUtils::ThrowIfFailed( dxgiAdapter1.As( &dxgiAdapter4 ) );
    }
    else
    {
        SIZE_T maxDedicatedVideoMemory = 0;
        for( UINT i = 0; dxgiFactory->EnumAdapters1( i, &dxgiAdapter1 ) != DXGI_ERROR_NOT_FOUND; ++i )
        {
            DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
            dxgiAdapter1->GetDesc1( &dxgiAdapterDesc1 );

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            if( ( dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ) == 0 &&
                SUCCEEDED( D3D12CreateDevice( dxgiAdapter1.Get(),
                                              D3D_FEATURE_LEVEL_11_0, __uuidof( ID3D12Device ), nullptr ) ) &&
                dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory )
            {
                maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
                WindowsUtils::ThrowIfFailed( dxgiAdapter1.As( &dxgiAdapter4 ) );
            }
        }
    }

    return dxgiAdapter4;
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

unsigned int DeviceDX12::getAttribLocation( unsigned programId, const String& attribName )
{
	return 0;
}

unsigned int DeviceDX12::getUniformLocation( unsigned programId, const String& attribName )
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

void DeviceDX12::vertexAttribPointer( const VertexAttributePtrMeta& meta )
{
}

void DeviceDX12::enableVertexAttribArray( unsigned attributeId )
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