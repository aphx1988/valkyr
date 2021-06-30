#pragma once
#ifdef _MSC_VER
#include <xmemory>
#endif
#ifdef _linux_
#include <sys/types.h>
#endif
#include <iostream>
#include <cassert>
using std::cout;
using std::endl;
namespace valkyr {
	namespace test {
		const size_t CHUNK_SIZE = 1 << 22;
		const size_t PAGE_SIZE = 1 << 12;
		const size_t PAGE_CAPACITY = CHUNK_SIZE / PAGE_SIZE;

		struct Color {
			int r, g, b;

			Color(int red, int green, int blue) {
				r = red;
				g = green;
				b = blue;
			}
		};

		std::ostream& operator<<(std::ostream& os, const Color& c)
		{
			os << "color.r=" << c.r << ",g=" << c.g << ",b=" << c.b;
			return os;
		}

		void basic_test() {
			assert(CHUNK_SIZE % PAGE_SIZE == 0);
			assert(PAGE_CAPACITY >= 1 && PAGE_CAPACITY <= CHUNK_SIZE);
			uintptr_t sz = (PAGE_CAPACITY - 1) * PAGE_SIZE;
			size_t align = 16;
			uintptr_t align_sz = (sz + (align - 1)) & ~(align - 1);
			void* chunk = malloc(align_sz);
			//<<针对char*做了重载，而void*没法加减，不得不转换回void*
			std::cout << "chunk address=" << (void*)chunk << endl;
			Color* p_color = new(chunk) Color(255, 223, 128);
			std::cout << "color 1: " << *p_color << endl;
			std::cout<<"color 1 address = " << p_color << std::endl;
		/*	Color* p_color2 = new((char*)chunk+sizeof(Color)) Color(154, 0, 32);
			cout << "color 2: " << *p_color2 << endl;
			std::cout << "color 2 address = " << p_color2 << std::endl;*/
			void* p_start = (void*)((uintptr_t)p_color & ~CHUNK_SIZE);
			std::cout << "computed chunk start address=" << p_start << endl;
			//有时候会不一致,可能内存未对齐
			assert(chunk == p_start);
			std::cout << "then check color 1:" << *((Color*)p_start) << endl;
		/*	Color* p_col2 = (Color*)((char*)p_start + sizeof(Color));
			std::cout << "then check color 2:" << *((Color*)p_col2) << endl;*/
		}
	}
}
