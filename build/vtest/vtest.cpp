// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../../vmalloc/Pool.h"

struct A {
    int i0;
    int i1;
};

void poolTest() {
    using namespace valkyr;
    Pool* pool = PoolUtil::CreatePool(4);
    std::cout << "pool last chunk=" << pool->lastChunk << std::endl;
    std::cout << "num of pool chunks=" << pool->chunkCount << std::endl;
    std::cout << "first chunk=" << pool->firstChunk << std::endl;
    ChunkInfo* info = ChunkUtil::GetInfo(pool->firstChunk);
    std::cout << "first chunk used size=" << info->usedSize << std::endl;
}

int main()
{
    using namespace valkyr;
    Chunk* chunk = ChunkAllocator::Malloc();
    ChunkInfo* info = ChunkUtil::GetInfo(chunk);
    int aHead = info->head;
    A* a = ChunkUtil::NewObjFrom<A>(chunk);
    a->i0 = 10;
    a->i1 = 100;
    std::cout << "chunk address=" << chunk << std::endl;
    std::cout << "info address=" << info << std::endl;
    std::cout << "a address=" << a << std::endl;
    std::cout << "a.i1=" << a->i1 << std::endl;
    std::cout << "info.usedSize=" << info->usedSize << std::endl;
    std::cout << "info.head=" << info->head << std::endl;
    std::cout << "===============================" << std::endl;
    A* a2 = ChunkUtil::NewObjFrom<A>(chunk);
    a2->i0 = 0;
    a2->i1 = 222;
    std::cout << "a2 address=" << a2 << std::endl;
    std::cout << "a2.i1=" << a2->i1 << std::endl;
    std::cout << "info.usedSize=" << info->usedSize << std::endl;
    std::cout << "info.head=" << info->head << std::endl;
    A* aa = ChunkUtil::GetFrom<A>(aHead, chunk);
    std::cout << "get a address=" << aa << std::endl;
    std::cout << "get a.i1=" << aa->i1 << std::endl;
    std::cout << "===============================" << std::endl;
    Chunk* chunk2 = ChunkAllocator::Malloc();
    chunk->next = chunk2;
    int chunk2InitHead = ChunkUtil::GetInfo(chunk2)->head;
    A* a3 = ChunkUtil::NewObjFrom<A>(chunk->next);
    a3->i1 = 33;
    std::cout << "chunk2InitHead=" << chunk2InitHead << std::endl;
    std::cout << "a3.i1=" << a3->i1 << std::endl;
    ChunkInfo* chunk2Info = ChunkUtil::GetInfo(chunk2);
    std::cout<<"chunk2 info.usedSize="<< chunk2Info->usedSize << std::endl;
    std::cout << "chunk->next info.usedSize=" << ChunkUtil::GetInfo(chunk->next)->usedSize << std::endl;
    std::cout << "chunk2 info.head=" << ChunkUtil::GetInfo(chunk2)->head << std::endl;
    std::cout << "chunk->next info.head=" << ChunkUtil::GetInfo(chunk->next)->head << std::endl;
    std::cout << "===============================" << std::endl;
    std::cout << "sizeof(typeid(A))=" << sizeof(typeid(A))<<",typeid(A).name=" <<typeid(A).name()<< std::endl;
    std::cout << "===============================" << std::endl;
    poolTest();
    ChunkAllocator::Free(chunk);
    ChunkAllocator::Free(chunk2);
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
