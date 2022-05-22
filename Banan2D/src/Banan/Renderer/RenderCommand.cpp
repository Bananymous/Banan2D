#include "bgepch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Banan
{

	RendererAPI* RenderCommand::s_rendererAPI = new OpenGLRendererAPI;

}