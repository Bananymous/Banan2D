#pragma once

#include "Banan/Networking/Server.h"

#include <thread>
#include <atomic>

namespace Banan::Networking
{

	class LinuxServer : public Server
	{
	public:
		LinuxServer(int port, TransportLayer tl, InternetLayer ip);
		~LinuxServer();

		virtual void Start() override;
		virtual void Stop() override;

		virtual bool IsActive() const override { return m_active; }

		virtual void Send(const Message& message, Socket socket) override;
		virtual void SendAll(const Message& message, Socket skip) override;

		virtual void SetMessageCallback(std::function<void(Socket, const Message&)> callback) override	{ m_messageCallback = callback; }
		virtual void SetConnectionCallback(std::function<void(Socket)> callback) override				{ m_connectionCallback = callback; }
		virtual void SetDisconnectionCallback(std::function<void(Socket)> callback) override			{ m_disconnectionCallback = callback; }

		virtual void QueryUpdates() override;

	private:
		void SendThread();
		void RecvThread();

	private:
		std::function<void(Socket, const Message&)> 	m_messageCallback;
		std::function<void(Socket)>                 	m_connectionCallback;
		std::function<void(Socket)>                 	m_disconnectionCallback;

		std::atomic<bool>								m_active;

		ThreadSafeQueue<std::pair<Socket, Message>>		m_recievedMessages;
		ThreadSafeQueue<Socket>							m_connections;
		ThreadSafeQueue<Socket>							m_disconnections;

		ThreadSafeQueue<std::pair<Socket, Message>>		m_toSend;
	};

}