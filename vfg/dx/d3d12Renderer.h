#pragma once
#include "../renderer.h"
#include "../../vtask/Scheduler.h"
#include "d3dcore.h"

using Microsoft::WRL::ComPtr;

namespace valkyr {
	const unsigned DESCRIPTOR_HEAP_NUM = 6u;
	const unsigned DECRIPTOR_HEAP_RTV = 0u;
	const unsigned DECRIPTOR_HEAP_DSV = 1u;
	//gpu visible
	const unsigned DECRIPTOR_HEAP_CBV_SRV_UAV = 2u;
	const unsigned DECRIPTOR_HEAP_SAMPLER = 3u;
	//upload is cpu access, default only gpu
	const unsigned DECRIPTOR_HEAP_CBV_SRV_UAV_UPLOAD = 4u;
	const unsigned DECRIPTOR_HEAP_SAMPLER_UPLOAD = 5u;

	class dxRenderTask :public Task {
		void exec() {
		}
	};

	class d3d12Renderer : public Renderer,public FgBuilder {
	public:
		d3d12Renderer(HWND hwnd):Renderer(),m_hwnd(hwnd),m_currFg(nullptr),m_scheduler(nullptr),
			m_rtvDescriptorSize(0),m_dsvDescriptorSize(0),m_cbvSrvUavDescriptorSize(0),m_samplerDescriptorSize(0)
		{
			m_currFg = vmake_ptr<Fg>();
			m_scheduler = vmake_ptr<Scheduler>(4);
		}

		void Init(RenderSetting&& setting);
		void Update();
		void Render();
		void Destroy();
		void Setup(Fg& fg);
		void CreateDeferredRes();
		virtual void AddPass(std::string_view name, bool isComputing, std::string_view parentName, const std::function<void(FgBuilder& builder)>& setupFun
			, const std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)>& renderFun) override;

		// Í¨¹ý FgBuilder ¼Ì³Ð
		virtual void CreateRT(std::string_view name, std::string_view passName, unsigned format, unsigned downSampleRatio) override;
		virtual void CreateTex(std::string_view name, std::string_view passName, ResType resType, unsigned format, unsigned width, unsigned height) override;
		virtual void CreateCB(std::string_view name, std::string_view passName, void** constantData) override;
		virtual void ReadTex(std::string_view name) override;
		virtual void ReadRT(std::string_view name) override;
		virtual void WriteRT(std::string_view name) override;
		virtual void WriteUAV(std::string_view name) override;

		vptr<Fg> m_currFg;
		vptr<Scheduler> m_scheduler;
		//first mFrameNum are frame buffers
		Vec<ComPtr<ID3D12Resource>> m_d3d12RTResList;
		Vec<ComPtr<ID3D12Resource>> m_d3d12SRVResList;

		ComPtr<ID3D12CommandAllocator> m_graphicsCmdAllocator;
		ComPtr<ID3D12CommandAllocator> m_computeCmdAllocator;
		ComPtr<ID3D12CommandQueue> m_graphicsCmdQueue;
		ComPtr<ID3D12CommandQueue> m_computeCmdQueue;
		//first mFrameNum is frame buffer
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap[DESCRIPTOR_HEAP_NUM];
		//for memory release
		unsigned tempResStart[DESCRIPTOR_HEAP_NUM];
		unsigned tempResSize[DESCRIPTOR_HEAP_NUM];
		unsigned scnResStart[DESCRIPTOR_HEAP_NUM];
		unsigned scnResSize[DESCRIPTOR_HEAP_NUM];

		ComPtr<ID3D12PipelineState> m_pso;
		ComPtr<ID3D12PipelineState> m_computeState;
	/*	ComPtr<ID3D12Resource> m_vertexBuffer;
		ComPtr<ID3D12Resource> m_vertexBufferUpload;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;*/
		ComPtr<ID3D12GraphicsCommandList> m_graphicsCmdList;
		ComPtr<ID3D12CommandList> m_computeCmdList;
		unsigned m_rtvDescriptorSize;
		unsigned m_dsvDescriptorSize;
		unsigned m_cbvSrvUavDescriptorSize;
		unsigned m_samplerDescriptorSize;

	private:
		void getAdapter(IDXGIFactory1* factory,IDXGIAdapter1** adapter1);
		void waitForPrevFrame();
		void loadAssets();
		void initFrameRT();

		ComPtr<ID3D12Device> m_d3dDevice;
		ComPtr<IDXGISwapChain3> m_swapChain;
		//Vec<unsigned> m_frameRTIdList;
		//Vec<ComPtr<ID3D12Resource>> m_frameRT;

		ComPtr<ID3D12Fence> m_fence;

		HANDLE* m_fenceEvents;
		UINT64* m_fenceValues;

		HWND m_hwnd;

	
		
		//Vec<Res> rtList;
		//Vec<Res> shaderResourceList;
	};
}
