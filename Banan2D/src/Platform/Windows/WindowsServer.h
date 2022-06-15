#pragma once

#include "Banan/Networking/Server.h"

#include "Banan/Threading/ThreadSafeContainers.h"
#include "Banan/Threading/ThreadPool.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace Banan::Networking
{

	class WindowsServer : public Server
	{
	public:
		WindowsServer(uint64_t thread_count);
		~WindowsServer();

		virtual void Start(int port, InternetLayer il = InternetLayer::IPv4) override;
		virtual void Stop() override;

		virtual bool IsActive() const override { return m_active; }

		virtual void Send(Socket socket, const Message& message) override;
		virtual void SendAll(const Message& message, Socket skip = ~Socket(0)) override;

		virtual void Kick(Socket socket) override;

		virtual void SetMessageCallback(std::function<void(Socket, const Message&)> callback) override	{ m_callbacks.message = callback; }
		virtual void SetConnectionCallback(std::function<void(Socket)> callback) override				{ m_callbacks.connection = callback; }
		virtual void SetDisconnectionCallback(std::function<void(Socket)> callback) override			{ m_callbacks.disconnection = callback; }

		virtual void QueryUpdates() override;

		virtual std::string GetIP(Socket socket) const override;

	private:
		void SelectThread();
		void AcceptConnectionsTask();
		void RecvTask(SOCKET socket);
		void SendTask(SOCKET socket, const Message& message);

	private:
		struct Callbacks
		{
			std::function<void(Socket, const Message&)>	message;
			std::function<void(Socket)>					connection;
			std::function<void(Socket)>					disconnection;
		};
		Callbacks											m_callbacks;

		std::atomic<bool>									m_active;

		std::atomic<uint64_t>								m_listening;

		Threading::TSUnorderedSet<SOCKET>					m_clients;

		Threading::ThreadPool								m_threadPool;

		Threading::TSQueue<std::pair<SOCKET, Message>>		m_messagesToSend;
		Threading::TSQueue<std::pair<SOCKET, Message>>		m_messagesReceived;
		Threading::TSQueue<SOCKET>							m_connected;
		Threading::TSQueue<SOCKET>							m_disconnected;

		Threading::TSUnorderedMap<SOCKET, std::string>		m_socketIPs;

		struct PendingMessage
		{
			uint64_t target		= 0;
			uint64_t current	= 0;
			std::vector<char> buffer;
		};
		Threading::TSUnorderedMap<SOCKET, PendingMessage>	m_pendingMessages;
	};

}