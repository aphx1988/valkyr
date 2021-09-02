#pragma once
//#include "../vcontainer/graph.h"
#include "../vcontainer/map.h"
#include "../vcontainer/vec.h"
#include "../vtask/Scheduler.h"
#include "cmd.h"
#include <array>
#include <string_view>

namespace valkyr {
	enum ResState {
		Present,
		RenderTarget,
		SRV,
		UAV,
		CBV,
		Destroyed = 0;
	};

	enum class ResType { RTV = 0, FrameRT, DSV, SRV, UAV, CBV, Sampler, VB, IB };

	enum class ResActionType { Create, Destroy, Read, Write };

	enum class ResLifeType { Imported, Temp };

	struct Res {
		unsigned id;
		unsigned format;
		ResType type;
		unsigned typeListIdx;
		unsigned width;
		unsigned height;
		ResState state;
		ResLifeType lifeType;
	};

	struct ResNode {
		unsigned id;
		unsigned resId;
		unsigned passId;
		ResActionType action;
	};

	class FgBuilder {
	public:
		virtual void CreateRT(std::string_view name, std::string_view passName, unsigned format, unsigned downSampleRatio) = 0;
		virtual void CreateTex(std::string_view name, std::string_view passName, ResType resType, unsigned format, unsigned width, unsigned height) = 0;
		virtual void CreateCB(std::string_view name, std::string_view passName, void** constantData) = 0;
		virtual void CreateUAV(std::string_view name, std::string_view passName, unsigned width, unsigned height) = 0;
		//create vb and ib
		virtual void ReadTex(std::string_view name) = 0;
		virtual void ReadRT(std::string_view name) = 0;
		virtual void WriteRT(std::string_view name) = 0;
		virtual void WriteUAV(std::string_view name) = 0;
	};

	struct Pass {
		unsigned id;
		std::string_view name;
		std::string_view desc;
		/*Vec<unsigned> inputs;
		Vec<unsigned> outputs;*/
		std::function<void(FgBuilder& builder)> setupFunc;
		std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)> renderFunc;
		alignas(8) bool isComputing;
		size_t pads[2] = { 0,0 };


		Pass() { id = 0; isComputing = false; }
		Pass(std::string_view tagName,bool computing,const std::function<void(FgBuilder& builder)>& setupFun
			,const std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>,Vec<unsigned>)>& renderFun) {
			id = 0;
			setupFunc = setupFun;
			renderFunc = renderFun;
			name = tagName;
			isComputing = computing;
		}
	};
	
	struct PassNode {
		unsigned passId;
		unsigned nextPassId;
		vptr<Pass> pPass;
		vptr<Pass> pNextPass;
	};

	struct Fg {
		vptr<Pass> root;
	};
} // namespace valkyr
