#pragma once

#include "Banan/Networking/Common.h"

#include "Banan/Core/Base.h"

#include <functional>

namespace Banan::Networking
{

	class Client
	{
	public:
		virtual ~Client() {}

		virtual void Connect() = 0;
		virtual void Disconnect() = 0;

		virtual bool IsConnected() const = 0;

		virtual void Send(const Message& message) = 0;

		virtual void SetMessageCallback(std::function<void(const Message&)> callback) = 0;
		virtual void SetConnectionCallback(std::function<void()> callback) = 0;
		virtual void SetDisconnectionCallback(std::function<void()> callback) = 0;

		virtual void QueryUpdates() = 0;

		static Scope<Client> Create(const std::string& ip, int port, TransportLayer tl = TransportLayer::TCP, InternetLayer il = InternetLayer::IPv4);
	};

}