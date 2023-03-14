#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "DX12/ImportWindows.hpp"
#include "CUL/STL_IMPORTS/STD_array.hpp"

NAMESPACE_BEGIN( LOGLW )

struct CommandListParams;
struct FramePerapreParams;
struct ResourceWrapper;
struct PrimitiveInfo;

class CommandListWrapper
{
public:
	void init( const CommandListParams& params );
	void prepareFrame( const FramePerapreParams& params );
	void barrier( ResourceWrapper& resource, D3D12_RESOURCE_STATES targetState );
	void OMSetRenderTargets( const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle );
	void ClearRenderTargetView( const CD3DX12_CPU_DESCRIPTOR_HANDLE& cpuDescriptorHandle, const std::array<float,4>& clearColor );

	ID3D12GraphicsCommandList* getRaw() const;

	void drawPrimitive( const PrimitiveInfo& pi );
	void close();
protected:
private:
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12PipelineState> m_pso;
	std::unique_ptr<CommandListParams> m_cmdListParams;
};

NAMESPACE_END( LOGLW )

#endif // GAME_ENGINE_WINDOWS