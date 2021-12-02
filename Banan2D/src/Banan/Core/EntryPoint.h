#pragma once

extern Banan::Application* Banan::CreateApplication();

#ifdef BANAN_PLATFORM_WINDOWS

#include "Banan/Core/ConsoleOutput.h"
#include "Platform/Windows/WindowsWindow.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR pCmdLine, _In_ int nCmdShow)
{
#ifndef BANAN_DISABLE_CONSOLE
	FILE* stream;
	AllocConsole();

	do
	{
		HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			BANAN_WARN("Could not enable colors\n");
			break;
		}

		DWORD dwMode = 0;
		if (!GetConsoleMode(hOut, &dwMode))
		{
			BANAN_WARN("Could not enable colors\n");
			break;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!SetConsoleMode(hOut, dwMode))
		{
			BANAN_WARN("Could not enable colors\n");
			break;
		}

		freopen_s(&stream, "CONOUT$", "wb", stdout);
	} while (0);

#endif

	Banan::WindowsWindow::SetHInstance(hInstance);

	auto app = Banan::CreateApplication();
	app->Run();
	delete app;

}

#else

#error Only supports windows (for now)

#endif