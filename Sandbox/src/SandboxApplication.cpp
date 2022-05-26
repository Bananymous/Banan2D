#include <Banan2D.h>




#if 0

#include <Banan/Core/EntryPoint.h>

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


#else

int main(int argc, char** argv)
{
	
}

#endif