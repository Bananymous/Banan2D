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

int main(int argc, char** argv)
{
	using namespace Banan::Networking;

	InternetLayer il;
	if (argc == 2 && std::memcmp(argv[1], "IPv4", 4) == 0)
		il = InternetLayer::IPv4;
	else if (argc == 2 && std::memcmp(argv[1], "IPv6", 4) == 0)
		il = InternetLayer::IPv6;
	else
	{
		BANAN_PRINT("Please specify IPv4 or IPv6 in arguments!\n");
		return 0;
	}

	auto server = Server::Create(54000, TransportLayer::TCP, il);

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

#endif