#include "bgepch.h"
#include "LinuxServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#define BANAN_MAX_MESSAGE_LEN 1'000'000

namespace Banan::Networking
{

	LinuxServer::LinuxServer(int port, TransportLayer tl, InternetLayer il) :
		m_active(false),
		m_internetLayer(il)
	{
		int ret, domain = 0, type = 0, opt = 1;

		if (il == InternetLayer::IPv4)
			domain = AF_INET;
		else if (il == InternetLayer::IPv6)
			domain = AF_INET6;
		
		if (tl == TransportLayer::TCP)
			type = SOCK_STREAM;
		else if (tl == TransportLayer::UDP)
			type = SOCK_DGRAM;

		// Create socket
		m_socket = socket(domain, type, 0);
		BANAN_ASSERT(m_socket != -1, "Could not create socket for server (errno: %d)\n", errno);

		ret = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		if (ret == -1)
			BANAN_WARN("setsockopt() failed (errno: %d)\n", errno);

		if (il == InternetLayer::IPv4)
		{
			sockaddr_in hint;
			hint.sin_family = AF_INET;
			hint.sin_addr.s_addr = INADDR_ANY;
			hint.sin_port = htons(port);

			// Bind socket to address and port
			ret = bind(m_socket, (sockaddr*)&hint, sizeof(hint));
			BANAN_ASSERT(ret != -1, "Could not bind the socket (errno: %d)\n", errno);
		}
		else if (il == InternetLayer::IPv6)
		{
			sockaddr_in6 hint;
			hint.sin6_family = AF_INET6;
			hint.sin6_addr = in6addr_any;
			hint.sin6_port = htons(port);

			// Bind socket to address and port
			ret = bind(m_socket, (sockaddr*)&hint, sizeof(hint));
			BANAN_ASSERT(ret != -1, "Could not bind the socket (errno: %d)\n", errno);
		}

		// Set socket to listening mode
		ret = listen(m_socket, SOMAXCONN);
		BANAN_ASSERT(ret != -1, "Could not set socket to listening (errno: %d)\n", errno);

		FD_ZERO(&m_clients);
	}

	LinuxServer::~LinuxServer()
	{
		Stop();
		shutdown(m_socket, SHUT_RDWR);
	}

	void LinuxServer::Start()
	{
		m_active = true;
		m_recvThread = std::thread(&LinuxServer::RecvThread, this);
		m_sendThread = std::thread(&LinuxServer::SendThread, this);
	}

	void LinuxServer::Stop()
	{
		m_active = false;

		m_connections.Kill();
		m_connections.Clear();
		
		m_disconnections.Kill();
		m_disconnections.Clear();
		
		m_recievedMessages.Kill();
		m_recievedMessages.Clear();

		m_toSend.Clear();

		m_pendingMessages.clear();

		if (m_recvThread.joinable())
			m_recvThread.join();

		if (m_sendThread.joinable())
			m_sendThread.join();

		for (Socket sock : m_clientSet)
			if (FD_ISSET(sock, &m_clients))
				close(sock);
		m_clientSet.clear();
		FD_ZERO(&m_clients);
	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		m_toSend.Push({ socket, message });
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{
		for (Socket sock : m_clientSet)
			if (sock != skip)
				Send(message, sock);
	}

	void LinuxServer::QueryUpdates()
	{
		if (!m_recievedMessages.Empty())
		{
			if (m_messageCallback) {
				std::pair<Socket, Message> message;
				while (m_recievedMessages.Pop(message))
					m_messageCallback(message.first, message.second);
			}
			else m_recievedMessages.Clear();
		}

		if (m_connectionCallback) {
			Socket socket;
			while (m_connections.Pop(socket))
				m_connectionCallback(socket);
		}
		else m_connections.Clear();

		if (m_disconnectionCallback) {
			Socket socket;
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

	void LinuxServer::Kick(Socket socket)
	{
		FD_CLR(socket, &m_clients);
		m_clientSet.erase(socket);
		m_disconnections.Push(socket);
		close(socket);

		m_pendingMessages.erase(socket);
		
		std::scoped_lock lock(m_ipMutex);
		m_ipMap.erase(socket);
	}

	bool LinuxServer::HasData(Socket socket) const
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(socket, &fd);

		timeval tv {0, 0};

		return (bool)select(FD_SETSIZE, &fd, NULL, NULL, &tv);
	}

	template<typename T>
	std::pair<Socket, std::string> AcceptConnection(Socket listening)
	{
		T addr;
		socklen_t size = sizeof(addr);

		int client = accept(listening, (sockaddr*)&addr, &size);
		if (client == -1)
		{
			BANAN_WARN("Error on accept() (errno: %d)\n", errno);
			return { BANAN_INVALID_SOCKET, std::string() };
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

	void LinuxServer::SendThread()
	{
		while (m_active)
		{
			std::pair<Socket, Message> object;
			if (m_toSend.WaitAndPop(object))
			{
				auto& [sock, message] = object;

				uint8_t* serialized = (uint8_t*)message.GetSerialized();
				int size = message.Size();

				int sent = 0;
				while (sent < size)
				{
					int bytes = send(sock, serialized + sent, size - sent, 0);
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


	void LinuxServer::RecvThread()
	{
		while (m_active)
		{
			fd_set copy = m_clients;
			FD_SET(m_socket, &copy);
			
			int ret = select(FD_SETSIZE, &copy, NULL, NULL, NULL);
			if (ret == -1)
			{
				BANAN_WARN("Error on select() (errno: %d)\n", errno);
				continue;
			}

			// New Connection
			if (FD_ISSET(m_socket, &copy))
			{
				std::pair<Socket, std::string> object;
				if (m_internetLayer == InternetLayer::IPv4)
					object = AcceptConnection<sockaddr_in>(m_socket);
				else if (m_internetLayer == InternetLayer::IPv6)
					object = AcceptConnection<sockaddr_in6>(m_socket);
				
				if (object.first != BANAN_INVALID_SOCKET)
				{
					auto&[sock, ip] = object;

					m_ipMutex.lock();
					m_ipMap[sock] = ip;
					m_ipMutex.unlock();

					FD_SET(sock, &m_clients);
					m_clientSet.insert(sock);
					m_connections.Push(sock);
				}
			}

			for (Socket sock : m_clientSet)
			{
				// New messages
				if (FD_ISSET(sock, &copy))
				{
					auto it = m_pendingMessages.find(sock);

					if (it == m_pendingMessages.end())
					{
						Message::size_type size;
						int bytes = recv(sock, &size, sizeof(size), 0);
						if (bytes == -1)
							BANAN_WARN("Error on recv() (errno: %d)\n", errno);
						if (bytes <= 0)
						{
							Kick(sock);
							break;
						}

						if (size == 0)
						{
							BANAN_WARN("%lu attempted to send 0 bytes!\n", sock);
							break;
						}
						if (size >= BANAN_MAX_MESSAGE_LEN)
						{
							BANAN_WARN("%lu attempted to send %lu bytes!\n", sock, size);
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
							pending.data = (uint8_t*)buffer;
							break;	
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
							pending.data = (uint8_t*)buffer;
						}
					}
					else
					{
						PendingMessage& pending = it->second;

						int bytes = recv(sock, pending.data + pending.current_size, pending.total_size - pending.current_size, 0);
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

}