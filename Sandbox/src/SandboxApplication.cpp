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
		BANAN_PRINT("%f ms\n", std::chrono::duration<float, std::milli>(end - start).count());
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

	{
		auto entity = r.Create();
		r.Emplace<comp<0>>(entity);
		r.Emplace<comp<1>>(entity);
		r.Emplace<comp<2>>(entity);
		r.Emplace<comp<3>>(entity);
		r.Emplace<comp<4>>(entity);
		r.Emplace<comp<5>>(entity);
		r.Emplace<comp<6>>(entity);
		r.Emplace<comp<7>>(entity);
		r.Emplace<comp<8>>(entity);
		r.Emplace<comp<9>>(entity);
		r.Emplace<comp<10>>(entity);
		r.Emplace<comp<11>>(entity);
		r.Emplace<comp<12>>(entity);
		r.Emplace<comp<13>>(entity);
		r.Emplace<comp<14>>(entity);
		r.Emplace<comp<15>>(entity);
		r.Emplace<comp<16>>(entity);
		r.Emplace<comp<17>>(entity);
		r.Emplace<comp<18>>(entity);
		r.Emplace<comp<19>>(entity);
		r.Emplace<comp<20>>(entity);
		r.Emplace<comp<21>>(entity);
		r.Emplace<comp<22>>(entity);
		r.Emplace<comp<23>>(entity);
		r.Emplace<comp<24>>(entity);
		r.Emplace<comp<25>>(entity);
		r.Emplace<comp<26>>(entity);
		r.Emplace<comp<27>>(entity);
		r.Emplace<comp<28>>(entity);
		r.Emplace<comp<29>>(entity);
		r.Emplace<comp<30>>(entity);
		r.Emplace<comp<31>>(entity);
		r.Emplace<comp<32>>(entity);
		r.Emplace<comp<33>>(entity);
		r.Emplace<comp<34>>(entity);
		r.Emplace<comp<35>>(entity);
		r.Emplace<comp<36>>(entity);
		r.Emplace<comp<37>>(entity);
		r.Emplace<comp<38>>(entity);
		r.Emplace<comp<39>>(entity);
		r.Emplace<comp<40>>(entity);
		r.Emplace<comp<41>>(entity);
		r.Emplace<comp<42>>(entity);
		r.Emplace<comp<43>>(entity);
		r.Emplace<comp<44>>(entity);
		r.Emplace<comp<45>>(entity);
		r.Emplace<comp<46>>(entity);
		r.Emplace<comp<47>>(entity);
		r.Emplace<comp<48>>(entity);
		r.Emplace<comp<49>>(entity);
		r.Emplace<comp<50>>(entity);
		r.Emplace<comp<51>>(entity);
		r.Emplace<comp<52>>(entity);
		r.Emplace<comp<53>>(entity);
		r.Emplace<comp<54>>(entity);
		r.Emplace<comp<55>>(entity);
		r.Emplace<comp<56>>(entity);
		r.Emplace<comp<57>>(entity);
		r.Emplace<comp<58>>(entity);
		r.Emplace<comp<59>>(entity);
		r.Emplace<comp<60>>(entity);
		r.Emplace<comp<61>>(entity);
		r.Emplace<comp<62>>(entity);
		r.Emplace<comp<63>>(entity);
		r.Emplace<comp<64>>(entity);
		r.Emplace<comp<65>>(entity);
		r.Emplace<comp<66>>(entity);
		r.Emplace<comp<67>>(entity);
		r.Emplace<comp<68>>(entity);
		r.Emplace<comp<69>>(entity);
		r.Emplace<comp<70>>(entity);
		r.Emplace<comp<71>>(entity);
		r.Emplace<comp<72>>(entity);
		r.Emplace<comp<73>>(entity);
		r.Emplace<comp<74>>(entity);
		r.Emplace<comp<75>>(entity);
		r.Emplace<comp<76>>(entity);
		r.Emplace<comp<77>>(entity);
		r.Emplace<comp<78>>(entity);
		r.Emplace<comp<79>>(entity);
		r.Emplace<comp<80>>(entity);
		r.Emplace<comp<81>>(entity);
		r.Emplace<comp<82>>(entity);
		r.Emplace<comp<83>>(entity);
		r.Emplace<comp<84>>(entity);
		r.Emplace<comp<85>>(entity);
		r.Emplace<comp<86>>(entity);
		r.Emplace<comp<87>>(entity);
		r.Emplace<comp<88>>(entity);
		r.Emplace<comp<89>>(entity);
		r.Emplace<comp<90>>(entity);
		r.Emplace<comp<91>>(entity);
		r.Emplace<comp<92>>(entity);
		r.Emplace<comp<93>>(entity);
		r.Emplace<comp<94>>(entity);
		r.Emplace<comp<95>>(entity);
		r.Emplace<comp<96>>(entity);
		r.Emplace<comp<97>>(entity);
		r.Emplace<comp<98>>(entity);
		r.Emplace<comp<99>>(entity);
		r.Emplace<comp<100>>(entity);
		r.Emplace<comp<101>>(entity);
		r.Emplace<comp<102>>(entity);
		r.Emplace<comp<103>>(entity);
		r.Emplace<comp<104>>(entity);
		r.Emplace<comp<105>>(entity);
		r.Emplace<comp<106>>(entity);
		r.Emplace<comp<107>>(entity);
		r.Emplace<comp<108>>(entity);
		r.Emplace<comp<109>>(entity);
		r.Emplace<comp<110>>(entity);
		r.Emplace<comp<111>>(entity);
		r.Emplace<comp<112>>(entity);
		r.Emplace<comp<113>>(entity);
		r.Emplace<comp<114>>(entity);
		r.Emplace<comp<115>>(entity);
		r.Emplace<comp<116>>(entity);
		r.Emplace<comp<117>>(entity);
		r.Emplace<comp<118>>(entity);
		r.Emplace<comp<119>>(entity);
		r.Emplace<comp<120>>(entity);
		r.Emplace<comp<121>>(entity);
		r.Emplace<comp<122>>(entity);
		r.Emplace<comp<123>>(entity);
		r.Emplace<comp<124>>(entity);
		r.Emplace<comp<125>>(entity);
		r.Emplace<comp<126>>(entity);
		r.Emplace<comp<127>>(entity);
		r.Emplace<comp<128>>(entity);
		r.Emplace<comp<129>>(entity);
		r.Emplace<comp<130>>(entity);
		r.Emplace<comp<131>>(entity);
		r.Emplace<comp<132>>(entity);
		r.Emplace<comp<133>>(entity);
		r.Emplace<comp<134>>(entity);
		r.Emplace<comp<135>>(entity);
		r.Emplace<comp<136>>(entity);
		r.Emplace<comp<137>>(entity);
		r.Emplace<comp<138>>(entity);
		r.Emplace<comp<139>>(entity);
		r.Emplace<comp<140>>(entity);
		r.Emplace<comp<141>>(entity);
		r.Emplace<comp<142>>(entity);
		r.Emplace<comp<143>>(entity);
		r.Emplace<comp<144>>(entity);
		r.Emplace<comp<145>>(entity);
		r.Emplace<comp<146>>(entity);
		r.Emplace<comp<147>>(entity);
		r.Emplace<comp<148>>(entity);
		r.Emplace<comp<149>>(entity);
		r.Emplace<comp<150>>(entity);
		r.Emplace<comp<151>>(entity);
		r.Emplace<comp<152>>(entity);
		r.Emplace<comp<153>>(entity);
		r.Emplace<comp<154>>(entity);
		r.Emplace<comp<155>>(entity);
		r.Emplace<comp<156>>(entity);
		r.Emplace<comp<157>>(entity);
		r.Emplace<comp<158>>(entity);
		r.Emplace<comp<159>>(entity);
		r.Emplace<comp<160>>(entity);
		r.Emplace<comp<161>>(entity);
		r.Emplace<comp<162>>(entity);
		r.Emplace<comp<163>>(entity);
		r.Emplace<comp<164>>(entity);
		r.Emplace<comp<165>>(entity);
		r.Emplace<comp<166>>(entity);
		r.Emplace<comp<167>>(entity);
		r.Emplace<comp<168>>(entity);
		r.Emplace<comp<169>>(entity);
		r.Emplace<comp<170>>(entity);
		r.Emplace<comp<171>>(entity);
		r.Emplace<comp<172>>(entity);
		r.Emplace<comp<173>>(entity);
		r.Emplace<comp<174>>(entity);
		r.Emplace<comp<175>>(entity);
		r.Emplace<comp<176>>(entity);
		r.Emplace<comp<177>>(entity);
		r.Emplace<comp<178>>(entity);
		r.Emplace<comp<179>>(entity);
		r.Emplace<comp<180>>(entity);
		r.Emplace<comp<181>>(entity);
		r.Emplace<comp<182>>(entity);
		r.Emplace<comp<183>>(entity);
		r.Emplace<comp<184>>(entity);
		r.Emplace<comp<185>>(entity);
		r.Emplace<comp<186>>(entity);
		r.Emplace<comp<187>>(entity);
		r.Emplace<comp<188>>(entity);
		r.Emplace<comp<189>>(entity);
		r.Emplace<comp<190>>(entity);
		r.Emplace<comp<191>>(entity);
		r.Emplace<comp<192>>(entity);
		r.Emplace<comp<193>>(entity);
		r.Emplace<comp<194>>(entity);
		r.Emplace<comp<195>>(entity);
		r.Emplace<comp<196>>(entity);
		r.Emplace<comp<197>>(entity);
		r.Emplace<comp<198>>(entity);
		r.Emplace<comp<199>>(entity);
	}

	r.Clean();

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

	while (true);
}

#endif