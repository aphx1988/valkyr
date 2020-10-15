#pragma once
#include "../renderer.h"

namespace valkyr {
	class d3d12Renderer : public Renderer {
	public:
		void Init();
		void Update();
		void Render();
		void Destroy();

	private:

	};
}