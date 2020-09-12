#pragma once
#include "../vcontainer/vec.h"
#include "Resource.h"

namespace valkyr {
	struct GfxSetting{
		unsigned int width;
		unsigned int height;
		unsigned short msaaCount;
		unsigned short msaaQuality;
	};

	class Rhi {
	public:
		virtual void Init(GfxSetting setting)=0;
		virtual void Update()=0;
		virtual void Reset()=0;
		virtual void Render()=0;
		virtual void Destroy()=0;

		Vec<GfxRes> m_res;
	};
}