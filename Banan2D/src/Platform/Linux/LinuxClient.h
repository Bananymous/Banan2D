#pragma once

#include "Banan/Networking/Client.h"

namespace Banan::Networking
{

	class LinuxClient : public Client
	{
	public:
		LinuxClient();
		~LinuxClient();

		virtual void Connect(const std::string& ip, int port, InternetLayer il) override;
		virtual void Disconnect() override;

		virtual bool IsConnected() const override;

		virtual void Send(const Message& message) override;

		virtual void SetMessageCallback(std::function<void(const Message&)> callback) override	{ m_messageCallback = callback; }
		virtual void SetConnectionCallback(std::function<void()> callback) override				{ m_connectionCallback = callback; }
		virtual void SetDisconnectionCallback(std::function<void()> callback) override			{ m_disconnectionCallback = callback; }

		virtual void QueryUpdates() override;

	private:
		void RecvThread();
		void SendThread();

	private:
		std::function<void(const Message&)>	m_messageCallback;
		std::function<void()>				m_connectionCallback;
		std::function<void()>				m_disconnectionCallback;

		int									m_socket;

		std::atomic<bool>					m_active;

		std::thread							m_recvThread;
		std::thread							m_sendThread;

		ThreadSafeQueue<Message>			m_recvMessages;
		ThreadSafeQueue<Message>			m_sendMessages;

		std::atomic<bool>					m_connected;
		std::atomic<bool>					m_disconnected;
	};

}