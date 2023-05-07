#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <glad/gl.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// My includes
#include <LCClient/UI/ConversationList.h>

#include <array>


// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

class Application final
{
public:
    Application();
	~Application();

	void Start();
	void Update();

    ConversationList* GetConversationListPtr() { return m_ListPtr; }
private:
	GLFWwindow* m_WindowPtr = nullptr;
	ImGuiIO* m_IOPtr = nullptr;
	ImFont* m_Font24, *m_Font18, *m_Font12;

	// Some local chat app vars
    ConversationList* m_ListPtr;
	// Some random ImGui test vals
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};