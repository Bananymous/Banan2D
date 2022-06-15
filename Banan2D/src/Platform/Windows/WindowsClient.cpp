#include "bgepch.h"
#include "WindowsClient.h"

namespace Banan::Networking
{

	static DWORD WINAPI ThreadProcDispatcher(LPVOID lParam);

	static std::string WindowsGetErrorString(int error_code)
	{
		std::wstring str;
		wchar_t* buffer = NULL;
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&buffer, 0, NULL);
		str = buffer;
		LocalFree(buffer);
		return std::string(str.begin(), str.end());
	}

	WindowsClient::WindowsClient() :
		m_active(false),
		m_socket(INVALID_SOCKET),
		m_connected(false),
		m_disconnected(false),
		m_threadHandle(NULL),
		m_threadID(NULL)
	{
		WSADATA wsaData;		
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			BANAN_ASSERT(false, "WSAStartup()\n%s\n", WindowsGetErrorString(WSAGetLastError()).c_str());

		m_recvBuffer.len = BANAN_MAX_MESSAGE_SIZE;
		m_recvBuffer.buf = new char[BANAN_MAX_MESSAGE_SIZE];

		m_sendBuffer.len = BANAN_MAX_MESSAGE_SIZE;
		m_sendBuffer.buf = new char[BANAN_MAX_MESSAGE_SIZE];
	}

	WindowsClient::~WindowsClient()
	{
		Disconnect();
		WSACleanup();
	}

	void WindowsClient::Connect(const std::string& ip, int port, InternetLayer il)
	{
		int ret;
		std::wstring ip_wstr;
		PADDRINFOW res = NULL;
		PADDRINFOW ptr = NULL;
		ADDRINFOW hint{};

		if (il == InternetLayer::IPv4)
			hint.ai_family = AF_INET;
		if (il == InternetLayer::IPv6)
			hint.ai_family = AF_INET6;

		hint.ai_socktype = SOCK_STREAM;

		ret = GetAddrInfoW(ip_wstr.c_str(), std::to_wstring(port).c_str(), &hint, &res);
		BANAN_ASSERT(ret == 0, "getaddrinfo()\n%s\n", WindowsGetErrorString(ret).c_str());

		for (ptr = res; ptr != NULL; ptr = ptr->ai_next)
		{
			m_socket = WSASocketW(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol, NULL, NULL, WSA_FLAG_OVERLAPPED);
			if (m_socket == INVALID_SOCKET)
			{
				BANAN_WARN("WSASocketW()\n%s\n", WindowsGetErrorString(WSAGetLastError()).c_str());
				continue;
			}

			ret = WSAConnect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen, NULL, NULL, NULL, NULL);
			if (ret == 0)
				break;
			BANAN_WARN("WSAConnect()\n%s\n", WindowsGetErrorString(WSAGetLastError()).c_str());

			closesocket(m_socket);
		}
		FreeAddrInfoW(res);
		BANAN_ASSERT(ptr != NULL, "Could not create socket!\n");


		m_threadHandle = CreateThread(NULL, 0, ThreadProcDispatcher, this, CREATE_SUSPENDED, &m_threadID);
		BANAN_ASSERT(m_threadHandle != NULL, "CreateThread()\n%s\n", WindowsGetErrorString(GetLastError()).c_str());
	}

	void WindowsClient::DisconnectNoJoin()
	{

	}

	void WindowsClient::Disconnect()
	{

	}

	void WindowsClient::Send(const Message& message)
	{

	}

	void WindowsClient::QueryUpdates()
	{
		if (!m_messagesReceived.Empty() && m_active)
		{
			if (m_messageCallback)
			{
				Message message;
				while (m_messagesReceived.Pop(message))
					m_messageCallback(message);
			}
			else
			{
				m_messagesReceived.Clear();
			}
		}

		if (m_connected)
		{
			if (m_connectionCallback)
				m_connectionCallback();
			m_connected = false;
		}
		
		if (m_disconnected)
		{
			Disconnect();
			if (m_disconnectionCallback)
				m_disconnectionCallback();
			m_disconnected = false;
		}
	}

	void WindowsClient::MessageRecieve()
	{

	}

	void WindowsClient::MessageSend()
	{

	}

	DWORD WindowsClient::ThreadProc()
	{

	}



	static DWORD WINAPI ThreadProcDispatcher(LPVOID lParam)
	{
		WindowsClient* client = reinterpret_cast<WindowsClient*>(lParam);
		return client->ThreadProc();
	}

}