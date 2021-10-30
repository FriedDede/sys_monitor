
#include "imgui/imgui.h"
#include "imgui/opengl3/imgui_impl_sdl.h"
#include "imgui/opengl3/imgui_impl_opengl3.h"

#include "include/format.h"
#include "include/process.h"
#include "include/processor.h"
#include "include/system.h"
#include "include/logger.h"
#include "include/updater.h"

#include <stdio.h>
#include <string.h>
#include <vector>
#include <thread>
#include <SDL.h>

#define refresh_interval 45

// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h> // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h> // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h> // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE      // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h> // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE        // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h> // Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Main code
int main(int, char **)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window *window = SDL_CreateWindow("Sys Monitor OpenGL3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync
// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char *name)
                          { return (glbinding::ProcAddress)SDL_GL_GetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_sys_window = false;
    bool show_cpu_window = false;
    bool show_mem_window = false;
    bool show_log_window = false;
    bool show_proc_window = false;
    ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 0.1f);

    //System
    //System class only return this values it does not save those in a class instance
    System system;
    std::string OS = system.OperatingSystem();
    std::string Kernel = system.Kernel();
    int Cores = system.cpu_.CoreCount();
    std::string Hostname = system.Hostname();
    // Cpu window
    char overlay[32];
    for (auto &usage : system.cpu_.Cpu_Usage_Log)
        usage = 0;
    //Logger
    int ID = 0;
    int round = 9;
    bool logging = false;
    bool background_logging = false;
    long log_start_time = 0;
    int background_round = 0;

    //Background polling thread
    std::thread Updater(Updater::ProcessesUpdater, &system);
    Updater.detach();

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Main Menù
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            ImGui::Begin("Main Menù");
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Windows:");
            ImGui::Checkbox("System Info", &show_sys_window);
            ImGui::Checkbox("CPU stat", &show_cpu_window);
            ImGui::Checkbox("Memory stat", &show_mem_window);
            ImGui::Checkbox("Processes", &show_proc_window);
            ImGui::Checkbox("Process Logger", &show_log_window);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("%.3f ms/frame ", 1000.0f / ImGui::GetIO().Framerate);
            if (background_logging)
            {
                ImGui::Text("Logging pid %d", ID);
                if ((system.UpTime() - log_start_time) >= background_round)
                {
                    background_logging = false;
                }
            }
            ImGui::End();
        }

        // Windows

        if (show_sys_window)
        {
            ImGui::Begin("System info", &show_sys_window);
            //if (ImGui::Button("Close Me"))
            //    show_sys_window = false;
            ImGui::Columns(2, "System attributes:", true);
            ImGui::Text("OS:");
            ImGui::NextColumn();
            ImGui::Text(OS.c_str());
            ImGui::NextColumn();
            ImGui::Text("Kernel:");
            ImGui::NextColumn();
            ImGui::Text(Kernel.c_str());
            ImGui::NextColumn();
            ImGui::Text("Hostname:");
            ImGui::NextColumn();
            ImGui::Text(Hostname.c_str());
            ImGui::NextColumn();
            ImGui::Text("Processes:");
            ImGui::NextColumn();
            ImGui::Text("%d", system.total_processes);
            ImGui::NextColumn();
            ImGui::Text("Uptime:");
            ImGui::NextColumn();
            ImGui::Text(Format::ElapsedTime(system.UpTime()).c_str());
            ImGui::NextColumn();
            ImGui::Text("Cores:");
            ImGui::NextColumn();
            ImGui::Text("%d", Cores);
            ImGui::End();
        }
        if (show_cpu_window)
        {
            ImGui::Begin("CPU stat", &show_cpu_window);
            ImGui::Text("CPU Util: %d [%]", (int)(system.cpu_usage * 100));
            //Need to understand how the color push work
            //ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0,1.0,0.0,1.0));
            ImGui::ProgressBar(system.cpu_usage, ImVec2(-1, 0), "");
            ImGui::Text("CPU Average 1 minute: %d [%]", (int)(system.cpu1m / (float)Cores) * 100);
            ImGui::ProgressBar(system.cpu1m / (float)Cores, ImVec2(-1, 0), "");
            ImGui::Text("CPU Average 5 minute: %d [%]", (int)(system.cpu5m / (float)Cores) * 100);
            ImGui::ProgressBar(system.cpu5m / (float)Cores, ImVec2(-1, 0), "");
            sprintf(overlay, "CPU Util (45s): \n\nAVG: %d [%]", (int)(system.cpu1m / (float)Cores * 100));
            ImGui::Text(overlay);
            ImGui::SameLine();
            ImGui::PlotLines("", system.cpu_.Cpu_Usage_Log, IM_ARRAYSIZE(system.cpu_.Cpu_Usage_Log), 0, "", 0, 100, ImVec2(0, 80));
            ImGui::End();
        }
        if (show_mem_window)
        {
            ImGui::Begin("Memory stat", &show_mem_window);
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Memory Util: %d [%]", (int)(system.memory_Utilization * 100));
            ImGui::ProgressBar(system.memory_Utilization, ImVec2(-1, 0), "");
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Memory Shared: %d [%]", (int)(system.memory_Shared * 100));
            ImGui::ProgressBar(system.memory_Shared, ImVec2(-1, 0), "");
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Memory Buffer: %d [%]", (int)(system.memory_Buffer * 100));
            ImGui::ProgressBar(system.memory_Buffer, ImVec2(-1, 0), "");
            ImGui::TextColored(ImVec4(1, 1, 1, 1), "Memory Swap: %d [%]", (int)(system.memory_Swap * 100));
            ImGui::ProgressBar(system.memory_Swap, ImVec2(-1, 0), "");
            ImGui::End();
        }
        if (show_proc_window)
        {
            ImGui::Begin("Processes", &show_proc_window);
            int vectorsize = system.processes_.size();
            ImGui::Columns(9, "CPU", true);

            ImGui::Text("PID");
            ImGui::NextColumn();
            ImGui::Text("PPID");
            ImGui::NextColumn();
            ImGui::Text("NAME");
            ImGui::NextColumn();
            ImGui::Text("UID");
            ImGui::NextColumn();
            ImGui::Text("CORE [%%]");
            ImGui::NextColumn();
            ImGui::Text("RAM [KB]");
            ImGui::NextColumn();
            ImGui::Text("UPTIME");
            ImGui::NextColumn();
            ImGui::Text("STATUS");
            ImGui::NextColumn();
            ImGui::Text("COMMAND");
            ImGui::NextColumn();
            for (int i = vectorsize - 1; i >= 0; i--)
            {
                if (system.processes_[i].Read_Cpu() > 0.01)
                {
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%d", system.processes_[i].Read_Pid());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_Parent().c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_Name().c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_User().c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%d", (int)(system.processes_[i].Read_Cpu() * 100));
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_Ram().c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", Format::ElapsedTime(system.processes_[i].Read_Uptime()).c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_Status().c_str());
                    ImGui::NextColumn();
                    ImGui::TextColored(ImVec4(0.0, 1.0, 0.0, 1.0), "%s", system.processes_[i].Read_Command().c_str());
                    ImGui::NextColumn();
                }
            }
            for (int i = vectorsize - 1; i >= 0; i--)
            {
                if (system.processes_[i].Read_Cpu() < 0.01)
                {
                    ImGui::Text("%d", system.processes_[i].Read_Pid());
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_Parent().c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_Name().c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_User().c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%d", (int)(system.processes_[i].Read_Cpu() * 100));
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_Ram().c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%s", Format::ElapsedTime(system.processes_[i].Read_Uptime()).c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_Status().c_str());
                    ImGui::NextColumn();
                    ImGui::Text("%s", system.processes_[i].Read_Command().c_str());
                    ImGui::NextColumn();
                }
            }
            ImGui::End();
        }
        if (show_log_window)
        {
            background_logging = false;
            ImGui::Begin("Logger", &show_log_window);
            ImGui::InputInt("Pid", &ID, 1, 100, 0);
            ImGui::InputInt("Cycles", &round, 1, 100, 0);
            ImGui::Checkbox("LOG", &logging);
            if (true == logging)
            {
                std::thread Logger(Logger::Proc, ID, round);
                Logger.detach();
                log_start_time = system.UpTime();
                logging = false;
                show_log_window = false;
                background_logging = true;
                background_round = round;
            }
            ImGui::End();
        }
        // Rendering

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
