#pragma once

#include "Banan/Networking/Server.h"

namespace Banan::Networking
{

	class WindowsServer : public Server
	{
	public:
		WindowsServer(uint64_t thread_count);
		~WindowsServer();

		virtual void Start(int port, InternetLayer il = InternetLayer::IPv4) override;
		virtual void Stop() override;

		virtual bool IsActive() const override;

		virtual void Send(const Message& message, Socket socket) override;
		virtual void SendAll(const Message& message, Socket skip = ~Socket(0)) override;

		virtual void SetMessageCallback(std::function<void(Socket, const Message&)> callback) override;
		virtual void SetConnectionCallback(std::function<void(Socket)> callback) override;
		virtual void SetDisconnectionCallback(std::function<void(Socket)> callback) override;

		virtual void QueryUpdates() override;

		virtual std::string GetIP(Socket socket) const override;
	};

}