#include "bgepch.h"
#include "Server.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsServer.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxServer.h"
#endif

namespace Banan::Networking
{

	Scope<Server> Server::Create()
	{
#if defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsServer>();
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<LinuxServer>();
#endif
		return nullptr;
	}

}