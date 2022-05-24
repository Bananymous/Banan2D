#include <Banan2D.h>
//#include <Banan/Core/EntryPoint.h>

#include <Banan/ECS/Registry.h>

#include "SandboxLayer.h"

class Sandbox : public Banan::Application
{
public:
	Sandbox() :
		Application("Sandbox", 1280, 720, false)
	{
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{

	}

};


Banan::Application* Banan::CreateApplication()
{
	return new Sandbox();
}


#if 1


#include <chrono>

using namespace Banan::ECS;

struct position { uint64_t x, y; };
struct velocity { uint64_t x, y; };

template<auto>
struct comp { int x; };

struct ScopeTimer
{
	ScopeTimer()
	{
		start = std::chrono::system_clock::now();
	}

	~ScopeTimer()
	{
		auto end = std::chrono::system_clock::now();
		BANAN_PRINT("%f\n", std::chrono::duration<float, std::milli>(end - start).count());
	}

	std::chrono::system_clock::time_point start;
};

int main(int argc, char** argv)
{
	Registry r;

	for (uint64_t i = 0; i < 500'000; i++)
	{
		auto entity = r.Create();
		r.Emplace<position>(entity);
		r.Emplace<velocity>(entity);
		r.Emplace<comp<0>>(entity);
	}

	BANAN_PRINT("PHASE 1 DONE\n");

	for (uint64_t i = 0; i < 10; i++)
	{
		r.Each([i = 0, &r](Entity entity) mutable
		{
			if (!(++i % 7))
				r.Remove<position>(entity);
			if (!(++i % 11))
				r.Remove<velocity>(entity);
			if (!(++i % 13))
				r.Remove<comp<0>>(entity);
			//if (!(++i % 17))
			//	r.Destroy(entity);
		});

		for (uint64_t j = 0; j < 50'000; j++)
		{
			auto entity = r.Create();
			r.Emplace<position>(entity);
			r.Emplace<velocity>(entity);
			r.Emplace<comp<0>>(entity);
		}
	}

	BANAN_PRINT("PHASE 2 DONE\n");

	BANAN_PRINT("Set position.x with r.View<position>()\n");
	{
		ScopeTimer t;
		for (position& p : r.View<position>())
			p.x = 1;
	}

	BANAN_PRINT("Set position.y and read comp<0> with r.View<position, comp<0>()\n");
	{
		ScopeTimer t;
		for (auto[p, _] : r.View<position, comp<0>>())
			p.y = 1;
	}

	BANAN_PRINT("Confirming results!\n");
	{
		uint64_t cnt = 0;
		for (position& p : r.View<position>())
			if (p.x != 1)
				cnt++;
		BANAN_PRINT("r.View<position>() failed %llu times\n", cnt);

		cnt = 0;
		for (auto[p, _] : r.View<position, comp<0>>())
			if (p.y != 1)
				cnt++;
		BANAN_PRINT("r.View<position, comp<0>>() failed %llu times\n", cnt);
	}



	while (true);
}

#endif