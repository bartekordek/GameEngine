#include "DX12/CommandListWrapper.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "DX12/ImportWindows.hpp"
#include "DX12/CommandListParams.hpp"

using namespace LOGLW;

#endif // GAME_ENGINE_WINDOWS

void CommandListWrapper::init( const CommandListParams& params )
{
    ThrowIfFailed( params.Device->CreateGraphicsPipelineState( &params.Description, IID_PPV_ARGS( m_pso.ReleaseAndGetAddressOf() ) ) );



	ThrowIfFailed( params.Device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( m_commandAllocator.ReleaseAndGetAddressOf() ) ) );

    ThrowIfFailed( params.Device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pso.Get(), IID_PPV_ARGS( m_commandList.ReleaseAndGetAddressOf() ) ) );
    ThrowIfFailed( m_commandList->Close() );
}