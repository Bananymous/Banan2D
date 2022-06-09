#pragma once

#include "Banan/Networking/Server.h"

#include <unordered_set>
#include <thread>

#include <poll.h>

namespace Banan::Networking
{

	// TODO confirm thread safety
	
	class LinuxServer : public Server
	{
	public:
		LinuxServer();
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
		void SendThread();
		void RecvThread();

		void Kick(int socket);

		bool HasData(int socket) const;

		bool AcceptConnection();

	private:
		std::function<void(Socket, const Message&)> 	m_messageCallback;
		std::function<void(Socket)>                 	m_connectionCallback;
		std::function<void(Socket)>                 	m_disconnectionCallback;

		std::atomic<bool>								m_active;
		
		int												m_listening;
		int												m_epfd;

		std::mutex										m_sfdMutex;
		std::vector<int>								m_sfds;

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
			uint64_t	target = 0;
			uint64_t	current = 0;
			char*		data = nullptr;
		};
		// no mutex needed since used only by send thread
		std::unordered_map<int, PendingMessage>			m_pendingMessages;
	};

}