// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
//#include "../../vmalloc/chunk.h"
//#include "../../vcontainer/span.h"
//#include "../../vmalloc/ChunkMgr.h"
//#include "../../vcontainer/algorithm.h";
//#include "../../vcontainer/tuple.h"
#include "../../vcontainer/pool.h"

using namespace valkyr;

struct A {
	int i0;
	int i1;

	A() :i0(0), i1(0) {}
	A(int a, int b) :i0(a), i1(b) {}
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
	Position2D() :x(0), y(0) {}
	Position2D(float x, float y) :x(x), y(y) {}
};

struct Position {
	float x, y, z;
	Position() :x(0), y(0), z(0) {}
	Position(float x, float y, float z) :x(x), y(y), z(z) {}
};

struct C {
	float baseline;
	int num;

	C() :baseline(0), num(0) {}

	C(float b, int n) :baseline(b), num(n) {}
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
	Position* pos = ChunkUtil::NewObjFrom<Position, float, float, float>(chunk, 100.9f, 222.22f, 333.333f);
	std::cout << "pos:x=" << pos->x << ",y=" << pos->y << ",z=" << pos->z << std::endl;
	std::cout << "chunk first.baseline=" << ChunkUtil::Get<C>(sizeof(ChunkInfo), chunk)->baseline << std::endl;
	ChunkAllocator::Free(chunk);
}

void chunkMgrTest() {
	g_chunkMgr = ChunkMgr::Create([&](ChunkMgr* chunkMgr) {
		g_currChunk = chunkMgr->m_currChunk;
	});
	vfor_each(g_chunkMgr, [&](Chunk* chunk, int i) {
		std::cout <<i<<":head="<< ChunkUtil::GetInfo(chunk)->head << ",address=" << chunk << std::endl;
	});
	std::cout <<"g_currChunk="<<  g_currChunk << std::endl;
	Chunk* next = nullptr;
	for (int i = 0; i < MAX_USE_NEXT_CHUNK; i++) {
		next = g_chunkMgr->UseNextChunk([](Chunk* chunk) {g_currChunk = chunk; });
		std::cout << "next" << i << " address=" << next << ",after use next, curr" << i << " address:" << g_currChunk << std::endl;
	}
	//g_chunkMgr->AddChunk();
	vfor_each(g_chunkMgr, [&](Chunk* chunk, int i) {
		std::cout << i << ":head=" << ChunkUtil::GetInfo(chunk)->head<<",address="<<chunk << std::endl;
		});
	g_chunkMgr->Destroy([]() {g_currChunk = nullptr; g_chunkMgr = nullptr; });
}

void myTupleTest() {
	Chunk* chunk = ChunkAllocator::Malloc();
	Tuple<int, int, C>* pSTuple = nullptr;
	pSTuple = vmake_tuple_ptr(chunk, 10, 20, C());
	std::cout << "sizeof(*pSTuple)=" << sizeof(*pSTuple) << std::endl;
	std::cout << "after std::tuple created,chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
	C* pc3 = &std::get<2>(*pSTuple);
	pc3->num = 666;
	std::cout << "std::get<0>(*pSTuple)=" << vget<0>(*pSTuple) << std::endl;
	std::cout << "(&std::get<2>(*pSTuple))->num=" << (&vget<2>(*pSTuple))->num << std::endl;
	auto atuple = vmake_tuple(chunk, 222, 333, C(222, 333));
	std::cout << "after std::tuple created,chunk head=" << ChunkUtil::GetInfo(chunk)->head << std::endl;
	std::cout << "std::get<0>(atuple)=" << vget<0>(atuple) << std::endl;
	Tuple<int, int, C>* pATuple = &atuple;
	C* pc4 = &std::get<2>(*pATuple);
	pc4->num = 3333;
	std::cout << "vget<2>(pATuple).num = std::get<2>(*pATuple).num=" << vget<2>(pATuple).num << std::endl;
	std::cout << "vget_ptr<C,2>(pATuple)->num=" << vget_ptr<C, 2>(pATuple)->num << std::endl;
	auto& [x, y, c] = atuple;
	std::cout << x << "," << y << "," << c.num << std::endl;
	auto* pTp = vmake_tuple_ptr<A, C>(chunk);
	A* a = vget_ptr<A, 0>(pTp);
	ChunkAllocator::Free(chunk);
}

void spanTest() {
	Chunk* chunk = ChunkAllocator::Malloc();
	Span<Tuple<A, C>>* span = SpanUtil::Create<A, C>(chunk, 8, A(2, 3), C(222, 333));
	auto* p0 = SpanUtil::Get(0, span);
	std::cout << "span 0: A a.i0=" << vget_ptr<A, 0>(p0)->i0 << ",C c.num=" << vget<1>(p0).num << std::endl;
	auto* p6 = SpanUtil::Get(6, span);
	auto& [ar, cr] = *p6;
	A* a = &ar;
	C* c = &cr;
	a->i0 = 666;
	c->num = 3333;
	vfor_each<Tuple<A, C>>(span, [&](auto* pv, int i) {
		std::cout << "span" << i << ": A a.i0=" << vget_ptr<A, 0>(pv)->i0 << ",C c.num=" << vget<1>(pv).num << std::endl;
	});
	vfor_each_entt<Tuple<A, C>>(span, [&](SpanEntity* ent, int i) {
		std::cout << "span" << i << " entt:id= " << ent->id << ",version=" << ent->isZero << std::endl;
		});
	SpanUtil::Zero(span, 2);
	auto azero = SpanUtil::PickZero(span);
	std::cout << "zero one:"<<azero<<", A a.10=" << vget_ptr<A, 0>(azero)->i0 << ",C c.num=" << vget<1>(azero).num << std::endl;
	SpanUtil::ZeroRange(span, 6, 2);
	std::cout << "span 6: A a.i0=" << vget_ptr<A, 0>(SpanUtil::Get(6, span))->i0 << ",C c.num=" << vget<1>(SpanUtil::Get(6, span)).num << std::endl;
	std::cout << "span 7: A a.i0=" << vget_ptr<A, 0>(SpanUtil::Get(7, span))->i0 << ",C c.num=" << vget<1>(SpanUtil::Get(7, span)).num << std::endl;
	vfor_each_entt<Tuple<A, C>>(span, [&](SpanEntity* ent, int i) {
		std::cout << "span" << i << " entt:id= " << ent->id << ",version=" << ent->isZero << std::endl;
	});
	SpanUtil::ZeroAll(span);
	vfor_each<Tuple<A, C>>(span, [&](auto* pv, int i) {
		std::cout << "span" << i << ": "<<pv <<",A a.i0=" << vget_ptr<A, 0>(pv)->i0 << ",C c.num=" << vget<1>(pv).num << std::endl;
	});
	vfor_each_entt<Tuple<A, C>>(span, [&](SpanEntity* ent, int i) {
		std::cout << "span" << i << " entt:id= " << ent->id << ",version=" << ent->isZero << std::endl;
	});
	ChunkAllocator::Free(chunk);
}

void spanTest2() {
	using Tuple_t = Tuple<A, C>;
	Chunk* chunk = ChunkAllocator::Malloc();
	auto span = SpanUtil::Create<A, C>(chunk, 8, A(2, 3), C(222, 333));
	auto span2 = SpanUtil::Create<A, C>(chunk, 8, A(3, 2), C(333, 222));
	SpanUtil::Connect(span, span2);
	SpanUtil::Zero(span2,2);
	//Tuple<A,C>* tgt = vpick<Span<Tuple<A,C>>,Tuple<A,C>>(span,[&](Span<Tuple<A,C>>* curr) { {SpanUtil::PickZero(curr); });
	
	/*Tuple_t* azero = vpick<Span<Tuple_t>,Tuple_t>(span,
		[&](auto* curr) { return SpanUtil::PickZero(curr); });
	std::cout << "zero one:" << azero << ", A a.10=" << vget_ptr<A, 0>(azero)->i0 << ",C c.num=" << vget<1>(azero).num << std::endl;
	std::cout << "span 2 has zero:" << SpanUtil::Get(2, span2) << std::endl;
	*/
	auto res = vpick_with_entity<Span<Tuple_t>, Tuple_t>(span, [&](auto* curr) { return SpanUtil::PickZeroWithEntity(curr); });
	std::cout << "zero one:" << res.first << ", A a.10=" << vget_ptr<A, 0>(res.first)->i0 << ",C c.num=" << vget<1>(res.first).num << std::endl;
	std::cout << "res entity is zero=" << (res.second->isZero ? "true" : "false") << std::endl;
	std::cout << "span 2 has zero:" << SpanUtil::Get(2, span2) << std::endl;
	ChunkAllocator::Free(chunk);
}

void poolTest() {
	g_chunkMgr = ChunkMgr::Create([&](ChunkMgr* chunkMgr) {
		g_currChunk = chunkMgr->m_currChunk;
	});
	Pool<A>* pool = PoolUtil::Create<A>(g_chunkMgr);
	auto item = PoolUtil::Pop(pool);
	A* a = item.first;
	std::cout << "new a:" << a->i0 <<",entity id="<<item.second->id<<" isZero="<<item.second->isZero<< std::endl;
	g_chunkMgr->Destroy([]() {g_currChunk = nullptr; g_chunkMgr = nullptr; });
}

int main()
{
	//chunkTest();
	//chunkMgrTest();
	spanTest2();
	//myTupleTest();
	system("pause");
	return 0;
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