#include "pch.h"
#include "toad.h"
#include "ImGuiWindow.h"

namespace toad
{

ImGuiWindow::ImGuiWindow(std::string window_title, int win_height, int win_width)
    : m_window_name(std::move(window_title)), m_window_width(win_width), m_window_height(win_height)
{
}

ImGuiWindow::~ImGuiWindow()
{
    DestroyWindow();
}

void ImGuiWindow::StartWindow()
{
    std::cout << "Creating window with name: " << m_window_name << std::endl;

    //if (!m_uifunc_set)
    //{
    //    std::cout << "The UI function has not been set for this window " << std::endl;
    //}

    // there should be no duplicate window names 
    if (m_window_from_name.contains(m_window_name))
    {
        std::cout << "Window " << m_window_name << " already exists! " << std::endl;
        assert(0);
        m_running = false;
        return;
    }

    std::cout << "Starting window thread\n";
    m_window_thread = std::thread(&ImGuiWindow::CreateImGuiWindow, this, m_window_name, m_window_height, m_window_width);
}

void ImGuiWindow::DestroyWindow()
{
    std::lock_guard lock(m_destroy_window_mutex);

    if (!m_running) return;

    std::cout << "Closing window: " << m_window_name << std::endl;

    m_window_from_name.erase(m_window_name);
    m_window_from_hwnd.erase(m_window);

    if (m_window_thread.joinable())
        m_window_thread.join();

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(m_window);
    glfwTerminate();
    
    m_running = false;

}

GLFWwindow* ImGuiWindow::GetHandle() const
{
    return m_window;
}

bool ImGuiWindow::IsActive() const
{
    return m_running && !m_should_close;
}

bool ImGuiWindow::IsFontUpdated() const
{
    return !m_update_font;
}

void ImGuiWindow::SetUI(const std::function<void(ImGuiIO* io)>& ui_func)
{
    m_uifunc_set = true;
    m_uifunc = ui_func;
}

void ImGuiWindow::AddFontTTF(std::string_view pathTTF)
{
    m_update_font_path = pathTTF;
    m_update_font = true;
}

void GLFWErrorCallback(int error, const char* description)
{
    log_errorf("ImGuiWindow error: %d, %s", error, description);
}

bool ImGuiWindow::CreateImGuiWindow(const std::string& window_title, int win_height, int win_width)
{
    glfwSetErrorCallback(GLFWErrorCallback);
    if (!glfwInit())
        return false;

    // Create window with graphics context
    m_window = glfwCreateWindow(1280, 720, window_title.data(), nullptr, nullptr);
    if (m_window == nullptr)
        return false;
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    m_io = &ImGui::GetIO(); (void)m_io;
    //m_io->ConfigWindowsMoveFromTitleBarOnly = true;
    m_io->Fonts->AddFontDefault();
    m_io->IniFilename = NULL; // disable imgui.ini file
    m_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    m_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    m_style = &ImGui::GetStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL2_Init();

    m_window_from_name.insert({ window_title.data(), this });
    m_window_from_hwnd.insert({ m_window, this });
    std::cout << "Window " << window_title << " has successfully been created " << std::endl;

    while (m_running && !m_should_close)
    {
        UpdateMenu();
    }
    return true;
}

void ImGuiWindow::UpdateMenu()
{
    glfwPollEvents();

    if (m_should_close)
        return;

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //ui
    m_uifunc(m_io);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    // If you are using this code with non-legacy OpenGL header/contexts (which you should not, prefer using imgui_impl_opengl3.cpp!!),
    // you may need to backup/reset/restore other state, e.g. for current shader using the commented lines below.
    //GLint last_program;
    //glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    //glUseProgram(0);
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    //glUseProgram(last_program);

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapBuffers(m_window);
}

void ImGuiWindow::DefaultUIWindow(ImGuiIO* io)
{
    // match the window sizes 
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize(io->DisplaySize);

    // transparent static imgui window 
    constexpr static auto window_flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Default Window UI", nullptr, window_flags);
    //center_text({ 1,0,0,1 }, "NO UI HAS BEEN SET FOR THIS WINDOW");
    ImGui::Text("No ui has been set for this window");
    ImGui::End();
}

ImGuiWindow* ImGuiWindow::GetWindowInstance(std::string_view window_name)
{
    auto it = m_window_from_name.find(window_name.data());
    if (it != m_window_from_name.end())
        return it->second;
    return nullptr;
}

ImGuiWindow* ImGuiWindow::GetWindowInstance(GLFWwindow* hwnd)
{
    auto it = m_window_from_hwnd.find(hwnd);
    if (it != m_window_from_hwnd.end())
        return it->second;
    return nullptr;
}

}
