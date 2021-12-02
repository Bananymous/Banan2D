#pragma once


#include "Banan/Core/PlatformDetection.h"

#pragma warning(disable : 26812)

// MACROS
#define BIT(x) (1 << x)

#define BANAN_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef BANAN_PLATFORM_WINDOWS
	#include <Windows.h>
	#define BANAN_DEBUG_BREAK() __debugbreak()
	#define BANAN_MAIN int WINAPI ::wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow)
#else
	#define BANAN_MAIN int main(int argc, char** argv)
#endif


#include <memory>

namespace Banan
{
	
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	constexpr void DestroyScope(Scope<T>& scope)
	{
		scope.reset(nullptr);
	}



	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}