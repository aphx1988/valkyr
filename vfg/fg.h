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
		Created = 0,
		NeverCreated,
		Present,
		RenderTarget,
		SRV,
		UAV,
		CBV,
		Destroyed
	};

	enum class ResType { RTV = 0, DSV, SRV_UAV_CBV, Sampler };

	enum class ResActionType { Create, Destroy, Read, Write };

	enum class ResLifeType { Imported, Temp };

	struct Res {
		unsigned id;
		unsigned format;
		unsigned type;
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
		virtual void CreateRT(std::string_view name, unsigned format, unsigned downSampleRatio) = 0;
		virtual void Create(std::string_view name, ResType resType, unsigned format, unsigned width, unsigned height) = 0;
		virtual void CreateCB(std::string_view name, void** constantData) = 0;
		virtual void Read(std::string_view name) = 0;
		virtual void Read(unsigned id) = 0;
		virtual void Read(vptr<Res> res) = 0;
		virtual void Write(std::string_view name) = 0;
		virtual void Write(unsigned id) = 0;
		virtual void Write(vptr<Res> res) = 0;
		//virtual void ReadTemp(std::string_view name) = 0;
		//virtual void ReadTemp(unsigned id) = 0;
		//virtual void WriteTemp(std::string_view name) = 0;
		//virtual void WriteTemp(unsigned id) = 0;
	};

	struct Pass {
		unsigned id;
		std::string_view name;
		/*Vec<unsigned> inputs;
		Vec<unsigned> outputs;*/
		std::function<void(FgBuilder& builder)> setupFunc;
		std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>, Vec<unsigned>)> renderFunc;

		Pass(std::string_view tagName,const std::function<void(FgBuilder& builder)>& setupFun
			,const std::function<void(FgBuilder& builder, CmdList& cmd, Vec<unsigned>,Vec<unsigned>)>& renderFun) {
			setupFunc = setupFun;
			renderFunc = renderFun;
			name = tagName;
		}
	};
	
	struct PassNode {
		unsigned passId;
		unsigned nextPassId;
		vptr<Pass> pPass;
		vptr<Pass> pNextPass;
	};

	// struct PassNode {
	//   unsigned passId;
	//   Vec<unsigned> readTempList;
	//   Vec<unsigned> readList;
	//   Vec<unsigned> writeTempList;
	//   Vec<unsigned> writeList;
	// };

	struct Fg {
		unsigned scnResStartIdx;
		unsigned tempResStartIdx;
		Vec<Res> resList;
		Vec<Pass> passList;
		Map<std::string_view, Res> resMap;
		Map<std::string_view, Pass> passMap;

		Map<unsigned, ResNode> passResMap;
		std::array<PassNode, 8> firstPassLayer{};

		Fg() {}
	};
} // namespace valkyr
