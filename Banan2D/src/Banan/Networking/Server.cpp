#include "bgepch.h"
#include "Server.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsServer.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxServer.h"
#endif

namespace Banan::Networking
{

	Scope<Server> Server::Create(int port, TransportLayer tl, InternetLayer ip)
	{
#if defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsServer>(port, tl, ip);
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<Banan::Networking::LinuxServer>(port, tl, ip);
#endif
	}

}