#pragma once

extern Banan::Application* Banan::CreateApplication();

#ifdef BANAN_PLATFORM_WINDOWS

#include "Platform/Windows/WindowsWindow.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow)
{
	BANAN_ENABLE_CONSOLE();

	Banan::WindowsWindow::SetHInstance(hInstance);

	auto app = Banan::CreateApplication();
	app->Run();
	delete app;

}

#else

#error Only supports windows (for now)

#endif