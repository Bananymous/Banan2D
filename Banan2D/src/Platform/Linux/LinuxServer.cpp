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

	LinuxServer::LinuxServer(uint64_t thread_count) :
		m_active(false),
		m_listening(-1),
		m_threadPool(thread_count)
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

		switch (il)
		{
			case InternetLayer::IPv4: family = AF_INET;  break;
			case InternetLayer::IPv6: family = AF_INET6; break;
		}
		
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

			ret = ::bind(m_listening, ptr->ai_addr, ptr->ai_addrlen);
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
		m_epollThread = std::thread(&LinuxServer::EpollThread, this);
	}

	void LinuxServer::Stop()
	{
		m_active = false;

		// Shutdown incoming messages/connections.
		// epoll will be notified
		shutdown(m_listening, SHUT_RD);

		if (m_epollThread.joinable())
			m_epollThread.join();

		m_threadPool.Wait(true);

		for (auto& sfd : m_sfds)
			close(sfd);
		close(m_listening);

		m_sfds.clear();
		m_toSend.Clear();
		m_connections.Clear();
		m_disconnections.Clear();
		m_recievedMessages.Clear();

		m_pendingMessages.clear();
	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		using namespace std::placeholders;
		m_threadPool.Push(std::bind(&LinuxServer::SendTask, this, _1, _2), socket, message);
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{
		std::scoped_lock _(m_sfdMutex);
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
		std::scoped_lock _(m_ipMutex);
		auto it = m_ipMap.find(socket);
		if (it == m_ipMap.end())
			return "<Not Connected>";
		return it->second;
	}

	void LinuxServer::Kick(int socket)
	{
		{
			std::scoped_lock _(m_sfdMutex);
			auto it = std::find(m_sfds.begin(), m_sfds.end(), (int)socket);
			if (it == m_sfds.end())
				return;
			m_sfds.erase(it);
		}

		epoll_ctl(m_epfd, EPOLL_CTL_DEL, socket, NULL);
		close(socket);

		m_pendingMessages.erase(socket);
		
		{
			std::scoped_lock _(m_ipMutex);
			m_ipMap.erase(socket);
		}

		m_disconnections.Push(socket);
	}

	void LinuxServer::AcceptConnections()
	{
		sockaddr addr;
		socklen_t size = sizeof(addr);

		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		while (true)
		{
			int client = accept4(m_listening, &addr, &size, SOCK_NONBLOCK);
			if (client == -1)
			{
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					BANAN_WARN("accept() (%s)\n", strerror(errno));
				break;
			}

			std::memset(host, 0, NI_MAXHOST);
			std::memset(serv, 0, NI_MAXSERV);

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
			epoll_ctl(m_epfd, EPOLL_CTL_ADD, client, &event);

			{
				std::scoped_lock _(m_sfdMutex);
				m_sfds.push_back(client);
			}
			{
				std::scoped_lock _(m_ipMutex);
				m_ipMap[client] = std::string(host) + ':' + std::string(serv);
			}

			m_connections.Push(client);
		}
	}

	void LinuxServer::EpollThread()
	{
		while (m_active)
		{
			int nfds = epoll_wait(m_epfd, g_events, BANAN_MAX_EVENTS, -1);
			if (!m_active)
				return;
			if (nfds == -1)
			{
				BANAN_WARN("epoll_wait() (%s)\n", strerror(errno));
				continue;
			}

			for (int i = 0; i < nfds; i++)
			{
				epoll_event& e = g_events[i];

				if (e.events & EPOLLERR)
				{
					BANAN_WARN("EPOLLERR\n");
					continue;
				}

				using namespace std::placeholders;

				int socket = e.data.fd;
				
				if ((socket == m_listening) && (e.events & EPOLLIN))
					m_threadPool.Push(std::bind(&LinuxServer::AcceptConnections, this));
				else if (e.events & (EPOLLRDHUP | EPOLLHUP))
					m_threadPool.Push(std::bind(&LinuxServer::Kick, this, _1), socket);
				else if (e.events & EPOLLIN)
					m_threadPool.Push(std::bind(&LinuxServer::RecvTask, this, _1), socket);
			}
		}
	}

	void LinuxServer::RecvTask(int socket)
	{
		uint64_t target = 0;
		uint64_t current = 0;
		std::vector<char> buffer;

		{
			std::scoped_lock _(m_pendingMutex);
			auto it = m_pendingMessages.find(socket);
			if (it != m_pendingMessages.end())
			{
				target	= it->second.target;
				current	= it->second.current;
				buffer	= it->second.buffer;
			}
		}

		buffer.resize(BANAN_MAX_MESSAGE_SIZE);

		while (true)
		{
			// Recieve data from socket
			int nbytes = recv(socket, buffer.data() + current, BANAN_MAX_MESSAGE_SIZE - current, 0);
			if (!m_active)
				return;
			if (nbytes == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					break;
				BANAN_WARN("recv() (%s)\n", strerror(errno));
			}
			if (nbytes <= 0)
			{
				Kick(socket);
				return;
			}

			current += nbytes;

			// Update target if not set
			if (target == 0)
			{
				if (current < sizeof(uint64_t))
					continue;
				
				target = Message::GetSize(buffer.data());

				if (target == 0 || target > BANAN_MAX_MESSAGE_SIZE)
				{
					BANAN_WARN("%d tried to send %lu bytes\n", socket, target);
					Kick(socket);
					return;
				}
			}

			// Process ready messages
			while (current >= target)
			{
				m_recievedMessages.Push({ socket, Message::Create(buffer.data(), target) });

				std::memmove(buffer.data(), buffer.data() + target, current - target);

				current -= target;
				target = 0;

				if (current < sizeof(uint64_t))
					break;

				target = Message::GetSize(buffer.data());

				if (target == 0 || target > BANAN_MAX_MESSAGE_SIZE)
				{
					BANAN_WARN("%d tried to send %lu bytes\n", socket, target);
					Kick(socket);
					return;
				}
			}
		}

		// Save pending message
		if (current > 0)
		{
			std::scoped_lock _(m_pendingMutex);

			PendingMessage& msg = m_pendingMessages[socket];
			msg.target = target;
			msg.current = current;
			msg.buffer.resize(current);
			std::memcpy(msg.buffer.data(), buffer.data(), current);
		}
	}

	void LinuxServer::SendTask(int socket, const Message& message)
	{
		uint64_t size = message.Size();
		uint64_t sent = 0;

		const char* buffer = message.GetSerialized();

		while (sent < size)
		{
			int nbytes = send(socket, buffer + sent, size - sent, 0);
			if (!m_active)
				return;
			// TODO: EAGAIN and EWOULDBLOCK should be handeled seperately
			if (nbytes == -1)
				BANAN_WARN("send() (%s)\n", strerror(errno));
			if (nbytes <= 0)
			{
				Kick(socket);
				return;
			}
			sent += nbytes;
		}
	}

}