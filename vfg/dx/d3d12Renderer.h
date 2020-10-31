#pragma once
#include "../renderer.h"
#include "../../vcontainer/vec.h"
#include "../../vcontainer/map.h"
#include "../../vtask/Scheduler.h"
#include "d3dcore.h"

using Microsoft::WRL::ComPtr;

namespace valkyr {
	class dxRenderTask :public Task {
		void exec();
	};

	class d3d12Renderer : public Renderer {
	public:
		d3d12Renderer(HWND hwnd):Renderer(),m_hwnd(hwnd),m_frameRT(3),m_frameIdx(0),m_rtvDescriptorSize(0),m_fenceEvent(0),m_fenceValue(0)
		{
			m_currFg = vmake_ptr<Fg>();
			m_scheduler = vmake_ptr<Scheduler<4>>();
		}

		void Init(RenderSetting setting);
		void Update();
		void Render();
		void Destroy();
		void Setup(Fg&& fg);

		vptr<Fg> m_currFg;
		vptr<Scheduler<4>> m_scheduler;
		Vec<ComPtr<ID3D12Resource>> m_resList;
		Vec<ComPtr<ID3D12Resource>> m_tempResList;

		ComPtr<ID3D12CommandAllocator> m_graphicsCmdAllocator;
		ComPtr<ID3D12CommandAllocator> m_computeCmdAllocator;
		ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;
		ComPtr<ID3D12CommandQueue> m_computeCmdQueue;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
		//gpu visible
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
		ComPtr<ID3D12DescriptorHeap> m_srvCbvUavHeap;

		ComPtr<ID3D12PipelineState> m_pso;
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCmdList;
		ComPtr<ID3D12CommandList> m_computeCmdList;
		unsigned m_frameIdx;
		unsigned m_rtvDescriptorSize;


	private:
		void getAdapter(IDXGIFactory1* factory,IDXGIAdapter1** adapter1);
		void waitForPrevFrame();

		ComPtr<ID3D12Device> m_d3dDevice;
		ComPtr<IDXGISwapChain3> m_swapChain;
		Vec<ComPtr<ID3D12Resource>> m_frameRT;

		ComPtr<ID3D12Fence> m_fence;

		HANDLE m_fenceEvent;
		UINT64 m_fenceValue;

		HWND m_hwnd;
	};
}