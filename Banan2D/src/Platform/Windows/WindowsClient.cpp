#include "bgepch.h"
#include "WindowsClient.h"

namespace Banan::Networking
{

	static std::string WSAErrorString()
	{
		std::string str;
		char* buffer = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, WSAGetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buffer, 0, NULL);
		str = buffer;
		LocalFree(buffer);
		return str;
	}

	WindowsClient::WindowsClient() :
		m_active(false),
		m_socket(INVALID_SOCKET),
		m_connected(false),
		m_disconncted(false)
	{
		WSADATA wsaData;
		int res;

		res = WSAStartup(MAKEWORD(2, 2), &wsaData);
		BANAN_ASSERT(res == 0, "WSAStartup() (%s)\n", WSAErrorString().c_str());
	}

	WindowsClient::~WindowsClient()
	{
		Disconnect();
		WSACleanup();
	}

	void WindowsClient::Connect(const std::string& ip, int port, InternetLayer il)
	{
		int ret, family;
		addrinfo* res = NULL;
		addrinfo* ptr = NULL;
		addrinfo hint {};

		switch (il)
		{
			case Banan::Networking::InternetLayer::IPv4: family = AF_INET;	break;
			case Banan::Networking::InternetLayer::IPv6: family = AF_INET6;	break;
			default: BANAN_ASSERT(false, "unknown InternetLayer\n");
		}

		hint.ai_family = family;
		hint.ai_socktype = SOCK_STREAM;

		ret = getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hint, &res);
		BANAN_ASSERT(ret == 0, "getaddrinfo() (%s)\n", WSAErrorString().c_str());

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
		{
			m_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
			if (m_socket == INVALID_SOCKET)
				continue;

			ret = connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (ret == 0)
				break;

			closesocket(m_socket);
		}

		freeaddrinfo(res);
		BANAN_ASSERT(ptr != NULL, "socket(), connect() (%s)\n", WSAErrorString().c_str());

		m_active = true;
		m_recvThread = std::thread(&WindowsClient::RecvThread, this);
		m_sendThread = std::thread(&WindowsClient::SendThread, this);

		m_connected = true;
	}

	void WindowsClient::DisconnectNonBlock()
	{
		m_active = false;

		shutdown(m_socket, SD_RECEIVE);
		m_messagesToSend.Kill();

		m_messagesReceived.Clear();
		m_messagesToSend.Clear();

		m_disconncted = true;
	}

	void WindowsClient::Disconnect()
	{
		DisconnectNonBlock();

		if (m_recvThread.joinable())
			m_recvThread.join();

		if (m_sendThread.joinable())
			m_sendThread.join();

		closesocket(m_socket);
	}

	void WindowsClient::Send(const Message& message)
	{
		if (m_active)
			m_messagesToSend.Push(message);
	}

	void WindowsClient::QueryUpdates()
	{
		if (m_messageCallback) {
			Message message;
			while (m_messagesReceived.Pop(message))
				m_messageCallback(message);
		} else m_messagesReceived.Clear();

		if (m_connectionCallback && m_connected)
			m_connectionCallback();
		m_connected = false;
		
		if (m_disconnectionCallback && m_disconncted)
			m_disconnectionCallback();
		m_disconncted = false;
	}

	void WindowsClient::RecvThread()
	{
		uint64_t target = 0;
		uint64_t current = 0;
		std::vector<char> buffer(BANAN_MAX_MESSAGE_SIZE);

		while (m_active)
		{
			// Recieve data and handle errors
			int nbytes = recv(m_socket, buffer.data() + current, (int)(BANAN_MAX_MESSAGE_SIZE - current), 0);
			if (!m_active)
				return;
			if (nbytes == SOCKET_ERROR)
				BANAN_WARN("recv() (%s)\n", WSAErrorString().c_str());
			if (nbytes == SOCKET_ERROR || nbytes == 0)
				return DisconnectNonBlock();

			current += nbytes;

			// Extract complete messages
			while (true)
			{
				if (current < sizeof(uint64_t))
					break;

				target = Message::GetSize(buffer.data());

				if (target == 0 || target > BANAN_MAX_MESSAGE_SIZE)
				{
					BANAN_ERROR("server sent an invalid message\n");
					DisconnectNonBlock();
					return;
				}

				if (current < target)
					break;

				m_messagesReceived.Push(Message::Create(buffer.data(), target));

				std::memmove(buffer.data(), buffer.data() + target, current - target);
				current -= target;
			}
		}
	}

	void WindowsClient::SendThread()
	{
		while (m_active)
		{
			Message message;
			if (m_messagesToSend.WaitAndPop(message) && m_active)
			{
				const char* serialized = message.GetSerialized();
				const uint64_t size = message.Size();
				uint64_t sent = 0;

				while (sent < size)
				{
					int nbytes = send(m_socket, serialized + sent, (int)(size - sent), 0);
					if (!m_active)
						return;
					if (nbytes == SOCKET_ERROR)
						BANAN_ERROR("send() (%s)\n", WSAErrorString().c_str());
					if (nbytes == SOCKET_ERROR || nbytes == 0)
						return DisconnectNonBlock();

					sent += nbytes;
				}
			}
		}
	}

}