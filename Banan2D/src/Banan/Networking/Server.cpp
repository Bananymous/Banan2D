#include "bgepch.h"
#include "Server.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsServer.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxServer.h"
#endif

namespace Banan::Networking
{

	Scope<Server> Server::Create(int port, TransportLayer tl, InternetLayer il)
	{
#if defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsServer>(port, tl, il);
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<LinuxServer>(port, tl, il);
#endif
	}

}