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

#include "Banan/Networking/Server.h"
#include "Banan/Networking/Client.h"

void PrintUsage()
{
	BANAN_PRINT("Please specify valid type.\n");
	BANAN_PRINT("  --client -c     Networking client\n");
	BANAN_PRINT("  --server -s     Networking server\n");
}

int ParseArguments(int argc, char** argv)
{
	if (argc != 2) {
		PrintUsage();
		return 0;	
	}

	if (!strcmp(argv[1], "--server") || !strcmp(argv[1], "-s"))
		return 1;
	if (!strcmp(argv[1], "--client") || !strcmp(argv[1], "-c"))
		return 2;

	PrintUsage();
	return 0;
}

int main(int argc, char** argv)
{
	using namespace Banan::Networking;

	int type = ParseArguments(argc, argv);

	if (type == 1)
	{
		auto server = Server::Create(54000);

		server->SetMessageCallback(
			[](Socket sock, const Message& msg)
			{
				BANAN_PRINT("%lu: %s\n", sock, msg.GetObject<std::string>().c_str());
			}
		);

		server->SetConnectionCallback(
			[&](Socket sock)
			{
				BANAN_PRINT("%lu connected (%s)\n", sock, server->GetIP(sock).c_str());
			}
		);

		server->SetDisconnectionCallback(
			[](Socket sock)
			{
				BANAN_PRINT("%lu disconnected\n", sock);
			}
		);

		server->Start();

		BANAN_PRINT("Server started!\n");

		while (true)
		{
			server->QueryUpdates();
		}
	}
	else if (type == 2)
	{

	}
	
	return 0;

	
}

#endif