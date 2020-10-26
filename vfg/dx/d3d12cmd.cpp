#include "d3d12cmd.h"

void valkyr::d3d12CmdList::ClearRTV(unsigned rtvHandle, float* color)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDsvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIdx, m_rtvDescriptorSize);
	m_graphicsCmdList->ClearRenderTargetView(rtvHandle,color,0, nullptr);
}
