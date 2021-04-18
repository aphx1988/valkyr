#include "d3d12Renderer.h"
#include <string_view>

using namespace valkyr;
using Microsoft::WRL::ComPtr;

void valkyr::d3d12Renderer::Init(RenderSetting&& setting) {
    m_renderSetting = std::move(setting);
    m_frameCount = setting.numFrames;
    m_fenceEvents = new HANDLE[m_frameCount];
    m_fenceValues = new UINT64[m_frameCount];
    for (unsigned i = 0; i < m_frameCount; i++) {
        m_d3d12RTResList.emplace_back(nullptr);
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
   /* {
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
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_d3d12RTResList[i])));
            m_d3dDevice->CreateRenderTargetView(m_d3d12RTResList[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }*/
    ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_graphicsCmdAllocator)));
    ThrowIfFailed(m_d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_graphicsCmdAllocator.Get(), nullptr, IID_PPV_ARGS(&m_graphicsCmdList)));
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
    loadAssets();
}

void valkyr::d3d12Renderer::Update()
{
}

void valkyr::d3d12Renderer::Render()
{
    ThrowIfFailed(m_graphicsCmdAllocator->Reset());
    ThrowIfFailed(m_graphicsCmdList->Reset(m_graphicsCmdAllocator.Get(), m_pso.Get()));
    ComPtr<ID3D12Resource> rtRes = m_d3d12RTResList[m_frameIdx];
    CD3DX12_RESOURCE_BARRIER rtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12RTResList[m_frameIdx].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_graphicsCmdList->ResourceBarrier(1, &rtBarrier);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap[DECRIPTOR_HEAP_RTV]->GetCPUDescriptorHandleForHeapStart(), m_frameIdx, m_rtvDescriptorSize);
    // Record commands.
    const float clearColor[] = { 1.0f, 0.2f, 0.4f, 1.0f };
    CD3DX12_RESOURCE_BARRIER prtBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_d3d12RTResList[m_frameIdx].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_graphicsCmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_graphicsCmdList->ResourceBarrier(1, &prtBarrier);
    ThrowIfFailed(m_graphicsCmdList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_graphicsCmdList.Get() };
    m_graphicsCmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    ThrowIfFailed(m_swapChain->Present(1, 0));
    waitForPrevFrame();

}

void valkyr::d3d12Renderer::Setup(Fg& fg)
{
    //create global and scene res,including scn mtl,mvp, RTs are crreated during init
    //create temp res in fg
    //D3D12_DESCRIPTOR_HEAP_DESC srvUavHeapDesc = {};
    ////rtv as srv, mtl srv + mtl cbv, temp srv/cbv/uav, also some for sampler
    //unsigned decriptorCount = 10;
    //srvUavHeapDesc.NumDescriptors = decriptorCount;
    //srvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    //srvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    //ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&srvUavHeapDesc, IID_PPV_ARGS(&m_descriptorHeap[DECRIPTOR_HEAP_CBV_SRV_UAV])));
    initFrameRT();
    fg.root->setupFunc(*this);
}

void valkyr::d3d12Renderer::CreateDeferredRes() 
{
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = m_rtResList.size();
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap[DECRIPTOR_HEAP_RTV])));
        m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap[DECRIPTOR_HEAP_RTV]->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < m_frameCount; i++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_d3d12RTResList[i])));
            m_d3dDevice->CreateRenderTargetView(m_d3d12RTResList[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }
    //create rt based on m_rtResList
    for (auto i = m_frameCount; i < m_rtResList.size(); i++) {

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

void valkyr::d3d12Renderer::loadAssets()
{
    
    Setup();
}

void valkyr::d3d12Renderer::initFrameRT()
{
    for (unsigned i = 0; i < m_frameCount; i++) {
        Res frameRT = {};
        frameRT.width = m_renderSetting.width;
        frameRT.height = m_renderSetting.height;
        frameRT.type = ResType::FrameRT;
        frameRT.format = 0;
        frameRT.typeListIdx = i;
        frameRT.state = ResState::Present;
        frameRT.id = i;
        frameRT.lifeType = ResLifeType::Imported;
        m_rtResList.emplace_back(frameRT);
        m_importedNum++;
    }

}

void valkyr::d3d12Renderer::AddPass(std::string_view name, bool isComputing, std::string_view parentName, const std::function<void(FgBuilder& builder)>& setupFun, const std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)>& renderFun)
{
    Pass pass = Pass(name,isComputing,setupFun,renderFun);
    pass.id = m_passList.size();
    m_passList.emplace_back(pass);
    m_passMap.emplace(name,pass.id);
    PassNode passNode;
    passNode.passId = pass.id;
    passNode.pPass = vmake_ptr<Pass>(pass);
    m_passNodeList.emplace_back(passNode);
    unsigned parentId = m_passMap.at(parentName);
    m_passNodeList[parentId].pNextPass = passNode.pPass;
    m_passNodeList[parentId].nextPassId = pass.id;
}

void valkyr::d3d12Renderer::CreateRT(std::string_view name, std::string_view passName, unsigned format, unsigned downSampleRatio)
{
    //at that time, res always created
    ComPtr<ID3D12Resource> d3dRes;
    unsigned resId = m_resList.size();
    Res res;
    res.id = resId;
    res.format = format;
    res.type = ResType::RTV;
    res.typeListIdx = m_rtResList.size();
    res.width = m_renderSetting.width * downSampleRatio;
    res.height = m_renderSetting.height * downSampleRatio;
    res.state = ResState::RenderTarget;
    res.lifeType = ResLifeType::Temp;
    m_resList.emplace_back(res);
    m_resMap.emplace(name, resId);
    m_rtResList.emplace_back(res.typeListIdx);
    //m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorHeap[DECRIPTOR_HEAP_RTV]->GetCPUDescriptorHandleForHeapStart());
    rtvHandle.Offset(res.typeListIdx,m_rtvDescriptorSize);
    m_d3dDevice->CreateRenderTargetView(d3dRes.Get(), nullptr, rtvHandle);
    m_d3d12RTResList.emplace_back(d3dRes);
}

void valkyr::d3d12Renderer::CreateTex(std::string_view name, std::string_view passName, ResType resType, unsigned format, unsigned width, unsigned height)
{
}

void valkyr::d3d12Renderer::CreateCB(std::string_view name, std::string_view passName, void** constantData)
{
}

void valkyr::d3d12Renderer::ReadRT(std::string_view name) {

}

void valkyr::d3d12Renderer::ReadTex(std::string_view name)
{
}

void valkyr::d3d12Renderer::WriteRT(std::string_view name){
}

void valkyr::d3d12Renderer::WriteUAV(std::string_view name)
{
}



//void valkyr::d3d12Renderer::WriteTemp(std::string_view name){
//}
//void valkyr::d3d12Renderer::WriteTemp(unsigned id){
//}

