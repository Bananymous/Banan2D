#pragma once

#include "Banan/Networking/Server.h"

#include <unordered_set>
#include <thread>

#include <poll.h>

namespace Banan::Networking
{

	class LinuxServer : public Server
	{
	public:
		LinuxServer(int port, TransportLayer tl, InternetLayer il);
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

		virtual std::string GetIP(Socket socket) const override;

	private:
		void SendThread();
		void RecvThread();

		void Kick(int socket);

		bool HasData(int socket) const;

	private:
		std::function<void(Socket, const Message&)> 	m_messageCallback;
		std::function<void(Socket)>                 	m_connectionCallback;
		std::function<void(Socket)>                 	m_disconnectionCallback;

		std::atomic<bool>								m_active;
		
		int												m_listening;
		std::vector<pollfd>								m_sockets;

		// TODO thread pools
		std::thread										m_recvThread;
		std::thread										m_sendThread;

		ThreadSafeQueue<std::pair<int, Message>>		m_recievedMessages;
		ThreadSafeQueue<int>							m_connections;
		ThreadSafeQueue<int>							m_disconnections;

		ThreadSafeQueue<std::pair<int, Message>>		m_toSend;

		mutable std::mutex								m_ipMutex;
		std::unordered_map<int, std::string>			m_ipMap;

		struct PendingMessage
		{
			uint64_t	total_size;
			uint64_t	current_size;
			void*		data;
		};
		// no mutex needed since used only by send thread
		std::unordered_map<int, PendingMessage>			m_pendingMessages;

		const int										m_port;
		const TransportLayer							m_transportLayer;
		const InternetLayer								m_internetLayer;
	};

}