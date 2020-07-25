﻿// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "../../vmalloc/chunk.h"
//#include "../../vcontainer/span.h"
#include "../../vcontainer/tuple.h"

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

struct Position {
    float x,y,z;

    Position(float x, float y, float z) :x(x), y(y), z(z){}
};

struct C {
    float baseline;
    int num;

    C(float b,int n):baseline(b),num(n){}
};

void chunkTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
    C* c = ChunkUtil::NewObjFrom<C, float, int>(chunk, 0.5f, 65535);
    std::cout << "c:baseline=" << c->baseline << ",num=" << c->num << std::endl;
    C* c2 = ChunkUtil::NewObjFrom<C, float, int>(chunk, 100.999f, 2222);
    std::cout << "c2:baseline=" << c2->baseline << ",num=" << c2->num << std::endl;
    Position* pos = ChunkUtil::NewObjFrom<Position, float, float, float>(chunk,100.9f,222.22f,333.333f);
    std::cout << "pos:x=" << pos->x << ",y=" << pos->y<<",z="<<pos->z << std::endl;
    ChunkAllocator::Free(chunk);
}

template<typename ...Ts>
void testTuple2(Ts... args) {
    std::cout << "sizeof...(args):" <<sizeof...(args) << std::endl;
    //fold expression
    (std::cout << ...<< args) << std::endl;
    //Variadic Expressions 
    auto tuple = std::make_tuple(1 + args...);
    std::cout <<"sizeof(tuple)"<< sizeof(tuple) << std::endl;
    std::cout << "tuple 0:" << std::get<0>(tuple)<<std::endl;
    //Variadic Indices
    //std::cout << a[args]... << std::endl;
}

template <typename ...T>
void printElements(T... args){}

template <typename H,typename ...T>
void printElements(H h,T... args) {
    int s = sizeof...(args);
    std::cout << s << "->" << h << std::endl;
    printElements(args...);
}

template <typename T>
void printElements(T t) {
    std::cout << 0 << "->" << t << std::endl;
}

struct ChunkObjInfo {
    void* ptr;
    int idx;
    int pad;
    size_t size;
};

template <typename ...T>
void printAll(T... args) {
    int s = sizeof...(args);
    std::cout << "sizeof...args=" << s << std::endl;
    std::initializer_list<void*> argList{
        ([&] {
            std::cout << args << std::endl; 
        }(), &args)...
    };
    std::cout << "argList size=" << argList.size() << std::endl;
    int idx = 0;
    for (auto it = argList.begin(); it != argList.end(); it++) {
        idx++;
        int* pOrigin = (int*)*it;

        std::cout << idx << ":"<<typeid(*it).name() << " " << *pOrigin << std::endl;
    }
}

void tupleTest() {
    //Tuple<int, float, bool> tuple(10, 222.22f, false);
    //Tuple<A, C> tuple({ 22,333 }, {1.99f,99});
    //printElements(20, 40, 'a', true);
    printAll(10,20,'a',true);
    
   /* std::cout<<"sizeof(tuple)="<< sizeof(tuple) << std::endl;
    std::cout << "sizeof(std::tuple)=" << sizeof(std::make_tuple<A,C>({ 22,333 }, { 1.99f,99 })) << std::endl;*/
    //std::cout<<"Tuple<H,R...>::Size=" << Tuple<A,C>::Size << std::endl;
    
}

//void multiTypeSpanTest() {
//    Chunk* chunk = ChunkAllocator::Malloc();
//    C* c = ChunkUtil::NewObjFrom<C, float, int>(chunk, 0.5f, 65535);
//    Span<A,C>* span = new Span<A,C>(chunk);
//    std::cout << "sizeof(A,C)=" << (sizeof(A)+sizeof(C)) << std::endl;
//    std::cout << "span: elementSize=" << Span<A,C>::ElementSize << ",startIdx="<<span->startIdx <<std::endl;
//    ChunkAllocator::Free(chunk);
//}

//void spanTest() {
//    Chunk* chunk = ChunkAllocator::Malloc();
//    Span<A>* span = SpanUtil::Create<A>(chunk,8);
//    SpanUtil::Get(0, span)->i0 = 222;
//    SpanUtil::Get(3, span)->i1 = 333;
//    SpanUtil::Get(6, span)->i0 = 666666;
//    SpanUtil::ForEach<A>(span, [&](A* a, int i) {
//        std::cout << "a" << i << ":i0=" << a->i0 << ",i1=" << a->i1 << std::endl;
//    });
//    ChunkInfo* info = ChunkUtil::GetInfo(span->chunk);
//    std::cout << "span->chunk info:" << std::endl;
//    std::cout << "  usedSize:"<<info->usedSize << std::endl;
//    std::cout << "  head:" << info->head << std::endl;
//    A* ac = ChunkUtil::NewObjFrom<A>(chunk);
//    ac->i0 = 1;
//    ac->i1 = 2;
//    Span<Rot>* spanRot = SpanUtil::Create<Rot>(chunk, 2);
//    SpanUtil::Get(0, spanRot)->angle = 30.5f;
//    SpanUtil::Get(1, spanRot)->angle = 45.0f;
//    SpanUtil::ForEach<Rot>(spanRot, [&](Rot* r, int i) {
//        std::cout << "rot" << i << ":angle=" << r->angle << std::endl;
//     });
//    Span<A>* span2 = SpanUtil::Create<A>(chunk, 4);
//    SpanUtil::Get(0, span2)->i0 = 3322;
//    SpanUtil::Get(2, span2)->i1 = 2233;
//    SpanUtil::Connect(span, span2);
//    SpanUtil::ForEach<A>(span->next, [&](A* a, int i) {
//        std::cout << "a" << i << ":i0=" << a->i0 << ",i1=" << a->i1 << std::endl;
//    });
//    ChunkAllocator::Free(chunk);
//}


int main()
{
    //chunkTest();
    //multiTypeSpanTest();
    tupleTest();
    /*testTuple2<int,int,char,bool>(10,100,'a',true);*/
    /*testTuple2<int, int,int>(100,2,3,4);*/
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
