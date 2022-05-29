#include "bgepch.h"
#include "Client.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsClient.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxClient.h"
#endif

namespace Banan::Networking
{

		Scope<Client> Client::Create(const std::string& ip, int port, TransportLayer tl, InternetLayer il)
		{
#if defined BANAN_PLATFORM_WINDOWS
			return CreateScope<WindowsClient>(ip, port, tl, il);
#elif defined BANAN_PLATFORM_LINUX
			return CreateScope<LinuxClient>(ip, port, tl, il);
#endif
			return nullptr;
		}

}