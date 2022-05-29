#include "bgepch.h"
#include "LinuxClient.h"

namespace Banan::Networking
{

    LinuxClient::LinuxClient(const std::string& ip, int port, TransportLayer tl, InternetLayer il) :
		m_active(false),
		m_connected(false),
		m_disconnected(false)
	{

	}

	LinuxClient::~LinuxClient()
	{
		Disconnect();
	}

    void LinuxClient::Connect()
	{
		m_active = true;
		m_recvThread = std::thread(&LinuxClient::RecvThread, this);
		m_sendThread = std::thread(&LinuxClient::SendThread, this);
	}

    void LinuxClient::Disconnect()
	{
		m_active = false;

		m_sendMessages.Kill();

		if (m_recvThread.joinable())
			m_recvThread.join();

		if (m_sendThread.joinable())
			m_sendThread.join();

		m_sendMessages.Clear();

		m_pendingMutex.lock();
		std::free(m_pendingMessage.data);
		m_pendingMessage.total_size = 0;
		m_pendingMessage.current_size = 0;
		m_pendingMessage.data = nullptr;
		m_pendingMutex.unlock();		
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

	}

    void LinuxClient::SendThread()
	{

	}

}