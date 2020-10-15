#pragma once

namespace valkyr {
	class Renderer {
	public:
		virtual void Init() = 0;
		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual void Destroy() = 0;

		unsigned m_numFrameBuffer;
		unsigned m_width;
		unsigned m_height;
		bool m_useWrap;
		bool m_fullscreen;
	};
}
