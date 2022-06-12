#include "bgepch.h"
#include "Server.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsServer.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxServer.h"
#endif

namespace Banan::Networking
{

	Scope<Server> Server::Create(uint64_t thread_count)
	{
#if defined BANAN_PLATFORM_WINDOWS
		return CreateScope<WindowsServer>(thread_count);
#elif defined BANAN_PLATFORM_LINUX
		return CreateScope<LinuxServer>(thread_count);
#endif
		return nullptr;
	}

}