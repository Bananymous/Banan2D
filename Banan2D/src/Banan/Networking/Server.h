#pragma once

#include "Banan/Networking/Common.h"

#include "Banan/Core/Base.h"

#include <functional>
#include <thread>

namespace Banan::Networking
{

	class Server
	{
	public:
		virtual ~Server() {}

		virtual void Start(int port, InternetLayer il = InternetLayer::IPv4) = 0;
		virtual void Stop() = 0;

		virtual bool IsActive() const = 0;

		virtual void Send(Socket socket, const Message& message) = 0;
		virtual void SendAll(const Message& message, Socket skip = ~Socket(0)) = 0;

		virtual void Kick(Socket socket) = 0;

		virtual void SetMessageCallback(std::function<void(Socket, const Message&)> callback) = 0;
		virtual void SetConnectionCallback(std::function<void(Socket)> callback) = 0;
		virtual void SetDisconnectionCallback(std::function<void(Socket)> callback) = 0;

		virtual void QueryUpdates() = 0;

		virtual std::string GetIP(Socket socket) const = 0;

		// Thread count must be atleast 2. 1 is for epoll/select and the rest are for thread pool.
		static Scope<Server> Create(uint64_t thread_count = std::thread::hardware_concurrency() - 1);
	};

}