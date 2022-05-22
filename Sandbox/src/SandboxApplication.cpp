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

using namespace Banan::ECS;

int main(int argc, char** argv)
{
	Registry registry;

	auto e1 = registry.Create();
	auto e2 = registry.Create();

	registry.Add<int>(e1, 69);
	registry.Add<int>(e2, 420);

	for (int n : registry.View<int>())
		BANAN_PRINT("%d\n", n);

}

#endif