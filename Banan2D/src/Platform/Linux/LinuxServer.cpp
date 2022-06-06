#include "bgepch.h"
#include "LinuxServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BANAN_MAX_EVENTS 100

epoll_event g_events[BANAN_MAX_EVENTS];

namespace Banan::Networking
{

	LinuxServer::LinuxServer(int port, TransportLayer tl, InternetLayer il) :
		m_active(false),
		m_listening(-1),
		m_port(port),
		m_transportLayer(tl),
		m_internetLayer(il)
	{}

	LinuxServer::~LinuxServer()
	{
		Stop();
	}

	void LinuxServer::Start()
	{
		int ret, domain = 0, type = 0, opt = 1;

		if (m_internetLayer == InternetLayer::IPv4)
			domain = AF_INET;
		else if (m_internetLayer == InternetLayer::IPv6)
			domain = AF_INET6;
		
		if (m_transportLayer == TransportLayer::TCP)
			type = SOCK_STREAM;
		else if (m_transportLayer == TransportLayer::UDP)
			type = SOCK_DGRAM;

		// Create socket
		m_listening = socket(domain, type, SOCK_NONBLOCK);
		BANAN_ASSERT(m_listening != -1, "Could not create socket for server (errno: %d)\n", errno);

		ret = setsockopt(m_listening, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		if (ret == -1)
			BANAN_WARN("setsockopt() failed (errno: %d)\n", errno);

		if (m_internetLayer == InternetLayer::IPv4)
		{
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_addr.s_addr = INADDR_ANY;
			hint.sin_port = htons(m_port);

			// Bind socket to address and port
			ret = bind(m_listening, (sockaddr*)&hint, sizeof(hint));
			BANAN_ASSERT(ret != -1, "Could not bind the socket (errno: %d)\n", errno);
		}
		else if (m_internetLayer == InternetLayer::IPv6)
		{
			sockaddr_in6 hint;
			hint.sin6_family = AF_INET6;
			hint.sin6_addr = in6addr_any;
			hint.sin6_port = htons(m_port);

			// Bind socket to address and port
			ret = bind(m_listening, (sockaddr*)&hint, sizeof(hint));
			BANAN_ASSERT(ret != -1, "Could not bind the socket (errno: %d)\n", errno);
		}

		// Set socket to listening mode
		ret = listen(m_listening, SOMAXCONN);
		BANAN_ASSERT(ret != -1, "Could not set socket to listening (errno: %d)\n", errno);

		m_efd = epoll_create1(0);
		BANAN_ASSERT(m_efd != -1, "Error epoll_create1()\n");

		epoll_event event{};
		event.data.fd = m_listening;
		event.events = EPOLLET | EPOLLIN;

		ret = epoll_ctl(m_efd, EPOLL_CTL_ADD, m_listening, &event);
		BANAN_ASSERT("epoll_ctl() (%d)\n", errno);

		m_active = true;
		m_recvThread = std::thread(&LinuxServer::RecvThread, this);
		m_sendThread = std::thread(&LinuxServer::SendThread, this);
	}

	void LinuxServer::Stop()
	{
		m_active = false;

		m_connections.Kill();
		m_disconnections.Kill();
		m_recievedMessages.Kill();
		m_toSend.Kill();

		shutdown(m_listening, SHUT_RDWR);

		if (m_recvThread.joinable())
			m_recvThread.join();

		if (m_sendThread.joinable())
			m_sendThread.join();

		m_connections.Clear();
		m_disconnections.Clear();
		m_recievedMessages.Clear();

		for (auto& [_, msg] : m_pendingMessages)
			std::free(msg.data);
		m_pendingMessages.clear();

		for (auto& sfd : m_sfds)
			close(sfd);
		m_sfds.clear();

		close(m_listening);
		m_listening = -1;
	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		m_toSend.Push({ (int)socket, message });
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{
		for (int sfd : m_sfds)
			if (sfd != skip)
				Send(message, sfd);
	}

	void LinuxServer::QueryUpdates()
	{
		if (!m_recievedMessages.Empty())
		{
			if (m_messageCallback) {
				std::pair<int, Message> message;
				while (m_recievedMessages.Pop(message))
					m_messageCallback(message.first, message.second);
			}
			else m_recievedMessages.Clear();
		}

		if (m_connectionCallback) {
			int socket;
			while (m_connections.Pop(socket))
				m_connectionCallback(socket);
		}
		else m_connections.Clear();

		if (m_disconnectionCallback) {
			int socket;
			while (m_disconnections.Pop(socket))
				m_disconnectionCallback(socket);
		}
		else m_disconnections.Clear();
	}

	std::string LinuxServer::GetIP(Socket socket) const
	{
		std::scoped_lock lock(m_ipMutex);
		auto it = m_ipMap.find(socket);
		if (it == m_ipMap.end())
			return "<Not Connected>";
		return it->second;
	}

	void LinuxServer::Kick(int socket)
	{
		m_sfds.erase(std::find(m_sfds.begin(), m_sfds.end(), (int)socket));

		epoll_ctl(m_efd, EPOLL_CTL_DEL, socket, NULL);

		close(socket);

		m_pendingMessages.erase(socket);
		
		m_ipMutex.lock();
		m_ipMap.erase(socket);
		m_ipMutex.unlock();

		m_disconnections.Push(socket);
	}

	std::pair<int, std::string> LinuxServer::AcceptConnection()
	{
		sockaddr addr;
		socklen_t size = sizeof(addr);

		int client = accept4(m_listening, &addr, &size, SOCK_NONBLOCK);
		if (client == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				BANAN_WARN("accept() (%d)\n", errno);
			return { -1, std::string() };
		}

		char host[NI_MAXHOST]{};
		char serv[NI_MAXSERV]{};

		int ret = getnameinfo(&addr, size, host, NI_MAXHOST, serv, NI_MAXSERV, 0);
		if (ret != 0)
		{
			std::sprintf(host, "?.?.?.?");
			std::sprintf(serv, "?");
		}

		std::string ip = std::string(host) + ':' + std::string(serv);
		return { client, ip };
	}

	void LinuxServer::RecvThread()
	{
		while (m_active)
		{
			int nfds = epoll_wait(m_efd, g_events, BANAN_MAX_EVENTS, -1);
			if (nfds == -1)
			{
				BANAN_WARN("epoll_wait() (%d)\n", errno);
				continue;
			}

			for (int i = 0; i < nfds; i++)
			{
				epoll_event& e = g_events[i];

				// Check for errors
				if (e.events & EPOLLERR)
				{
					BANAN_WARN("EPOLLERR\n");
					continue;
				}

				// Accept new connections
				if (e.data.fd == m_listening && e.events)
				{
					std::pair<int, std::string> connectionInfo = AcceptConnection();
					while (connectionInfo.first != -1)
					{
						m_ipMutex.lock();
						m_ipMap.insert(connectionInfo);
						m_ipMutex.unlock();

						epoll_event event{};
						event.data.fd = m_listening;
						event.events = 	EPOLLET |	// Edge triggered mode
										EPOLLIN |	// Read events
										EPOLLRDHUP;	// Disconnection events

						int ret = epoll_ctl(m_efd, EPOLL_CTL_ADD, m_listening, &event);
						if (ret == -1)
							BANAN_PRINT("epoll_ctl() (%d)\n", errno);

						connectionInfo = AcceptConnection();
					}
				}
				else if (e.events & (EPOLLRDHUP | EPOLLHUP))
				{
					Kick(e.data.fd);
				}
				else if (e.events & EPOLLIN)
				{
					char* buffer = new char[BANAN_MAX_MESSAGE_SIZE];
					uint64_t target = -1;
					uint64_t current = 0;

					auto it = m_pendingMessages.find(e.data.fd);
					if (it != m_pendingMessages.end())
					{
						PendingMessage& msg = it->second;
						memcpy(buffer, msg.data, msg.current);
						target = msg.target;
						current = msg.current;
					}

					bool error = false;

					while (true)
					{
						int nbytes = recv(e.data.fd, buffer + current, BANAN_MAX_MESSAGE_SIZE - current, 0);
						if (nbytes == -1)
						{
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								BANAN_WARN("recv() (%d)\n", errno);
								error = true;
							}
							break;
						}

						current += nbytes;

						if (target == 0)
						{
							if (current < sizeof(uint64_t))
								continue;
							target = *(uint64_t*)buffer;

							if (target == 0 || target > BANAN_MAX_MESSAGE_SIZE)
							{
								BANAN_WARN("%d sent an invalid message\n", e.data.fd);
								error = true;
								break;
							}
						}
						
						if (current >= target)
						{
							m_recievedMessages.Push({ (int)e.data.fd, Message::Create(buffer, target) });

							std::memmove(buffer, buffer + target, current - target);
							current -= target;
							target = 0;
						}
					}

					if (error) 
					{
						Kick(e.data.fd);
					}
					else if (current > 0)
					{
						PendingMessage pending;
						pending.current = current;
						pending.target = target;
						pending.data = new char[target];
						std::memcpy(pending.data, buffer, current);
					}
					
					delete[] buffer;
				}
			}
		}
	}

	void LinuxServer::SendThread()
	{
		while (m_active)
		{
			std::pair<int, Message> object;
			if (m_toSend.WaitAndPop(object))
			{
				auto& [sock, message] = object;

				uint8_t* serialized = (uint8_t*)message.GetSerialized();
				int size = message.Size();

				int sent = 0;
				while (sent < size)
				{
					int bytes = send(sock, serialized + sent, size - sent, 0);
					if (!m_active)
						return;
					if (bytes < 0)
						BANAN_WARN("Error on send() (errno: %d)\n", errno);
					if (bytes <= 0)
					{
						Kick(sock);
						break;
					}
				}
			}
		}
	}

}