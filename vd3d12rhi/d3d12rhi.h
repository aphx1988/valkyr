#pragma once

#include "../vrhi/rhi.h"
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>

namespace valkyr{
    class D3d12Rhi:public Rhi{
    public:
        virtual void Init(GfxSetting setting);
        virtual void Update();
        virtual void Reset();
        virtual void Render();
        virtual void Destroy();
    };
}

