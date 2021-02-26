#pragma once
#include "../renderer.h"
#include "../../vcontainer/vec.h"
#include "../../vcontainer/map.h"
#include "../../vtask/Scheduler.h"
#include "d3dcore.h"
#include <string_view>

using Microsoft::WRL::ComPtr;

namespace valkyr {
	const unsigned HEAP_NUM = 4u;
	const unsigned HEAP_RTV = 0u;
	const unsigned HEAP_DSV = 1u;
	//gpu visible
	const unsigned HEAP_CBV_SRV_UAV = 2u;
	const unsigned HEAP_SAMPLER = 3u;

	class dxRenderTask :public Task {
		void exec() {
		}
	};

	class d3d12Renderer : public Renderer,public FgBuilder {
	public:
		d3d12Renderer(HWND hwnd):Renderer(),m_hwnd(hwnd),m_frameCount(0),m_frameIdx(0),m_rtvDescriptorSize(0)
		{
			m_currFg = vmake_ptr<Fg>();
			m_scheduler = vmake_ptr<Scheduler>(4);
		}

		void Init(RenderSetting setting);
		void Update();
		void Render();
		void Destroy();
		void Setup(Fg&& fg);

		vptr<Fg> m_currFg;
		vptr<Scheduler> m_scheduler;
		Vec<ComPtr<ID3D12Resource>> m_resList;
		Map<std::string_view, unsigned> m_resMap;

		Vec<unsigned> m_tempResIdList;
		

		ComPtr<ID3D12CommandAllocator> m_graphicsCmdAllocator;
		ComPtr<ID3D12CommandAllocator> m_computeCmdAllocator;
		ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;
		ComPtr<ID3D12CommandQueue> m_computeCmdQueue;
		ComPtr<ID3D12DescriptorHeap> m_heap[HEAP_NUM];
		//for memory release
		unsigned tempResStart[HEAP_NUM];
		unsigned tempResSize[HEAP_NUM];
		unsigned scnResStart[HEAP_NUM];
		unsigned scnResSize[HEAP_NUM];

		ComPtr<ID3D12PipelineState> m_pso;
		ComPtr<ID3D12PipelineState> m_computeState;
	/*	ComPtr<ID3D12Resource> m_vertexBuffer;
		ComPtr<ID3D12Resource> m_vertexBufferUpload;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;*/
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCmdList;
		ComPtr<ID3D12CommandList> m_computeCmdList;
		unsigned m_frameIdx;
		unsigned m_frameCount;
		unsigned m_rtvDescriptorSize;
		unsigned m_dsvDescriptorSize;
		unsigned m_cbvSrvUavDescriptorSize;
		unsigned m_samplerDescriptorSize;

	private:
		void getAdapter(IDXGIFactory1* factory,IDXGIAdapter1** adapter1);
		void waitForPrevFrame();

		ComPtr<ID3D12Device> m_d3dDevice;
		ComPtr<IDXGISwapChain3> m_swapChain;
		Vec<unsigned> m_frameRTIdList;
		//Vec<ComPtr<ID3D12Resource>> m_frameRT;

		ComPtr<ID3D12Fence> m_fence;

		HANDLE* m_fenceEvents;
		UINT64* m_fenceValues;

		HWND m_hwnd;

		// Í¨¹ý FgBuilder ¼Ì³Ð
		virtual void CreateRT(std::string_view name, unsigned format, unsigned downSampleRatio) override;
		virtual void Read(std::string_view name) override;
    virtual void Read(unsigned id) override;
    virtual void Write(std::string_view name) override;
    virtual void Write(unsigned id) override;
    virtual void ReadTemp(std::string_view name) override;
    virtual void ReadTemp(unsigned id) override;
    virtual void WriteTemp(std::string_view name) override;
    virtual void WriteTemp(unsigned id) override;

	};
}
