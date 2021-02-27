// vtest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <chrono>   
#include <random>

#include <unordered_set>
#include <vector>
#include <future>
#include <numeric>
#include "../../vcontainer/span.h"
//#include "../../vcontainer/tuple.h"
#include "../../vcontainer/pool.h"
#include "../../vtask/Scheduler.h"

using namespace valkyr;
using std::cout;
using std::endl;

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

struct A {
	int i0;
	int i1;
	Position pos;
	Position2D uv;

	A() :i0(0), i1(0) {}
	A(int a, int b) :i0(a), i1(b){}
};

struct B {
	int code;
	char big[CHUNK_SIZE - sizeof(ChunkInfo) - sizeof(int)];
};

struct Rot {
	float angle;
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
	std::cout << "vget_ptr<C,2>(pATuple)->num=" << vget_ptr<2,C>(pATuple)->num << std::endl;
	auto& [x, y, c] = atuple;
	std::cout << x << "," << y << "," << c.num << std::endl;
	auto* pTp = vmake_tuple_ptr<A, C>(chunk);
	A* a = vget_ptr<0,A>(pTp);
	ChunkAllocator::Free(chunk);
}

void spanTest() {
	Chunk* chunk = ChunkAllocator::Malloc();
	Span<Tuple<A, C>>* span = SpanUtil::Create<Tuple<A,C>>(chunk, 8);
	auto* p0 = SpanUtil::Get(0, span);
	std::cout << "span 0: A a.i0=" << vget_ptr<0,A>(p0)->i0 << ",C c.num=" << vget<1>(p0).num << std::endl;
	auto* p6 = SpanUtil::Get(6, span);
	auto& [ar, cr] = *p6;
	A* a = &ar;
	C* c = &cr;
	a->i0 = 666;
	c->num = 3333;
	SpanUtil::ForEach<Tuple<A,C>>(span, [&](auto* pv, int i) {
		std::cout << "span" << i << ": A a.i0=" << vget_ptr<0,A>(pv)->i0 << ",C c.num=" << vget<1>(pv).num << std::endl;
		});
	SpanUtil::Zero(span, 2);
	SpanUtil::ZeroRange(span, 6, 2);
	std::cout << "span 6: A a.i0=" << vget_ptr<0,A>(SpanUtil::Get(6, span))->i0 << ",C c.num=" << vget<1>(SpanUtil::Get(6, span)).num << std::endl;
	std::cout << "span 7: A a.i0=" << vget_ptr<0,A>(SpanUtil::Get(7, span))->i0 << ",C c.num=" << vget<1>(SpanUtil::Get(7, span)).num << std::endl;
	SpanUtil::ZeroAll(span);
	SpanUtil::ForEach<Tuple<A, C>>(span, [&](auto* pv, int i) {
		std::cout << "span" << i << ": "<<pv <<",A a.i0=" << vget_ptr<0,A>(pv)->i0 << ",C c.num=" << vget<1>(pv).num << std::endl;
	});
	ChunkAllocator::Free(chunk);
}

void spanTest2() {
	using Tuple_t = Tuple<A, C>;
	Chunk* chunk = ChunkAllocator::Malloc();
	auto span = SpanUtil::Create<Tuple<A,C>>(chunk, 8);
	auto span2 = SpanUtil::Create<Tuple<A, C>>(chunk, 8);
	SpanUtil::Connect(span, span2);
	SpanUtil::Zero(span2,2);
	ChunkAllocator::Free(chunk);
}

void poolTest1() {
	using MyEntt_t = Tuple<C, Rot, Position>;
	Chunk* chunk = ChunkAllocator::Malloc();
	Pool<Tuple<C, Rot, Position>>* pool = PoolUtil::CreateAndFill<MyEntt_t>(chunk);
	auto item = PoolUtil::Pop<MyEntt_t>(pool);
	vget_ptr<1, Rot>(item->data)->angle = 90.0f;
	std::cout << "item->id << item->isZero << vget_ptr<1, Rot>(item->data)->angle" << std::endl;
	std::cout << item->id <<","<< item->isZero<<"," << vget_ptr<1, Rot>(item->data)->angle << std::endl;
	std::cout << "=======================" << std::endl;
	int i = 0;
	PoolUtil::TraverseZero<MyEntt_t>(pool, [&i](Entt<MyEntt_t>* entt) {
			if (i%100==0) {
				std::cout << "entt->id << entt->isZero << vget_ptr<1, Rot>(entt->data)->angle" << std::endl;
				std::cout << entt->id<<"," << entt->isZero<<"," << vget_ptr<1, Rot>(entt->data)->angle << std::endl;
			}
			i++;
		});
}

void poolTest() {
	using MyEntt_t = Tuple<C, Rot, Position>;
	Chunk* chunk = ChunkAllocator::Malloc();
	Pool<Tuple<C,Rot,Position>>* pool = PoolUtil::CreateAndFill<MyEntt_t>(chunk);
	std::default_random_engine randEngine;
	std::uniform_int_distribution<unsigned> randDist(0, 9);
	auto start = std::chrono::system_clock::now();
	size_t maxCapacity = 10000000;// PoolUtil::GetMaxCapacityFor<Tuple<C,Rot,Position>>(chunk);
	Pool<MyEntt_t>* curr = pool;
	for (size_t i = 0; i < maxCapacity; i++) {
		auto it = PoolUtil::PopOrCreate<MyEntt_t>(curr);
		curr = it.second;
		if (!it.first) continue;
		C* c = vget_ptr<C>(it.first->data);
		c->baseline = i;
		c->num = 2233;
		if (randDist(randEngine) > 5) {
			PoolUtil::Push<MyEntt_t>(it.first,curr);
		}
		if (i % (1000000) == 0) {
			std::cout << "pool=" << curr << std::endl;
			std::cout << i << " c num:" << vget_ptr<C>(it.first->data)->num << ",entity id=" << it.first->id << " isZero=" << it.first->isZero << std::endl;
		}
	}
	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	auto sec = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	std::cout <<"耗时:"<< sec << std::endl;
	std::cout<<"test count="<<maxCapacity << ",first pool capacity=" << pool->capacity << ",zero count=" << pool->zeroCount << std::endl;
	PoolUtil::FreeAll(pool);
	std::cout << "====对照组========" << std::endl;
	std::vector<std::tuple<C,Rot,Position>> vec;
	auto start2 = std::chrono::system_clock::now();
	for (size_t i = 0; i < maxCapacity; i++) {
		vec.push_back(std::make_tuple(C(),Rot(),Position()));
		/*if (randDist(randEngine) > 5) {
			std::get<0>(vec[i]).baseline = 2233;
		}*/
		if (i % (1000000) == 0) {
			std::cout << "vec" << i << std::endl;
			//std::cout << i << "vec[i].baseline:" << std::get<0>(vec[i]).baseline << std::endl;
		}
	}
	auto end2 = std::chrono::system_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);
	auto sec2 = double(duration2.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	std::cout << "vec耗时:" << sec2 << std::endl;
}

void poolGroupTest() {
	Chunk* chunk = ChunkAllocator::Malloc();
	size_t num = 100;
	PoolGroup<A>* group = PoolUtil::CreateGroup<A>(chunk,1);
	for (int i = 0; i < num; i++) {
		auto r = PoolUtil::PopOrNewFrom<A>(group);
		if (i % 10 == 0) {
			std::cout << "item" << std::endl;
			std::cout << "r.first->id=" << r.first->id << std::endl;
			std::cout << "r.first->data->i0=" << r.first->data->i0 << std::endl;
			std::cout << "r.second=" << r.second << std::endl;
			std::cout << "r.second->zeroCount=" << r.second->zeroCount << std::endl;
			std::cout << "r.second->frontZero->id=" << (!r.second->frontZero ? "null" : "" + r.second->frontZero->id) << std::endl;
			std::cout << "end" << std::endl;
		}
	}
	std::cout << " group->poolCount=" << group->poolCount << std::endl;
	PoolUtil::FreeAll(group);
	/*PoolUtil::Free(group->firstPool);
	ChunkAllocator::Free(chunk);*/
}

using std::thread;
using std::this_thread::sleep_for;

struct CodeTeam {
	TaskQueue& codeRepo;
	//std::vector<bool> workersTouchingFish;
	bool running;
	std::atomic_size_t currGroupCompletedTasks;

	void coderGotoIcu(int no) {
		while (running) {
			unsigned waitTime = 1000u;
			auto task = codeRepo.get();
			if (task)
				task.value()->exec();
			currGroupCompletedTasks++;
			sleep_for(std::chrono::milliseconds(waitTime));
		}
		std::this_thread::yield();
	}

	CodeTeam(TaskQueue& queue):codeRepo(queue),running(true)//,workersTouchingFish(queue.len,true)
	{}

};

class PrintTask : public Task {
public:
	unsigned m_v;

	PrintTask(unsigned v):m_v(v){
	}

	void exec() {
		cout <<std::this_thread::get_id()<<":"<< m_v << std::endl;
	}
};

class ComputeTask : public Task {
public:
	Vec<int> input;
	int output;

	ComputeTask(std::initializer_list<int> inputs):input(inputs),output(0) 
	{
	}

	void exec() {
		output = std::accumulate(input.begin(), input.end(), 0);
		cout <<"compute result:"<< output << std::endl;
	}
};


void testTasks() {
	TaskQueue queue(4);
	vptr<CodeTeam> team = vmake_ptr<CodeTeam>(queue);
	for (auto i = 0u; i < 4; i++) {
		thread th(&CodeTeam::coderGotoIcu, team, 0);
		th.detach();
	}
	std::default_random_engine randEngine;
	std::uniform_int_distribution<unsigned> randDist(0u, 2048u);
	while(true) {
		unsigned x = randDist(randEngine);
		if (x > 2000u) {
			team->running = false;
			return;
		}
		if (team->currGroupCompletedTasks > 4) {
			cout << "add tasks of next task group" << endl;
			team->currGroupCompletedTasks = 0;
		}
		vptr<Task> t = vmake_ptr<PrintTask>(x);
		queue.put(t);
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}

Vec<int> blackboard;

TaskSeq genTaskSeq() {
	TaskSeq seq;
	vptr<Task> t10 = vmake_ptr<PrintTask>(10u);
	vptr<Task> t11 = vmake_ptr<PrintTask>(11u);
	TaskGroup group1 = {t10,t11};
	seq.push(group1);
	vptr<Task> t20 = vmake_ptr<ComputeTask>(std::initializer_list<int>{1,2,3,4,5,6});
	vptr<Task> t21 = vmake_ptr<ComputeTask>(std::initializer_list<int>{1, 20, 300});
	vptr<Task> t22 = vmake_ptr<PrintTask>(65535u);
	TaskGroup group2 = { t20,t21,t22 };
	seq.push(group2);
	vptr<Task> t3 = vmake_ptr<PrintTask>(666u);
	TaskGroup group3 = { t3 };
	// seq.push(group3);
	return seq;
}

void testScheduler() {
	//std::default_random_engine randEngine;
	//std::uniform_int_distribution<unsigned> randDist(0u, 2048u);
	vptr<Scheduler> scheduler = vmake_ptr<Scheduler>(4);
	scheduler->InitWorkers();
	scheduler->Add(genTaskSeq());
	/*vptr<Task> t = vmake_ptr<PrintTask>(0);
	scheduler->Add(t);*/
	while (scheduler->m_workerCtx->running) {
		//vptr<Task> t = vmake_ptr<PrintTask>(x);
		if (scheduler->m_taskSeq.empty()&&scheduler->m_taskQueue.isEmpty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100u));
			scheduler->m_workerCtx->running = false;
			break;
		}
		scheduler->tick();
		//std::this_thread::sleep_for(std::chrono::milliseconds(500u));
	}
}

int main()
{
	//chunkTest();
	//spanTest2();
	//myTupleTest();
	poolTest();
	//poolGroupTest();
	//testScheduler();
#ifdef _WIN32
	system("pause");
#endif
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
