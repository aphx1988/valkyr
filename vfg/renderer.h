#pragma once
#include "../vcore/vptr.h"
#include "../../vcontainer/vec.h"
#include "../../vcontainer/map.h"
#include "fg.h"
#include <string_view>

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
		Renderer() :m_frameIdx(0), m_frameCount(2), m_renderSetting{} {
		}

		virtual void Init(RenderSetting&& setting) = 0;
		virtual void Update() = 0;
		virtual void Setup(Fg& fg) = 0;
		virtual void CreateDeferredRes() = 0;
		virtual void Render() = 0;
		virtual void Destroy() = 0;
		virtual void AddPass(std::string_view name, bool isComputing, std::string_view parentName, const std::function<void(FgBuilder& builder)>& setupFun
			, const std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)>& renderFun) = 0;

		RenderSetting m_renderSetting;
		unsigned m_frameIdx;
		unsigned m_frameCount;
		unsigned m_importedNum;
		Vec<Res> m_resList;
		Vec<unsigned> m_rtResList;
		Vec<unsigned> m_srvResList;
		Vec<Pass> m_passList;
		Vec<ResNode> m_resNodeList;
		Vec<PassNode> m_passNodeList;
		Map<std::string_view, unsigned> m_passMap;
		Map<std::string_view, unsigned> m_resMap;
	};
}
