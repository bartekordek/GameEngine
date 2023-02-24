#include "gameengine/DX12/DescriptorHeap.hpp"


#if defined(GAME_ENGINE_WINDOWS)
#include "CUL/GenericUtils/SimpleAssert.hpp"

LOGLW::DescriptorHeap::DescriptorHeap()
{

}

HRESULT LOGLW::DescriptorHeap::create(
	ID3D12Device* pDevice,
	D3D12_DESCRIPTOR_HEAP_TYPE Type,
	UINT NumDescriptors,
	bool bShaderVisible = false)
{
	Desc.Type = Type;
	Desc.NumDescriptors = NumDescriptors;
	Desc.Flags = ( bShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : ( D3D12_DESCRIPTOR_HEAP_FLAGS ) 0 );

	HRESULT hr = pDevice->CreateDescriptorHeap( &Desc,
												__uuidof( ID3D12DescriptorHeap ),
												( void** ) &pDH );
	if( FAILED( hr ) ) return hr;

	hCPUHeapStart = pDH->GetCPUDescriptorHandleForHeapStart();
	if( bShaderVisible )
	{
		hGPUHeapStart = pDH->GetGPUDescriptorHandleForHeapStart();
	}
	else
	{
		hGPUHeapStart.ptr = 0;
	}
	HandleIncrementSize = pDevice->GetDescriptorHandleIncrementSize( Desc.Type );
	return hr;
}

SIZE_T LOGLW::DescriptorHeap::MakeOffsetted( SIZE_T ptr, UINT index )
{
	SIZE_T offsetted;
	offsetted = ptr + index * HandleIncrementSize;
	return offsetted;
}

D3D12_CPU_DESCRIPTOR_HANDLE LOGLW::DescriptorHeap::hCPU( UINT index )
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = MakeOffsetted( hCPUHeapStart.ptr, index );
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE LOGLW::DescriptorHeap::hGPU( UINT index )
{
	CUL::Assert::simple( Desc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, "ERROR!" );
	D3D12_GPU_DESCRIPTOR_HANDLE handle;
	handle.ptr = MakeOffsetted( hGPUHeapStart.ptr, index );
	return handle;
}

#endif // #if defined(GAME_ENGINE_WINDOWS)