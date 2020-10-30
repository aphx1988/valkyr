#include "d3d12cmd.h"

void valkyr::d3d12CmdList::ClearRTV(float* color)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_renderer->m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_renderer->m_frameIdx, m_renderer->m_rtvDescriptorSize);
	m_renderer->m_graphicsCmdList->ClearRenderTargetView(rtvHandle, color,0, nullptr);
}
