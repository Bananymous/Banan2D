#pragma once

#include "Banan/Networking/Client.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace Banan::Networking
{

	class WindowsClient : public Client
	{
	public:
		WindowsClient();
		~WindowsClient();

		virtual void Connect(const std::string& ip, int port, InternetLayer il = InternetLayer::IPv4) override;
		virtual void Disconnect() override;

		virtual bool IsConnected() const override { return m_active; }

		virtual void Send(const Message& message) override;

		virtual void SetMessageCallback(std::function<void(const Message&)> callback) override	{ m_messageCallback = callback; }
		virtual void SetConnectionCallback(std::function<void()> callback) override				{ m_connectionCallback = callback; }
		virtual void SetDisconnectionCallback(std::function<void()> callback) override			{ m_disconnectionCallback = callback; }

		virtual void QueryUpdates() override;

	private:
		void DisconnectNonBlock();

		void RecvThread();
		void SendThread();

	private:
		std::function<void(const Message&)> m_messageCallback;
		std::function<void()>				m_connectionCallback;
		std::function<void()>				m_disconnectionCallback;

		std::atomic<bool>					m_active;
		SOCKET								m_socket;

		std::thread							m_recvThread;
		std::thread							m_sendThread;

		ThreadSafeQueue<Message>			m_messagesToSend;
		ThreadSafeQueue<Message>			m_messagesReceived;

		std::atomic<bool>					m_connected;
		std::atomic<bool>					m_disconncted;
	};

}