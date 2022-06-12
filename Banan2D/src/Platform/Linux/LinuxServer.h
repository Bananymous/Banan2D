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
		LinuxServer(uint64_t thread_count);
		~LinuxServer();

		virtual void Start(int port, InternetLayer il = InternetLayer::IPv4) override;
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
		void EpollThread();
		void RecvTask(int socket);
		void SendTask(int socket, const Message& message);

		void Kick(int socket);

		bool HasData(int socket) const;

		void AcceptConnections();

	private:
		std::function<void(Socket, const Message&)> 	m_messageCallback;
		std::function<void(Socket)>                 	m_connectionCallback;
		std::function<void(Socket)>                 	m_disconnectionCallback;

		std::atomic<bool>								m_active;
		
		std::atomic<int>								m_listening;
		int												m_epfd;

		std::mutex										m_sfdMutex;
		std::vector<int>								m_sfds;

		std::thread										m_epollThread;
		ThreadPool										m_threadPool;

		ThreadSafeQueue<std::pair<int, Message>>		m_recievedMessages;
		ThreadSafeQueue<int>							m_connections;
		ThreadSafeQueue<int>							m_disconnections;

		ThreadSafeQueue<std::pair<int, Message>>		m_toSend;

		mutable std::mutex								m_ipMutex;
		std::unordered_map<int, std::string>			m_ipMap;

		struct PendingMessage
		{
			uint64_t			target	= 0;
			uint64_t			current	= 0;
			std::vector<char>	buffer;
		};
		std::mutex										m_pendingMutex;
		std::unordered_map<int, PendingMessage>			m_pendingMessages;
	};

}