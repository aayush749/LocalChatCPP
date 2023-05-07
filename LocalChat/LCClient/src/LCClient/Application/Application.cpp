#include <LCClient/Application/Application.h>

extern TextureLoader GLOBAL_TEX_LOADER;
extern std::array<const char*, (size_t)TextureType::COUNT> texturePaths;
extern LCClient GLOBAL_CLIENT;

static void SetupFonts();

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

Application::Application()
{
    Logger::LogInfo("Application Started!");
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        Logger::LogError("Could not initialize glfw, quitting.");
        std::exit(EXIT_FAILURE);
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    m_WindowPtr = glfwCreateWindow(1280, 720, "LocalChat", NULL, NULL);
    if (m_WindowPtr == NULL)
    {
        Logger::LogError("Unable to create a window, quitting.");
        std::exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(m_WindowPtr);

    int version = gladLoadGL(glfwGetProcAddress);
    Logger::logfmt<Log::WARNING>("gladLoadGL returned %d", version);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    m_IOPtr = &io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 9.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.7f;
    }

    //io.ConfigViewportsNoDecoration = false;
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = false;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_WindowPtr, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    m_Font24 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaCode.ttf", 24.0f);
    m_Font18 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaCode.ttf", 18.0f);
    m_Font12 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaCode.ttf", 12.0f);

    SetupFonts();

    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Load Textures
    GLOBAL_TEX_LOADER.LoadAllTextures(texturePaths);

    m_ListPtr = new ConversationList({
            {1001, "Aayush Anand"},
            {1002, "Prince Vishwakarma"}
        });

    AudioManager::Init();

    // Load audios
    const char* sentMsgAudioPath = "../../../../LocalChat/LCClient/res/audio/audio-send.wav";
    const char* recvMsgAudioPath = "../../../../LocalChat/LCClient/res/audio/audio-receive.wav";
    AudioManager::LoadAudio(AudioAlert::MSG_SENT, sentMsgAudioPath);
    AudioManager::LoadAudio(AudioAlert::MSG_RECV, recvMsgAudioPath);
}

void Application::Start()
{
    LCClient::SetApplicationPtr(this);
}

void Application::Update()
{
    ImGuiIO& io = *m_IOPtr;

    while (!glfwWindowShouldClose(m_WindowPtr))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

        // Start the Dear ImGui frame

        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::PushFont(m_Font18);
        //// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        //if (show_demo_window)
        //    ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            // Alt + F4 to quit
            if (io.KeyAlt && io.KeysDown[GLFW_KEY_F4])
            {
                Logger::LogInfo("Close requested from shortcut key");
                break;
            }

            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit", "Alt+F4"))
                    {
                        Logger::LogInfo("Close requested!");

                        break;  // will close the render loop
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());




            ImGui::Begin("Chats", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.

            //ImGui::SameLine();
            //ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            //ImGui::SameLine();
            //ImGui::Checkbox("Another Window", &show_another_window);

            //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            //    counter++;
            //ImGui::SameLine();
            //ImGui::Text("counter = %d", counter);

            //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            // My code

            if (ImGui::Button("Add New Contact"))
                ImGui::OpenPopup("Add New Contact");

            // Always center this window when appearing
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

            if (ImGui::BeginPopupModal("Add New Contact", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static char hashText[11] = { 0 };
                static char contactName[256] = { 0 };

                ImGui::Text("Enter the details...");

                ImGui::Separator();

                ImGui::Text("Display Name");
                ImGui::SameLine();
                ImGui::Spacing();
                ImGui::InputText("##NameInputText", contactName, sizeof(contactName));

                ImGui::Text("Hash");
                ImGui::SameLine();
                ImGui::Spacing();
                ImGui::InputText("##HashInputText", hashText, sizeof(hashText));

                ImGui::Separator();

                if (ImGui::Button("Create"))
                {
                    uint64_t clientHash = std::stoull(std::string(hashText));

                    m_ListPtr->AddNewContact(clientHash, contactName);

                    ImGui::CloseCurrentPopup();
                }
                ImGui::SetItemDefaultFocus();

                ImGui::SameLine();

                if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }

                ImGui::EndPopup();
            }

            m_ListPtr->OnCreate();
            m_ListPtr->OnImGuiRender();

            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        ImGui::PopFont();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_WindowPtr, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(m_WindowPtr);
    }
}

Application::~Application()
{
    delete m_ListPtr;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_WindowPtr);
    glfwTerminate();

    AudioManager::Cleanup();
}

void SetupFonts()
{
    FontManager::SetFont(FontUsage::FONT_USAGE_CONVERSATION_LIST, "c:\\Windows\\Fonts\\CascadiaCode.ttf", 24.0f);
    FontManager::SetFont(FontUsage::FONT_USAGE_MESSAGE_BLOB, "C:/Windows/Fonts/REFSAN.TTF", 24.0f);
}