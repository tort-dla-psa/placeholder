#include <chrono>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "rwqueue/readerwriterqueue.h"
#include <SDL.h>
#include <string>
#include <tuple>
#include <vector>
#include <complex>
#include <thread>
#include <mutex>
#include <atomic>
#include <GL/glew.h>
#include "ragdoll.h"

int main(int, char**) {
    moodycamel::BlockingReaderWriterQueue<int> q;
    ragdoll_handler r(q);
    r.run();

    const unsigned w = 1920, h = 1080;

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
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
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = glewInit() != GLEW_OK;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Main loop
    bool done = false;
    GLuint id;
    GLubyte raw[w*h*3];
    std::fill(std::begin(raw), std::end(raw), 128);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, &raw);
    glBindTexture(GL_TEXTURE_2D, 0);
    bool iter_incr = true;

    ImVec2 drag_1, drag_2;
    bool drag_beg;
    polygon pol;
    pol.set_pos({0,0});
    pol.add_vert({100,   0});
    pol.add_vert({100, 100});
    pol.add_vert({  0, 100});
    pol.set_angle(0);
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();
        auto drawList = ImGui::GetBackgroundDrawList();

        ImGui::Begin("Parameters");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        if(ImGui::IsMouseClicked(0)){
            drag_1 = ImGui::GetMousePos();
        }
        if(ImGui::GetMouseDragDelta(0).x > .0f &&
            ImGui::GetMouseDragDelta(0).y > .0f)
        {
            drag_beg = true;
        }
        if(drag_beg){
            drag_2 = ImGui::GetMousePos();
        }
        if(ImGui::IsMouseReleased(0)){
            drag_2 = ImGui::GetMousePos();
            if(drag_beg){
            }
            drag_beg = false;
        }

        auto size = ImGui::GetIO().DisplaySize;
        int res;
        if(q.try_dequeue(res)){
            do{
                /*
                glBindTexture(GL_TEXTURE_2D, id);
                glTexSubImage2D(GL_TEXTURE_2D, 0, i.x, i.y,
                        i.w, i.h, GL_RGB, GL_UNSIGNED_BYTE,
                        i.pixels.data());
                glBindTexture(GL_TEXTURE_2D, 0);
                */
            }while(q.try_dequeue(res));
        }
        drawList->AddImage((void*)(intptr_t)id, ImVec2(0,0), ImVec2(1920, 1080));
        if(drag_beg){
            auto str = [](double data){ return std::to_string(data).substr(0, 4); };
            drawList->AddLine(drag_1, drag_2, ImColor{255,255,255});
        }

        auto prev_it = pol.begin();
        for(auto it = std::next(prev_it); it != pol.end(); it++){
            auto beg = *prev_it;
            auto end = *it;
            drawList->AddLine({(float)beg.x, (float)beg.y}, {(float)end.x, (float)end.y}, ImColor{255,255,255});
            prev_it = it;
        }
        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    r.stop();

    return 0;
}
