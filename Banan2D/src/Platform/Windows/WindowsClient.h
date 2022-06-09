#pragma once

#include "Banan/Networking/Client.h"

namespace Banan::Networking
{

	class WindowsClient : public Client
	{
	public:
		WindowsClient();
		~WindowsClient();

		virtual void Connect(const std::string& ip, int port, InternetLayer il = InternetLayer::IPv4) override;
		virtual void Disconnect() override;

		virtual bool IsConnected() const override;

		virtual void Send(const Message& message) override;

		virtual void SetMessageCallback(std::function<void(const Message&)> callback) override;
		virtual void SetConnectionCallback(std::function<void()> callback) override;
		virtual void SetDisconnectionCallback(std::function<void()> callback) override;

		virtual void QueryUpdates() override;
	};

}