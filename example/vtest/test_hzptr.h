#pragma once
#include "../../vcore/vhzptr.h"
#include <iostream>

using std::cout;
using std::endl;

namespace valkyr {
	namespace test {

		struct Pos {
			int x, y, z;
		};

		void test1() {

		}

		void test2() {

		}

		void basic_test() {
			constexpr size_t vhzptr_size = sizeof(vhzptr<Pos>);
			static_assert(vhzptr_size == std::hardware_constructive_interference_size);

		}
	}

}

