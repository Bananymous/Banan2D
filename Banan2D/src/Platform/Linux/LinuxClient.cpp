#include "bgepch.h"
#include "LinuxClient.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace Banan::Networking
{

    LinuxClient::LinuxClient() :
		m_socket(-1),
		m_active(false),
		m_connected(false),
		m_disconnected(false)
	{}

	LinuxClient::~LinuxClient()
	{
		Disconnect();
	}

    void LinuxClient::Connect(const std::string& ip, int port, InternetLayer il)
	{
		int ret, family;
		addrinfo* res = NULL;
		addrinfo* ptr = NULL;
		addrinfo hints {};

		if (il == InternetLayer::IPv4)
			family = AF_INET;
		else if (il == InternetLayer::IPv6)
			family = AF_INET6;

		hints.ai_family = family;
		hints.ai_socktype = SOCK_STREAM;

		ret = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &res);
		BANAN_ASSERT(ret == 0, "getaddrinfo() (%s)\n", strerror(errno));

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
		{
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_socket == -1)
				continue;

			ret = connect(m_socket, ptr->ai_addr, ptr->ai_addrlen);
			if (ret != -1)
				break;

			close(m_socket);
		}

		freeaddrinfo(res);
		BANAN_ASSERT(ptr, "socket(), connect() (%s)\n", strerror(errno));

		m_active = true;
		m_recvThread = std::thread(&LinuxClient::RecvThread, this);
		m_sendThread = std::thread(&LinuxClient::SendThread, this);

		m_connected = true;
	}

    void LinuxClient::Disconnect()
	{
		m_active = false;

		shutdown(m_socket, SHUT_RD);
		if (m_recvThread.joinable())
			m_recvThread.join();

		m_sendMessages.Kill();
		if (m_sendThread.joinable())
			m_sendThread.join();

		close(m_socket);

		m_sendMessages.Clear();

		m_disconnected = true;
	}

    bool LinuxClient::IsConnected() const
	{
		return m_active;
	}

    void LinuxClient::Send(const Message& message)
	{
		m_sendMessages.Push(message);
	}

    void LinuxClient::QueryUpdates()
	{
		if (m_messageCallback)
		{
			Message message;
			while (m_recvMessages.Pop(message))
				m_messageCallback(message);
		} else m_recvMessages.Clear();

		if (m_connectionCallback && m_connected)
			m_connectionCallback();
		m_connected = false;
		
		if (m_disconnectionCallback && m_disconnected)
			m_disconnectionCallback();
		m_disconnected = false;
	}

    void LinuxClient::RecvThread()
	{
		char* buffer = new char[BANAN_MAX_MESSAGE_SIZE];
		uint64_t target = 0;
		uint64_t current = 0;

		while (m_active)
		{
			int bytes = recv(m_socket, buffer + current, BANAN_MAX_MESSAGE_SIZE - current, 0);
			if (bytes == -1 && m_active)
				BANAN_ERROR("recv() (%s)\n", strerror(errno));
			if (bytes <= 0 || !m_active)
			{
				m_active = false;
				break;
			}

			if (target == 0)
			{
				if (current < sizeof(uint64_t))
					continue;
				target = *(uint64_t*)buffer;

				if (target == 0 || target > BANAN_MAX_MESSAGE_SIZE)
				{
					BANAN_ERROR("server sent an invalid message\n");
					break;
				}
			}

			if (current >= target)
			{
				m_recvMessages.Push(Message::Create(buffer, target));

				std::memmove(buffer, buffer + target, current - target);
				current -= target;
				target = 0;
			}
		}

		delete[] buffer;
	}

    void LinuxClient::SendThread()
	{
		while (m_active)
		{
			Message message;
			if (m_sendMessages.Pop(message))
			{
				const char* serialized = message.GetSerialized();
				int size = message.Size();

				int sent = 0;
				while (sent < size)
				{
					int bytes = send(m_socket, serialized + sent, size - sent, 0);
					if (!m_active)
						return;
					if (bytes < 0)
						BANAN_ERROR("send() (%s)\n", strerror(errno));
					if (bytes <= 0)
					{
						m_active = false;
						return;
					}
				}
			}
		}
	}

}