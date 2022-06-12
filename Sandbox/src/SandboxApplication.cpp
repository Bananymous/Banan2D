#if 0

#include <Banan2D.h>
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

#include <Banan2D.h>
#include "Banan/Networking/Server.h"
#include "Banan/Networking/Client.h"

#include <thread>
#include <iostream>

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

struct pos { int x, y; };
namespace Banan::Networking
{
	template<> void Serialize  <pos>(std::ostream& os, const pos& p) { os << bits(p.x) << bits(p.y); }
	template<> void Deserialize<pos>(std::istream& is,       pos& p) { is >> bits(p.x) >> bits(p.y); }
}

using namespace Banan;
using namespace Banan::Networking;

void InputThread(Scope<Client>& client)
{
	while (client->IsConnected())
	{
		std::string input;
		std::getline(std::cin, input);

		if (input == "stop")
		{
			client->Disconnect();
			break;
		}
		
		client->Send(Message::Create(input));
	}
}

int main(int argc, char** argv)
{
#if 1
	int type = ParseArguments(argc, argv);

	if (type == 1)
	{
		auto server = Server::Create();

		server->SetMessageCallback(
			[&](Socket sock, const Message& msg)
			{
				std::string str;
				msg.Get(str);
				BANAN_PRINT("%lu: %s\n", sock, str.c_str());

				server->SendAll(msg, sock);
			}
		);

		server->SetConnectionCallback(
			[&](Socket sock)
			{
				BANAN_PRINT("%lu connected (%s)\n", sock, server->GetIP(sock).c_str());
				server->Send(Message::Create(std::string("Hello!")), sock);
			}
		);

		server->SetDisconnectionCallback(
			[](Socket sock)
			{
				BANAN_PRINT("%lu disconnected\n", sock);
			}
		);

		server->Start(54000);

		BANAN_PRINT("Server started!\n");

		while (true)
		{
			server->QueryUpdates();
		}
	}
	else if (type == 2)
	{
		auto client = Client::Create();
		std::thread input;

		client->SetMessageCallback(
			[](const Message& msg)
			{
				std::string str;
				msg.Get(str);
				BANAN_PRINT("%s\n", str.c_str());
			}
		);

		client->SetConnectionCallback(
			[]()
			{
				BANAN_PRINT("Connected to server!\n");
			}
		);

		client->SetDisconnectionCallback(
			[]()
			{
				BANAN_PRINT("Disconnected!\n");
			}
		);

		client->Connect("localhost", 54000);

		input = std::thread(&InputThread, std::ref(client));
		while (client->IsConnected())
		{
			client->QueryUpdates();
		}

		if (input.joinable())
			input.join();
	}
#endif
	
	return 0;
}

#endif