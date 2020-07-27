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

template <typename ...T>
void CreateTuple(Chunk* chunk,T... prototypes) {
    int arr[] = {
        ([&] {
            ChunkUtil::NewObjFrom<T>(chunk);
        }(),0)...
    };
}


//template<typename... Ts>
//std::ostream& operator<<(std::ostream& os, Tuple<Ts...> const& theTuple)
//{
//    vapply
//    (
//        [&os](Ts const&... tupleArgs)
//        {
//            os << '[';
//            std::size_t n{ 0 };
//            ((os << tupleArgs << (++n != sizeof...(Ts) ? ", " : "")), ...);
//            os << ']';
//        }, theTuple
//    );
//    return os;
//}

void myTupleTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
    using Tuple_t = Tuple<int, double, char, bool>;
    std::cout <<"chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    Tuple_t* tuple = TupleUtil::Make<int,double,char,bool>(chunk,100,200.99f,'a',false);
    std::cout << "tuple->head=" << tuple->head << std::endl;
    std::cout << "tuple->tail().tail().head=" << tuple->tail().tail().head << std::endl;
    std::cout <<"sizeof(*tuple)=" << sizeof(*tuple)<< std::endl;
    std::cout << "TupleCount<Tuple<int,double,char,bool>>::value=" << TupleCountValue<Tuple_t> << std::endl;
    //std::cout << sizeof(F) << std::endl;
    std::cout << "TupleUtil::Get<2>(*tuple)=" << TupleUtil::Get<2>(*tuple) << std::endl;
    auto add_lambda = [](auto first, auto second) { return first + second; };
    //std::cout << *tuple << '\n';
    std::cout << "chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
    ChunkAllocator::Free(chunk);
}

void multiTypeSpanTest() {
    Chunk* chunk = ChunkAllocator::Malloc();
    A* a = ChunkUtil::NewObjFrom<A, int, int>(chunk, 222, 333);
    C* c = ChunkUtil::NewObjFrom<C, float, int>(chunk, 0.5f, 65535);
    /*std::cout << "can create span<A,C>=" << SpanUtil::CanCreate<A, C>(chunk) << std::endl;*/
    /*Span<A,C>* span = SpanUtil::Create(chunk,*a,*c);
    std::cout << "sizeof(A)+sizeof(C)=" << (sizeof(A)+sizeof(C)) << std::endl;
    std::cout << "span: elementSize=" << Span<A,C>::ElementSize << ",startIdx="<<span->startIdx <<std::endl;
    std::cout << "sizeof((Span<A, C>)=" << sizeof(Span<A, C>) <<", while sizeof(span->ElementSize)="<<sizeof(Span<A,C>::ElementSize)<< std::endl;*/
    ChunkAllocator::Free(chunk);
}

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
