// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "../../vmalloc/chunk.h"
//#include "../../vcontainer/span.h"
#include "../../vcontainer/tuple.h"

using namespace valkyr;

struct A {
    int i0;
    int i1;

    A():i0(0),i1(0){}
    A(int a, int b) :i0(a), i1(b){}
};

struct B {
    int code;
    char big[CHUNK_SIZE - sizeof(ChunkInfo) - sizeof(int)];
};

struct Rot {
    float angle;
};

struct Position2D {
    float x, y;
    Position2D():x(0),y(0){}
    Position2D(float x, float y) :x(x), y(y) {}
};

struct Position {
    float x,y,z;
    Position():x(0),y(0),z(0){}
    Position(float x, float y, float z) :x(x), y(y), z(z){}
};

struct C {
    float baseline;
    int num;

    C():baseline(0), num(0){}

    C(float b,int n):baseline(b),num(n){}
};

//加x，size为4，只有方法则为1，有两个方法也为1
struct F {
    //int x;
    int f() { return 333; }
    int f3() { return 333; }
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

void myTupleTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
   /* using Tuple_t = Tuple<int, double, char, bool,C>;
    std::cout <<"chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    auto* pTuple = TupleUtil::MakePtr<int, double, char, bool, C>(chunk, 100, 200.99f, 'a', false, {0,10});
    std::cout << "tuple->head=" << pTuple->head << std::endl;
    std::cout << "tuple->tail().tail().head=" << pTuple->tail().tail().head << std::endl;
    std::cout <<"sizeof(*tuple)=" << sizeof(*pTuple)<< std::endl;
    std::cout << "TupleCount<Tuple<int,double,char,bool>>::value=" << TupleCountValue<Tuple_t> << std::endl;
    std::cout << "TupleUtil::Get<2>(*tuple)=" << TupleUtil::Get<2>(*pTuple) << std::endl;
    std::cout << "chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    C* pc = TupleUtil::GetPtr<4>(pTuple);
    std::cout << "pc is  TupleUtil::GetPtr<4>(tuple)"<<std::endl;
    std::cout << "TupleUtil::GetPtr<4>(tuple)->num=" << pc->num << std::endl;
    pc->num = 222;
    std::cout << "after change pc->num to 222,pc->num=" << pc->num << std::endl;
    std::cout << "after change pc->num to 222,Get from tuple=" << TupleUtil::GetPtr<4>(pTuple)->num << std::endl;
    auto tuple2 = TupleUtil::Make(chunk, 10, 20, C());
    std::cout << "TupleUtil::Get<1>(tuple2)=" << TupleUtil::Get<1>(tuple2) << std::endl;
    C* pc2 = TupleUtil::GetPtr<2>(&tuple2);
    pc2->num = 333;
    std::cout << "TupleUtil::GetPtr<2>(tuple2)->num=" << TupleUtil::GetPtr<2>(&tuple2)->num << std::endl;
    std::cout << "before std::tuple created,chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;*/
    std::tuple<int, int, C>* sTuple = nullptr;
    sTuple = vmake_tuple_ptr(chunk,10,20,C());
    std::cout << "sizeof(*sTuple)=" << sizeof(*sTuple) << std::endl;
    std::cout << "after std::tuple created,chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    C* pc3 = &std::get<2>(*sTuple);
    pc3->num = 666;
    std::cout << "std::get<0>(*sTuple)=" << std::get<0>(*sTuple)<< std::endl;
    std::cout <<"(&std::get<2>(*sTuple))->num="<< (&std::get<2>(*sTuple))->num << std::endl;
    auto atuple = vmake_tuple(chunk, 222, 333,C(222,333));
    std::cout << "after std::tuple created,chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    std::cout << "std::get<0>(atuple)=" << vget<0>(atuple) << std::endl;
    std::tuple<int, int,C>* pATuple = &atuple;
    C* pc4 = &std::get<2>(*pATuple);
    pc4->num = 3333;
    std::cout << "vget<2>(pATuple).num = std::get<2>(*pATuple).num=" << vget<2>(pATuple).num << std::endl;
    std::cout << "vget_ptr<C,2>(pATuple)->num=" << vget_ptr<C,2>(pATuple)->num << std::endl;
    ChunkAllocator::Free(chunk);
}

void multiTypeSpanTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
    A* a = ChunkUtil::NewObjFrom<A, int, int>(chunk, 222, 333);
    C* c = ChunkUtil::NewObjFrom<C, float, int>(chunk, 0.5f, 65535);
    ChunkAllocator::Free(chunk);
}

int main()
{
    //chunkTest();
    /*multiTypeSpanTest();*/
    myTupleTest();
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
