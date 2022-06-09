#include "bgepch.h"
#include "Client.h"

#if defined BANAN_PLATFORM_WINDOWS
	#include "Platform/Windows/WindowsClient.h"
#elif defined BANAN_PLATFORM_LINUX
	#include "Platform/Linux/LinuxClient.h"
#endif

namespace Banan::Networking
{

		Scope<Client> Client::Create()
		{
#if defined BANAN_PLATFORM_WINDOWS
			return CreateScope<WindowsClient>();
#elif defined BANAN_PLATFORM_LINUX
			return CreateScope<LinuxClient>();
#endif
			return nullptr;
		}

}