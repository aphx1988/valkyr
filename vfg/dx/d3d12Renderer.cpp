#include "d3d12Renderer.h"
#include <string_view>

using namespace valkyr;
using Microsoft::WRL::ComPtr;

void valkyr::d3d12Renderer::Init(RenderSetting setting) {
    m_frameCount = setting.numFrames;
    m_fenceEvents = new HANDLE[m_frameCount];
    m_fenceValues = new UINT64[m_frameCount];
    for (unsigned i = 0; i < m_frameCount; i++) {
        m_resList.push_back(nullptr);
    }
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
    {
        ComPtr<ID3D12Debug> d3d12Debug;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&d3d12Debug))))
        {
            d3d12Debug->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif
    ComPtr<IDXGIFactory4> dxgiFactory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));
    if (setting.softwareMode) {
        ComPtr<IDXGIAdapter> adapter;
        ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
        ThrowIfFailed(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_d3dDevice)
        ));
    }
    else {
        ComPtr<IDXGIAdapter1> adapter;
        getAdapter(dxgiFactory.Get(), &adapter);
        ThrowIfFailed(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_d3dDevice)
        ));
    }
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_graphicsCmdQueue)));

    D3D12_COMMAND_QUEUE_DESC computeQueueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
    ThrowIfFailed(m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeCmdQueue)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = setting.numFrames;
    swapChainDesc.Width = setting.width;
    swapChainDesc.Height = setting.height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(dxgiFactory->CreateSwapChainForHwnd(
        m_graphicsCmdQueue.Get(),
        m_hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
    ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIdx = m_swapChain->GetCurrentBackBufferIndex();
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = setting.numFrames;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap[DECRIPTOR_HEAP_RTV])));

        m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap[DECRIPTOR_HEAP_RTV]->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < m_frameCount; i++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_resList[i])));
            m_d3dDevice->CreateRenderTargetView(m_resList[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }
    ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCmdAllocator)));
    ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_graphicsCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCmdList)));
    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_graphicsCmdList->Close());
    {
        ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValues[0] = 1;
        m_fenceEvents[0] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvents[0] == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
    }
}

void valkyr::d3d12Renderer::Update()
{
}

void valkyr::d3d12Renderer::Render()
{
    ThrowIfFailed(m_graphicsCmdAllocator->Reset());
    ThrowIfFailed(m_graphicsCmdList->Reset(m_graphicsCmdAllocator.Get(), m_pso.Get()));
    ComPtr<ID3D12Resource> rtRes = m_resList[m_frameIdx];
    CD3DX12_RESOURCE_BARRIER rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resList[m_frameIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_graphicsCmdList->ResourceBarrier(1, &rtBarrier);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap[DECRIPTOR_HEAP_RTV]->GetCPUDescriptorHandleForHeapStart(), m_frameIdx, m_rtvDescriptorSize);
    // Record commands.
    const float clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
    CD3DX12_RESOURCE_BARRIER prtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resList[m_frameIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_graphicsCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_graphicsCmdList->ResourceBarrier(1, &prtBarrier);
    ThrowIfFailed(m_graphicsCmdList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_graphicsCmdList.Get() };
    m_graphicsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    ThrowIfFailed(m_swapChain->Present(1, 0));
    waitForPrevFrame();

}

void valkyr::d3d12Renderer::Setup(Fg&& fg)
{
    //create global and scene res,including scn mtl,mvp, RTs are crreated during init
    //create temp res in fg
    Fg frameGraph = std::forward<Fg>(fg);
    for (int i = 0; i < frameGraph.firstPassLayer.size(); i++) {
        vptr<Pass> pass = frameGraph.firstPassLayer[i].pPass;
        pass->setupFunc(*this);

    }
}

void valkyr::d3d12Renderer::Destroy()
{
    waitForPrevFrame();
    CloseHandle(m_fenceEvents[0]);
}

void valkyr::d3d12Renderer::getAdapter(IDXGIFactory1* factory1, IDXGIAdapter1** adapter1)
{
    *adapter1 = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(factory1->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(&adapter));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    else
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory1->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }
    *adapter1 = adapter.Detach();
}

void valkyr::d3d12Renderer::waitForPrevFrame()
{
    const UINT64 fence = m_fenceValues[0];
    ThrowIfFailed(m_graphicsCmdQueue->Signal(m_fence.Get(), fence));
    m_fenceValues[0]++;
    if (m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvents[0]));
        WaitForSingleObject(m_fenceEvents[0], INFINITE);
    }
    m_frameIdx = m_swapChain->GetCurrentBackBufferIndex();
}

void valkyr::d3d12Renderer::CreateRT(std::string_view name, unsigned format, unsigned downSampleRatio)
{
}

void valkyr::d3d12Renderer::Create(std::string_view name, ResType resType, unsigned format, unsigned width, unsigned height)
{
}

void valkyr::d3d12Renderer::CreateCB(std::string_view name, void** constantData)
{
}

void valkyr::d3d12Renderer::Read(std::string_view name)
{
}

void valkyr::d3d12Renderer::Read(unsigned id)
{
}

//void valkyr::d3d12Renderer::ReadTemp(std::string_view name){
//  
//}

//void valkyr::d3d12Renderer::ReadTemp(unsigned id){
//}

void valkyr::d3d12Renderer::Write(std::string_view name){
}

void valkyr::d3d12Renderer::Write(unsigned id){
}

void valkyr::d3d12Renderer::Read(vptr<Res> res)
{
}

void valkyr::d3d12Renderer::Write(vptr<Res> res)
{
}


//void valkyr::d3d12Renderer::WriteTemp(std::string_view name){
//}
//void valkyr::d3d12Renderer::WriteTemp(unsigned id){
//}

