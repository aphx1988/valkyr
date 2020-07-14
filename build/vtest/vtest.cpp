// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../../vmalloc/Pool.h"

using namespace valkyr;

struct A {
    int i0;
    int i1;
};

void poolTest() {
    Pool* pool = PoolUtil::CreatePool(4);
    std::cout << "pool first chunk=" << pool->firstChunk << std::endl;
    std::cout << "pool last chunk=" << pool->lastChunk << std::endl;
    std::cout << "num of pool chunks=" << pool->chunkCount << std::endl;
    Chunk* curr = pool->firstChunk;
    for (int i = 0; i < 4; i++) {
        std::cout << "pool's chunk" << i << ":" << curr << std::endl;
        ChunkInfo* info2 = ChunkUtil::GetInfo(curr);
        std::cout << "pool's chunk" << i << "info usedSize:" << info2->usedSize << std::endl;
        curr = curr->next;
    }
    ChunkInfo* info = ChunkUtil::GetInfo(pool->firstChunk);
    std::cout << "first chunk used size=" << info->usedSize << std::endl;
    PoolUtil::Clear(pool);
}

int main()
{
    poolTest();
    system("pause");
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
