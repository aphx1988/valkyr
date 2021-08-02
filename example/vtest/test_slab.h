#pragma once

#include <iostream>
#include "../../vcore/vslab.h"
using std::cout;
using std::endl;
namespace valkyr {
	namespace test {

		void test1() {

		}

		void test2() {

		}

		void basic_test() {
			//first alloc a chunk, then alloc preloaded cache and slab on this chunk, finally create obj
			//create some thread local cache
			std::cout << "basic test for slab" << endl;
		}
	}
	
}

