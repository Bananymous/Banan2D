#pragma once

#include "Banan/Networking/Common.h"

#include "Banan/Core/Base.h"

#include <functional>

namespace Banan::Networking
{

	class Server
	{
	public:
		virtual ~Server() {}

		virtual void Start() = 0;
		virtual void Stop() = 0;

		virtual bool IsActive() const = 0;

		virtual void Send(const Message& message, Socket socket) = 0;
		virtual void SendAll(const Message& message, Socket skip = InvalidSocket) = 0;

		virtual void SetMessageCallback(std::function<void(Socket, const Message&)> callback) = 0;
		virtual void SetConnectionCallback(std::function<void(Socket)> callback) = 0;
		virtual void SetDisconnectionCallback(std::function<void(Socket)> callback) = 0;

		virtual void QueryUpdates() = 0;

		static Scope<Server> Create(int port, TransportLayer tl = TransportLayer::TCP, InternetLayer ip = InternetLayer::IPv4);
	};

}