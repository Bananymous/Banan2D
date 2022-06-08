#include "bgepch.h"
#include "LinuxServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netdb.h>

#define BANAN_MAX_EVENTS 100

epoll_event g_events[BANAN_MAX_EVENTS];

namespace Banan::Networking
{

	LinuxServer::LinuxServer() :
		m_active(false),
		m_listening(-1)
	{}

	LinuxServer::~LinuxServer()
	{
		Stop();
	}

	void LinuxServer::Start(int port, InternetLayer il)
	{
		int ret, family;
		addrinfo* res = NULL;
		addrinfo* ptr = NULL;
		addrinfo hints{};

		if (il == InternetLayer::IPv4)
			family = AF_INET;
		else if (il == InternetLayer::IPv6)
			family = AF_INET6;
		
		hints.ai_family = family;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		ret = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &res);
		BANAN_ASSERT(ret == 0, "getaddrinfo() (%s)\n", strerror(errno));

		for (ptr = res; ptr != NULL; ptr->ai_next)
		{
			m_listening = socket(ptr->ai_family, ptr->ai_socktype | SOCK_NONBLOCK, 0);
			if (m_listening == -1)
				continue;

			int opt = 1;
			setsockopt(m_listening, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

			ret = bind(m_listening, ptr->ai_addr, ptr->ai_addrlen);
			if (ret == 0)
				break;
			
			close(m_listening);
		}

		freeaddrinfo(res);
		BANAN_ASSERT(ptr, "socket(), bind() (%s)\n", strerror(errno));

		ret = listen(m_listening, SOMAXCONN);
		BANAN_ASSERT(ret != -1, "listen() (%s)\n", strerror(errno));

		m_epfd = epoll_create1(0);
		BANAN_ASSERT(m_epfd != -1, "epoll_create1() (%s)\n", strerror(errno));

		epoll_event event{};
		event.data.fd = m_listening;
		event.events = EPOLLET | EPOLLIN;

		ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_listening, &event);
		BANAN_ASSERT(ret != -1, "epoll_ctl() (%s)\n", strerror(errno));

		m_active = true;
		m_recvThread = std::thread(&LinuxServer::RecvThread, this);
		m_sendThread = std::thread(&LinuxServer::SendThread, this);
	}

	void LinuxServer::Stop()
	{
		m_active = false;

		shutdown(m_listening, SHUT_RD);
		if (m_recvThread.joinable())
			m_recvThread.join();

		m_toSend.Kill();
		if (m_sendThread.joinable())
			m_sendThread.join();

		for (auto& sfd : m_sfds)
			close(sfd);
		close(m_listening);


		m_sfds.clear();
		m_toSend.Clear();
		m_connections.Clear();
		m_disconnections.Clear();
		m_recievedMessages.Clear();

		for (auto& [_, msg] : m_pendingMessages)
			delete[] msg.data;
		m_pendingMessages.clear();
	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		m_toSend.Push({ (int)socket, message });
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{
		m_sfdMutex.lock();
		for (int sfd : m_sfds)
			if (sfd != skip)
				Send(message, sfd);
		m_sfdMutex.unlock();
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
		m_sfdMutex.lock();
		auto it = std::find(m_sfds.begin(), m_sfds.end(), (int)socket);
		if (it == m_sfds.end())
			return;
		m_sfds.erase(it);
		m_sfdMutex.unlock();

		close(socket); // closing should remove fd from epoll

		m_pendingMessages.erase(socket);
		
		m_ipMutex.lock();
		m_ipMap.erase(socket);
		m_ipMutex.unlock();

		m_disconnections.Push(socket);
	}

	bool LinuxServer::AcceptConnection()
	{
		sockaddr addr;
		socklen_t size = sizeof(addr);

		int client = accept4(m_listening, &addr, &size, SOCK_NONBLOCK);
		if (client == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return false;
			BANAN_WARN("accept() (%s)\n", strerror(errno));
			return true;
		}

		char host[NI_MAXHOST]{};
		char serv[NI_MAXSERV]{};

		int ret = getnameinfo(&addr, size, host, NI_MAXHOST, serv, NI_MAXSERV, 0);
		if (ret != 0)
		{
			std::sprintf(host, "?.?.?.?");
			std::sprintf(serv, "?");
		}

		epoll_event event{};
		event.data.fd = client;
		event.events  =	EPOLLET |	// Edge triggered mode
						EPOLLIN |	// Read events
						EPOLLRDHUP;	// Disconnection events

		ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, client, &event);
		if (ret == -1)
		{
			BANAN_WARN("epoll_ctl() (%s)\n", strerror(errno));
			close(client);
			return true;
		}

		m_sfdMutex.lock();
		m_sfds.push_back(client);
		m_sfdMutex.unlock();

		m_ipMutex.lock();
		m_ipMap[client] = std::string(host) + ':' + std::string(serv);
		m_ipMutex.unlock();

		m_connections.Push(client);

		return true;
	}

	void LinuxServer::RecvThread()
	{
		while (m_active)
		{
			int nfds = epoll_wait(m_epfd, g_events, BANAN_MAX_EVENTS, -1);
			if (nfds == -1)
			{
				BANAN_WARN("epoll_wait() (%s)\n", strerror(errno));
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

				if ((e.data.fd == m_listening) && (e.events & EPOLLIN))
				{
					// Accept new connections

					while (AcceptConnection());
				}
				else if (e.events & (EPOLLRDHUP | EPOLLHUP))
				{
					// Disconnection

					Kick(e.data.fd);
				}
				else if (e.events & EPOLLIN)
				{
					// New data available on socket

					char* buffer = new char[BANAN_MAX_MESSAGE_SIZE];
					uint64_t target = 0;
					uint64_t current = 0;

					auto it = m_pendingMessages.find(e.data.fd);
					if (it != m_pendingMessages.end())
					{
						PendingMessage& msg = it->second;
						std::memcpy(buffer, msg.data, msg.current);
						target = msg.target;
						current = msg.current;
					}

					bool error = false;

					while (true)
					{
						int nbytes = recv(e.data.fd, buffer + current, BANAN_MAX_MESSAGE_SIZE - current, 0);
						if (!m_active)
						{
							delete[] buffer;
							return;
						}
						if (nbytes == -1)
						{
							if (errno != EAGAIN && errno != EWOULDBLOCK)
							{
								BANAN_WARN("recv() (%s)\n", strerror(errno));
								error = true;
							}
							break;
						}
						if (nbytes == 0)
							continue;

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

				const char* serialized = message.GetSerialized();
				int size = message.Size();

				int sent = 0;
				while (sent < size)
				{
					int bytes = send(sock, serialized + sent, size - sent, 0);
					if (!m_active)
						return;
					if (bytes < 0)
						BANAN_WARN("send() (%s)\n", strerror(errno));
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