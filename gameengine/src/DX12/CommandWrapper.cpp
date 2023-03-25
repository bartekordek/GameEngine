#include "DX12/CommandWrapper.hpp"

#if defined(GAME_ENGINE_WINDOWS)

using namespace LOGLW;



void CommandWrapper::create( ID3D12Device2* device )
{
    WindowsUtils::ThrowIfFailed( device->CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( Allocator.ReleaseAndGetAddressOf() ) ) );

    WindowsUtils::ThrowIfFailed(
        device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            Allocator.Get(),
            PipelineState.Get(),
            IID_PPV_ARGS( CommandList.ReleaseAndGetAddressOf() ) ) );
    WindowsUtils::ThrowIfFailed( CommandList->Close() );
}

void CommandWrapper::reset()
{
    WindowsUtils::ThrowIfFailed( Allocator->Reset() );

    WindowsUtils::ThrowIfFailed(
        CommandList->Reset(
            Allocator.Get(),
            PipelineState.Get() ) );
}

#endif // #if defined(GAME_ENGINE_WINDOWS)