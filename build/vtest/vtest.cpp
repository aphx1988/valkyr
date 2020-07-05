// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../../vmalloc/Pool.h"

struct A {
    int i0;
    int i1;
};

int main()
{
    using namespace valkyr;
    Chunk* chunk = ChunkAllocator::Malloc();
    ChunkInfo* info = ChunkUtil::GetInfo(chunk);
    std::cout << "chunk address:" << chunk << std::endl;
    std::cout<<

    //Pool* pool = new Pool();
    //A* a = pool->Spawn<A>();
    //a->i0 = 1;
    //a->i1 = 10;
    //std::cout << "Pool pool:" << pool << std::endl;
    //std::cout << "A* a:" << a << std::endl;
    ////std::cout << "a+sizeof(A):" << (a+sizeof(A)) << std::endl;
    //ChunkInfo* lastChunkInfo = pool->GetLastChunkInfo();
    //char* pBuff = pool->mDesc->lastChunk->buff;
    //A* aa = (A*)(pBuff + lastChunkInfo->head-sizeof(A));
    //std::cout << "buff+head:" << (A*)(pBuff + lastChunkInfo->head) << std::endl;
    //std::cout << "buff+head-sizeof(A):" << aa << std::endl;
    //std::cout << "buff+head:" << pBuff+lastChunkInfo->head << std::endl;
    //_CrtDumpMemoryLeaks();
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
