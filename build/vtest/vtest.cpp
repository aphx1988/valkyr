// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "../../vmalloc/MemoryArena.h"
#include "../../vcontainer/span.h"

using namespace valkyr;

struct A {
    int i0;
    int i1;
};

struct B {
    int code;
    char big[CHUNK_SIZE - sizeof(ChunkInfo) - sizeof(int)];
};

struct Rot {
    float angle;
};

void spanTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
    Span<A>* span = SpanUtil::Create<A>(chunk,8);
    SpanUtil::Get(0, span)->i0 = 222;
    SpanUtil::Get(3, span)->i1 = 333;
    SpanUtil::Get(6, span)->i0 = 666666;
    SpanUtil::ForEach<A>(span, [&](A* a, int i) {
        std::cout << "a" << i << ":i0=" << a->i0 << ",i1=" << a->i1 << std::endl;
    });
    ChunkInfo* info = ChunkUtil::GetInfo(span->chunk);
    std::cout << "span->chunk info:" << std::endl;
    std::cout << "  usedSize:"<<info->usedSize << std::endl;
    std::cout << "  head:" << info->head << std::endl;
    A* ac = ChunkUtil::NewObjFrom<A>(chunk);
    ac->i0 = 1;
    ac->i1 = 2;
    Span<Rot>* spanRot = SpanUtil::Create<Rot>(chunk, 2);
    SpanUtil::Get(0, spanRot)->angle = 30.5f;
    SpanUtil::Get(1, spanRot)->angle = 45.0f;
    SpanUtil::ForEach<Rot>(spanRot, [&](Rot* r, int i) {
        std::cout << "rot" << i << ":angle=" << r->angle << std::endl;
     });
    Span<A>* span2 = SpanUtil::Create<A>(chunk, 4);
    SpanUtil::Get(0, span2)->i0 = 3322;
    SpanUtil::Get(2, span2)->i1 = 2233;
    SpanUtil::Connect(span, span2);
    SpanUtil::ForEach<A>(span->next, [&](A* a, int i) {
        std::cout << "a" << i << ":i0=" << a->i0 << ",i1=" << a->i1 << std::endl;
    });
    ChunkAllocator::Free(chunk);
}

//void arenaTest() {
//    Arena* arena = ArenaUtil::Create();
//    A* a = ArenaUtil::NewObjFrom<A>(arena);
//    a->i0 = 333;
//    a->i1 = 222;
//    std::cout << "pool chunk count:" << arena->chunkCount << std::endl;
//    A* a2 = ChunkUtil::GetFrom<A>(sizeof(ChunkInfo) + sizeof(Arena),arena->firstChunk);
//    std::cout << "a2:i0=" << a2->i0<<",i1="<<a2->i1 << std::endl;
//    B* b = ArenaUtil::NewObjFrom<B>(arena);
//    b->code = 200;
//    B* b2 = ChunkUtil::GetFrom<B>(sizeof(ChunkInfo),ChunkUtil::GetInfo(arena->firstChunk)->next);
//    std::cout << "big one b2:code=" << b2->code << std::endl;
//    B* b3 = ArenaUtil::NewObjFrom<B>(arena);
//    b3->code = 404;
//    //为了测试大容量表现，套一次娃，GetInfo有可能返回nullptr因此不安全，但这里预期是不会为空指针
//    B* b4 = ChunkUtil::GetFrom<B>(sizeof(ChunkInfo), ChunkUtil::GetInfo(ChunkUtil::GetInfo(arena->firstChunk)->next)->next);
//    std::cout << "big one b4:code=" << b4->code << std::endl;
//    ArenaUtil::Traverse(arena, [arena](Chunk* chunk,int i) {
//        ChunkInfo* info = ChunkUtil::GetInfo(chunk);
//        std::cout << "chunk" << i << ":" << std::endl;
//        std::cout<<"  used size="<<info->usedSize << std::endl;
//        std::cout << "  head=" << info->head << std::endl;
//        if (arena->firstChunk == chunk)
//            std::cout << "  is first chunk" << std::endl;
//        if (arena->lastChunk == chunk)
//            std::cout << "  is last chunk" << std::endl;
//    });
//    A* aa = ArenaUtil::NewObjFrom<A>(arena);
//    aa->i0 = 666;
//    aa->i1 = 333;
//    std::cout << "aa:i0=" << aa->i0 << ",i1=" << aa->i1 << std::endl;
//    ArenaUtil::TraverseBackword(arena, [arena](Chunk* chunk, int i) {
//        ChunkInfo* info = ChunkUtil::GetInfo(chunk);
//        std::cout << "chunk" << i << ":" << std::endl;
//        std::cout << "  used size=" << info->usedSize << std::endl;
//        std::cout << "  head=" << info->head << std::endl;
//        if (arena->firstChunk == chunk)
//            std::cout << "  is first chunk" << std::endl;
//        if (arena->lastChunk == chunk)
//            std::cout << "  is last chunk" << std::endl;
//    });
//    ArenaUtil::Clear(arena);
//}

int main()
{
    //arenaTest();
    spanTest();
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
