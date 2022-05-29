#include "bgepch.h"
#include "LinuxServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BANAN_MAX_MESSAGE_LEN 1'000'000

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
		m_listening = socket(domain, type, 0);
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

		m_sockets.push_back({ m_listening, POLLIN, 0 });

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

		for (auto& pfd : m_sockets)
			close(pfd.fd);
		m_sockets.clear();

		close(m_listening);
		m_listening = -1;
	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		m_toSend.Push({ socket, message });
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{
		for (uint64_t i = 1; i < m_sockets.size(); i++)
			if (m_sockets[i].fd != skip)
				Send(message, m_sockets[i].fd);
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
			return "<Socket not connected>";
		return it->second;
	}

	void LinuxServer::Kick(int socket)
	{
		for (auto it = m_sockets.begin(); it != m_sockets.end();)
		{
			if (it->fd == socket)
				it = m_sockets.erase(it);
			else it++;
		}
		close(socket);

		m_pendingMessages.erase(socket);
		
		m_ipMutex.lock();
		m_ipMap.erase(socket);
		m_ipMutex.unlock();

		m_disconnections.Push(socket);
	}

	bool LinuxServer::HasData(int socket) const
	{
		pollfd pfd;
		pfd.fd = socket;
		pfd.events = POLLIN;
		pfd.revents = 0;

		int ret = poll(&pfd, 1, 0);
		if (ret == -1)
		{
			BANAN_WARN("Error on poll() (errno: %d)\n", errno);
			return false;
		}

		return (bool)ret;
	}

	template<typename T>
	std::pair<int, std::string> AcceptConnection(int listening)
	{
		T addr;
		socklen_t size = sizeof(addr);

		int client = accept(listening, (sockaddr*)&addr, &size);
		if (client == -1)
		{
			BANAN_WARN("Error on accept() (errno: %d)\n", errno);
			return { -1, std::string() };
		}

		char host[NI_MAXHOST]{};
		char serv[NI_MAXSERV]{};

		int ret = getnameinfo((sockaddr*)&addr, size, host, NI_MAXHOST, serv, NI_MAXSERV, 0);
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
			int ret = poll(m_sockets.data(), m_sockets.size(), -1);
			if (ret == -1)
			{
				BANAN_WARN("Error on select() (errno: %d)\n", errno);
				continue;
			}

			// New Connection
			if (m_sockets[0].revents == POLLIN)
			{
				std::pair<int, std::string> object;
				if (m_internetLayer == InternetLayer::IPv4)
					object = AcceptConnection<sockaddr_in>(m_listening);
				else if (m_internetLayer == InternetLayer::IPv6)
					object = AcceptConnection<sockaddr_in6>(m_listening);
				
				if (object.first != -1)
				{
					auto&[sock, ip] = object;

					m_ipMutex.lock();
					m_ipMap[sock] = ip;
					m_ipMutex.unlock();

					m_sockets.push_back({ sock, POLLIN | POLLRDHUP, 0 });

					m_connections.Push(sock);
				}
			}

			for (uint64_t i = 1; i < m_sockets.size(); i++)
			{
				// Disconnection
				if (m_sockets[i].revents & POLLRDHUP)
				{
					Kick(m_sockets[i].fd);
					continue;
				}
				
				// New messages
				if (m_sockets[i].revents & POLLIN)
				{
					int sock = m_sockets[i].fd;

					auto it = m_pendingMessages.find(sock);
					if (it == m_pendingMessages.end())
					{
						// TODO handle messages with first recv less than sizeof(Message::size_type) bytes

						Message::size_type size;
						int bytes = recv(sock, &size, sizeof(size), 0);
						if (bytes == -1)
							BANAN_WARN("Error on recv() (errno: %d)\n", errno);
						if (bytes <= 0)
						{
							Kick(sock);
							break;
						}

						if (size == 0 || size >= BANAN_MAX_MESSAGE_LEN)
						{
							BANAN_WARN("%d attempted to send %lu bytes!\n", sock, size);
							Kick(sock);
							break;
						}
						
						void* buffer = std::malloc(size);
						BANAN_ASSERT(buffer, "Could not allocate memory\n");

						uint8_t* ptr = (uint8_t*)buffer;
						*(Message::size_type*)buffer = size;	ptr += sizeof(size);
						
						if (!HasData(sock))
						{
							PendingMessage& pending = m_pendingMessages[sock];
							pending.total_size = size;
							pending.current_size = sizeof(size);
							pending.data = buffer;
							continue;	
						}

						bytes = recv(sock, ptr, size - sizeof(size), 0);
						if (bytes == -1)
							BANAN_WARN("Error on recv() (errno: %d)\n", errno);
						if (bytes <= 0)
						{
							std::free(buffer);
							Kick(sock);
							break;
						}

						if (bytes + sizeof(size) == size)
						{
							m_recievedMessages.Push(std::make_pair(sock, Message::CreateMove(buffer)));
						}
						else
						{
							PendingMessage& pending = m_pendingMessages[sock];
							pending.total_size = size;
							pending.current_size = bytes + sizeof(size);
							pending.data = buffer;
						}
					}
					else
					{
						PendingMessage& pending = it->second;

						int bytes = recv(sock, (uint8_t*)pending.data + pending.current_size, pending.total_size - pending.current_size, 0);
						if (bytes == -1)
							BANAN_WARN("Error on recv() (errno: %d)\n", errno);
						if (bytes <= 0)
						{
							Kick(sock);
							break;
						}

						if (pending.current_size + bytes == pending.total_size)
						{
							m_recievedMessages.Push({ sock, Message::CreateMove(pending.data) });
							m_pendingMessages.erase(sock);
						}
						else
						{
							pending.current_size += bytes;
						}
					}
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