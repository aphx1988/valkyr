#pragma once
#include "RenderTypes.h"

namespace valkyr {
	namespace render {
		class ResBuilder
		{
		public:
			ResBuilder() {}
			~ResBuilder() {}

			virtual void CreateRT(unsigned id,Tex tex) = 0;
			virtual void CreateTex(unsigned id, Tex tex) = 0;
			virtual void CreateCB(unsigned id, Tex tex) = 0;
			virtual void CreateUAV(unsigned id, Tex tex) = 0;
			//create vb and ib
			virtual void ReadTex(unsigned id) = 0;
			virtual void ReadRT(unsigned id) = 0;
			virtual void WriteRT(unsigned id) = 0;
			virtual void WriteUAV(unsigned id) = 0;

		private:

		};
	}
}