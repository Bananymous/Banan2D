#include "bgepch.h"
#include "LinuxWindow.h"

#include "../../Banan/Core/ConsoleOutput.h"
#include "../../Banan/Renderer/RenderContext.h"

namespace Banan
{

	LinuxWindow::LinuxWindow(const std::wstring& title, uint32_t width, uint32_t height, bool vsync, const EventCallbackFn& callback)
    {
        m_data.title = std::string(title.begin(), title.end());
        m_data.width = width;
        m_data.height = height;
        m_data.vsync = vsync;
        m_data.eventCallback = callback;

        m_data.display = ::XOpenDisplay(NULL);
        BANAN_ASSERT(m_data.display, "Could not open display\n");

        ::Window root = DefaultRootWindow(m_data.display);

        GLint att[] { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        m_data.visualInfo = glXChooseVisual(m_data.display, 0, att);
        BANAN_ASSERT(m_data.visualInfo, "No visual found!\n");

        Colormap cmap = ::XCreateColormap(m_data.display, root, m_data.visualInfo->visual, AllocNone);

        XSetWindowAttributes swa;
        swa.colormap = cmap;
        swa.event_mask = KeyPressMask | KeyReleaseMask | ResizeRedirectMask;

        m_data.window = ::XCreateWindow(m_data.display, root, 0, 0, width, height, 0, m_data.visualInfo->depth, InputOutput, m_data.visualInfo->visual, CWColormap | CWEventMask, &swa);

        ::XMapWindow(m_data.display, m_data.window);
        ::XStoreName(m_data.display, m_data.window, m_data.title.c_str());
    
        m_renderContext = RenderContext::Create(this);
        m_renderContext->Init();
        m_renderContext->SetVSync(m_data.vsync);
    }
	
    LinuxWindow::~LinuxWindow()
    {
        DestroyScope(m_renderContext);

        ::XCloseDisplay(m_data.display);
    }
	
    void LinuxWindow::OnUpdate(bool minimized)
    {
        // Handle events

        if (!minimized)
        {
            // Update controllers
            m_renderContext->SwapBuffers();
        }
    }
	
    void LinuxWindow::SetVSync(bool enable)
    {
        m_data.vsync = enable;
        m_renderContext->SetVSync(enable);
    }
	
    void LinuxWindow::SetTitle(const std::wstring& title)
    {
        m_data.title = std::string(title.begin(), title.end());
        XStoreName(m_data.display, m_data.window, m_data.title.c_str());
    }
	
    void* LinuxWindow::GetRenderContext()
    {
        return m_renderContext->GetContext();
    }

}