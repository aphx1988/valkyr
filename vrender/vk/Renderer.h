#pragma once
#include "../RenderGraph.h"

namespace valkyr {
	namespace render {
		class Renderer :RenderGraph
		{
		public:
			Renderer(){}
			~Renderer(){}

			virtual void init(RenderSetting setting);
			virtual void build();
			virtual void exec();

		private:

		};
	}
}