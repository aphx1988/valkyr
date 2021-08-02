#pragma once
#include <cstdint>
#include <optional>

uint16_t reverse_mask(uint16_t x, uint16_t mask){
	return x & (mask);
}

// blog.csdn.net/wishes2018/article/details/85011982
std::optional<uint16_t> tail_one(uint16_t x)
{
	if (x == 0) {
		return std::nullopt;
	}

	int num = 1;
	/*if ((x & 0xffff) == 0) {
		num += 16;
		x >>= 16;
	}*/
	if ((x & 0xff) == 0) {
		num += 8;
		x >>= 8;
	}
	if ((x & 0xf) == 0) {
		num += 4;
		x >>= 4;
	}
	if ((x & 0x3) == 0) {
		num += 2;
		x >>= 2;
	}
	if ((x & 0x1) == 0) {
		num += 1;
	}
	return num-1;
}


