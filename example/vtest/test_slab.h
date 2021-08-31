#pragma once

#include <iostream>
#include "../../vcore/vslab.h"
using std::cout;
using std::endl;
namespace valkyr {
	namespace test {

		struct Pos {
			int x, y, z;
			Pos():x(0),y(0),z(0){}
			Pos(int x,int y,int z):x(x),y(y),z(z){}
		};

		void test1() {

		}

		void test2() {

		}

		void basic_test() {
			//first alloc a chunk, then alloc preloaded cache and slab on this chunk, finally create obj
			//create some thread local cache
			std::cout << "basic test for slab" << endl;
			char* buf = new char[CACHE_LINE_SIZE];
			Cache<Pos>* cache = new Cache<Pos>();
			Slab<Pos>* slab = cache->create(buf,3,0,0,0,0);
			SlabItem<Pos>* entt = slab->pop();
			entt->data.x = 10;
			cout << "slab->usage_bits=" << slab->usage_bits << endl;
			for (auto i = 0; i < 3; i++) {
				auto item = (*slab)[i];
				cout << "slabs[" << i << "]->data={x=" << item->data.x << "}" << endl;
			}
		}
	}
	
}

