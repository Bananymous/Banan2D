#include "bgepch.h"
#include "LinuxServer.h"

namespace Banan::Networking
{

	LinuxServer::LinuxServer(int port, TransportLayer tl, InternetLayer ip)
	{

	}

	LinuxServer::~LinuxServer()
	{

	}

	void LinuxServer::Start()
	{

	}

	void LinuxServer::Stop()
	{

	}

	void LinuxServer::Send(const Message& message, Socket socket)
	{
		m_toSend.Push({ socket, message });
	}

	void LinuxServer::SendAll(const Message& message, Socket skip)
	{

	}

	void LinuxServer::QueryUpdates()
	{
		if (m_messageCallback) {
			std::pair<Socket, Message> message;
			while (m_recievedMessages.Pop(message))
				m_messageCallback(message.first, message.second);
		}
		else m_recievedMessages.Clear();

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




	void LinuxServer::SendThread()
	{
		while (m_active)
		{
			std::pair<Socket, Message> message;
			if (m_toSend.WaitAndPop(message))
			{
				// Send message
			}
		}
	}


	void LinuxServer::RecvThread()
	{
		while (m_active)
		{
			
		}
	}

}