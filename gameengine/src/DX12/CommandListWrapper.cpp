#include "DX12/CommandListWrapper.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "DX12/ImportWindows.hpp"
#include "DX12/CommandListParams.hpp"
#include "DX12/FramePerapreParams.hpp"
#include "DX12/ResourceWrapper.hpp"
#include "DX12/PrimitiveInfo.hpp"

using namespace LOGLW;

#endif // GAME_ENGINE_WINDOWS

void CommandListWrapper::init( const CommandListParams& params )
{
    m_cmdListParams = std::make_unique<CommandListParams>();
    *m_cmdListParams = params;

    auto descriptionCopy = params.Description;
    ThrowIfFailed( params.Device->CreateGraphicsPipelineState( &m_cmdListParams->Description, IID_PPV_ARGS( m_pso.ReleaseAndGetAddressOf() ) ) );

	ThrowIfFailed( params.Device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( m_commandAllocator.ReleaseAndGetAddressOf() ) ) );

    ThrowIfFailed( params.Device->CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), m_pso.Get(), IID_PPV_ARGS( m_commandList.ReleaseAndGetAddressOf() ) ) );
    ThrowIfFailed( m_commandList->Close() );
}

void CommandListWrapper::prepareFrame( const FramePerapreParams& params )
{
    ThrowIfFailed( m_commandAllocator->Reset() );
    ThrowIfFailed( m_commandList->Reset( m_commandAllocator.Get(), params.PipelineState ) );

    m_commandList->SetGraphicsRootSignature( params.RootSignature );
    m_commandList->RSSetViewports( 1, params.Viewport );
    m_commandList->RSSetScissorRects( 1, params.ScissorRect );
}

void CommandListWrapper::barrier( ResourceWrapper& resource, D3D12_RESOURCE_STATES targetState )
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition( resource.Resource.Get(), resource.State, targetState );
    m_commandList->ResourceBarrier( 1, &barrier );
}

void CommandListWrapper::OMSetRenderTargets( const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle )
{
    m_commandList->OMSetRenderTargets( 1, &cpuDescriptorHandle, FALSE, nullptr );
}

void CommandListWrapper::ClearRenderTargetView( const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, const std::array<float, 4>& clearColor )
{
    m_commandList->ClearRenderTargetView( cpuDescriptorHandle, clearColor.data(), 0, nullptr );
}

ID3D12GraphicsCommandList* CommandListWrapper::getRaw() const
{
    return m_commandList.Get();
}

void CommandListWrapper::drawPrimitive( const PrimitiveInfo& pi )
{
    m_commandList->IASetPrimitiveTopology( pi.Topology );
    m_commandList->IASetVertexBuffers( pi.StartSlot, pi.NumViews, pi.VBV );
    m_commandList->DrawInstanced(
        pi.VertexCountPerInstance,
        pi.InstanceCount,
        pi.StartVertexLocation,
        pi.StartInstanceLocation );
}

void CommandListWrapper::close()
{
    ThrowIfFailed( m_commandList->Close() );
}