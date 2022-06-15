#include "bgepch.h"
#include "WindowsServer.h"

namespace Banan::Networking
{

	static std::string WSAGetErrorString()
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

	WindowsServer::WindowsServer(uint64_t thread_count) :
		m_active(false),
		m_listening(INVALID_SOCKET),
		m_threadPool(thread_count - 1)
	{
		WSAData wsaData;
		int res;

		BANAN_ASSERT(thread_count >= 2, "Banan::Networking::Server needs atleast 2 threads to work!\n");

		res = WSAStartup(MAKEWORD(2, 2), &wsaData);
		BANAN_ASSERT(res == 0, "WSAStartup() (%s)\n", WSAGetErrorString().c_str());
	}

	WindowsServer::~WindowsServer()
	{
		Stop();
		WSACleanup();
	}

	void WindowsServer::Start(int port, InternetLayer il)
	{
	}

	void WindowsServer::Stop()
	{
	}

	void WindowsServer::Send(Socket socket, const Message& message)
	{
		using namespace std::placeholders;
		m_threadPool.Push(std::bind(&WindowsServer::SendTask, this, _1, _2), socket, message);
	}

	void WindowsServer::SendAll(const Message& message, Socket skip)
	{
		
	}

	void WindowsServer::Kick(Socket socket)
	{

	}

	void WindowsServer::QueryUpdates()
	{
	}

	std::string WindowsServer::GetIP(Socket socket) const
	{
		if (m_socketIPs.HasKey(socket))
			return m_socketIPs.GetCopy(socket);
		return "<Socket not connected>";
	}

	void WindowsServer::SelectThread()
	{
		// TODO only send task to sockets that are not being processed
	}

	void WindowsServer::AcceptConnectionsTask()
	{
	}

	void WindowsServer::RecvTask(SOCKET socket)
	{
		uint64_t target = 0;
		uint64_t current = 0;
		std::vector<char> buffer;

		if (m_pendingMessages.HasKey(socket))
		{
			std::scoped_lock _(m_pendingMessages.GetMutex());
			PendingMessage& message = m_pendingMessages.GetRef(socket);
			target = message.target;
			current = message.current;
			buffer = message.buffer;
		}
		buffer.resize(BANAN_MAX_MESSAGE_SIZE);

		while (true)
		{
			// Recieve data
			int nbytes = recv(socket, buffer.data() + current, (int)(BANAN_MAX_MESSAGE_SIZE - current), 0);
			if (!m_active)
				return;
			if (nbytes == SOCKET_ERROR)
				BANAN_WARN("recv() (%s)\n", WSAGetErrorString().c_str());
			if (nbytes == SOCKET_ERROR || nbytes == 0)
				return Kick(socket);

			// Extract complete messages
			while (true)
			{
				if (current < sizeof(uint64_t))
					break;

				target = Message::GetSize(buffer.data());

				if (current < target)
					break;

				m_messagesReceived.Push({ socket, Message::Create(buffer.data(), target) });

				std::memmove(buffer.data(), buffer.data() + target, current - target);

				current -= target;
			}
		}

		// Save non complete message
		if (current > 0)
		{
			std::scoped_lock _(m_pendingMessages.GetMutex());
			PendingMessage& pending = m_pendingMessages.GetRef(socket);
			pending.current = current;
			pending.target = target;
			pending.buffer = std::vector<char>(buffer.begin(), buffer.begin() + current);
		}
	}

	void WindowsServer::SendTask(SOCKET socket, const Message& message)
	{
		uint64_t size = message.Size();
		uint64_t sent = 0;
		const char* serialized = message.GetSerialized();

		while (sent < size)
		{
			int nbytes = send(socket, serialized + sent, (int)(size - sent), 0);
			if (!m_active)
				return;
			if (nbytes == SOCKET_ERROR)
				BANAN_WARN("send() (%s)\n", WSAGetErrorString().c_str());
			if (nbytes == SOCKET_ERROR || nbytes == 0)
				return Kick(socket);

			sent += nbytes;
		}
	}

}