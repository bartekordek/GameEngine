#pragma once

#include "gameengine/Import.hpp"

#if defined(GAME_ENGINE_WINDOWS)

#include "gameengine/IMPORT_Windows.hpp"

NAMESPACE_BEGIN(LOGLW)

class GAME_ENGINE_API DescriptorHeap
{
public:
	DescriptorHeap();

	HRESULT create(
		ID3D12Device* pDevice,
		D3D12_DESCRIPTOR_HEAP_TYPE Type,
		UINT NumDescriptors,
		bool bShaderVisible = false );

	SIZE_T MakeOffsetted( SIZE_T ptr, UINT index );
	D3D12_CPU_DESCRIPTOR_HANDLE hCPU( UINT index );
	D3D12_GPU_DESCRIPTOR_HANDLE hGPU( UINT index );

protected:
private:
	D3D12_DESCRIPTOR_HEAP_DESC Desc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pDH;
	D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
	D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
	UINT HandleIncrementSize;
};

NAMESPACE_END(LOGLW)

#endif // #if defined(GAME_ENGINE_WINDOWS)