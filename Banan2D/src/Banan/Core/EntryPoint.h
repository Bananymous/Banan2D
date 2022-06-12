#pragma once

extern Banan::Application* Banan::CreateApplication(int argc, char** argv);

#ifdef BANAN_USE_GLFW

int main(int argc, char** argv)
{
	auto app = Banan::CreateApplication(argc, argv);
	app->Run();
	delete app;
}

#elif defined BANAN_PLATFORM_WINDOWS

#include "Banan/Core/ConsoleOutput.h"
#include "Platform/Windows/WindowsWindow.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR pCmdLine, _In_ int nCmdShow)
{
#ifndef BANAN_DISABLE_CONSOLE
	FILE* stream = NULL;
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

		freopen_s(&stream, "CONIN$",  "r", stdin);
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
	} while (0);
#endif

	Banan::WindowsWindow::SetHInstance(hInstance);

	auto app = Banan::CreateApplication(__argc, __argv);
	app->Run();
	delete app;

}

#elif defined BANAN_PLATFORM_LINUX

int main(int argc, char** argv)
{
	auto app = Banan::CreateApplication(argc, argv);
	app->Run();
	delete app;
}

#else

#error Unsupported platform (for now)

#endif