#pragma once
#include "../vcore/vptr.h"
#include "fg.h"

namespace valkyr {

	struct RenderSetting {
		unsigned width;
		unsigned height;
		unsigned numFrames;
		bool fullscreen;
		bool softwareMode;
	};

	class Renderer {
	public:
		Renderer() {
		}

		virtual void Init(RenderSetting setting) = 0;
		virtual void Update() = 0;
		virtual void Setup(Fg fg) = 0;
		virtual void Render() = 0;
		virtual void Destroy() = 0;
	};
}
