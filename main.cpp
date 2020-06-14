#include <chrono>
#include <ios>
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_sdl.h"
#include "imgui/examples/imgui_impl_opengl3.h"
#include "rwqueue/readerwriterqueue.h"
#include <SDL.h>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <complex>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>
#include <GL/glew.h>
#include "ragdoll.h"
#include "gfx_funcs.h"
#include "phys_obj.h"
#include "phys_eng.h"

ImColor fg_clr = ImColor({250, 250, 250});

void draw_polygon(ImDrawList* drawList, const polygon &pol,
    bool draw_labels=false, bool draw_norms=false,
    bool draw_centroid=false)
{
    auto draw_lbl = [drawList](polygon::dot_t dot){
        std::stringstream ss;
        ss.precision(2);
        ss<< std::fixed << dot.x << " " << dot.y;
        auto str = ss.str();
        drawList->AddText({dot.x, dot.y}, fg_clr, str.c_str(), NULL);
    };
    std::optional<std::vector<dot>> norms;
    if(draw_norms){
        norms = pol.normals();
    }
    for(size_t n = 0; n<pol.size(); n++){
        auto beg = pol.at(n);
        auto end = pol.at((n+1)%pol.size());
        drawList->AddLine({beg.x, beg.y}, {end.x, end.y}, fg_clr);
        if(draw_labels)
            draw_lbl(beg);
        if(draw_norms){
            auto mid = (beg+end)/2;
            auto norm = norms->at(n);
            norm += mid;
            drawList->AddLine({norm.x, norm.y}, {mid.x, mid.y}, fg_clr);
            drawList->AddCircle({norm.x, norm.y}, 3, fg_clr);
        }
    }
    if(draw_centroid){
        auto centr = pol.centroid();
        drawList->AddCircle({centr.x, centr.y}, 5, fg_clr);
    }
}

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
    bool draw_labels = false;
    bool draw_norms = false;
    bool draw_centrs = false;

    ImVec2 drag_1, drag_2, prev_drag_2;
    bool drag_beg;
    polygon pol, angle1, angle2, angle3, angle4;
    float main_w = 100;
    polygon::dot_t main_pos = {200, 200};
    float sub_w = main_w/2;
    pol.add_vert({     0,      0});
    pol.add_vert({     0, main_w});
    pol.add_vert({main_w, main_w});
    pol.add_vert({main_w,      0});
    angle1.add_vert({    0,     0});
    angle1.add_vert({    0, sub_w});
    angle1.add_vert({sub_w, sub_w});
    angle1.add_vert({sub_w,     0});
    angle2 = angle3 = angle4 = angle1;
    gfx_func::move_ref(pol, main_pos);

    phys_eng eng;
    std::vector<phys_obj> objs;
    {
        polygon pol({{0,0}, {20,30}, {40, 35}, {30, 20}, {50, 0}});
        phys_obj obj(std::move(pol));
        eng.move(obj, {400, 400});
        objs.emplace_back(std::move(obj));
    } {
        polygon pol({{0,0}, {20,30}, {40, 50}, {30, 20}, {10, 0}});
        phys_obj obj(std::move(pol));
        eng.move(obj, {450, 400});
        objs.emplace_back(std::move(obj));
    } {
        polygon pol({{0,0}, {10,20}, {10, 70}, {30, 50}, {35, 15}, {10, 0}});
        phys_obj obj(std::move(pol));
        eng.move(obj, {450, 400});
        objs.emplace_back(std::move(obj));
    }

    phys_obj* dragging_obj;
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
        ImGui::Checkbox("Draw verts labels", &draw_labels);
        ImGui::Checkbox("Draw norms", &draw_norms);
        ImGui::Checkbox("Draw centroids", &draw_centrs);
        ImGui::End();
        if(ImGui::IsMouseClicked(0)){
            drag_1 = ImGui::GetMousePos();
            for(auto& obj:objs){
                if(obj.verts().contains({drag_1.x, drag_1.y})){
                    dragging_obj = &obj;
                    break;
                }
            }
        }
        if(std::abs(ImGui::GetMouseDragDelta(0).x) > .0f &&
            std::abs(ImGui::GetMouseDragDelta(0).y) > .0f)
        {
            drag_beg = true;
        }
        if(drag_beg){
            drag_2 = ImGui::GetMousePos();
        }
        if(ImGui::IsMouseReleased(0)){
            drag_2 = ImGui::GetMousePos();
            drag_beg = false;
            dragging_obj = nullptr;
        }

        auto size = ImGui::GetIO().DisplaySize;
        int res;
        /*
        if(q.try_dequeue(res)){
            do{
                glBindTexture(GL_TEXTURE_2D, id);
                glTexSubImage2D(GL_TEXTURE_2D, 0, i.x, i.y,
                        i.w, i.h, GL_RGB, GL_UNSIGNED_BYTE,
                        i.pixels.data());
                glBindTexture(GL_TEXTURE_2D, 0);
            }while(q.try_dequeue(res));
        }
        */
        gfx_func::rotate_ref(pol, 0.1, {250, 250}); //rotate around dot
        gfx_func::move_ref(angle1, pol.at(0)-sub_w/2);
        gfx_func::move_ref(angle2, pol.at(1)-sub_w/2);
        gfx_func::move_ref(angle3, pol.at(2)-sub_w/2);
        gfx_func::move_ref(angle4, pol.at(3)-sub_w/2);
        gfx_func::rotate_ref(angle1, 0.1, pol.at(0)); 
        gfx_func::rotate_ref(angle2, 0.1, pol.at(1)); 
        gfx_func::rotate_ref(angle3, 0.1, pol.at(2)); 
        gfx_func::rotate_ref(angle3, 0.1, pol.at(3)); 
        
        drawList->AddImage((void*)(intptr_t)id, ImVec2(0,0), ImVec2(1920, 1080));
        if(drag_beg){
            auto str = [](double data){ return std::to_string(data).substr(0, 4); };
            if(dragging_obj){
                auto &ref = *dragging_obj;
                eng.move(ref, {drag_2.x, drag_2.y});
            }else{
                drawList->AddLine(drag_1, drag_2, fg_clr);
            }
            if(draw_labels){
                std::stringstream ss;
                ss<< std::fixed;
                ss.precision(2);
                ss << drag_2.x << " " << drag_2.y;
                drawList->AddText({drag_2.x, drag_2.y}, fg_clr, ss.str().c_str(), NULL);
                ss = std::stringstream();
                ss.flush();
                ss << drag_1.x << " " << drag_1.y;
                drawList->AddText({drag_1.x, drag_1.y}, fg_clr, ss.str().c_str(), NULL);
            }
        }

        draw_polygon(drawList,    pol, draw_labels, draw_norms, draw_centrs);
        draw_polygon(drawList, angle1, draw_labels, draw_norms, draw_centrs);
        draw_polygon(drawList, angle2, draw_labels, draw_norms, draw_centrs);
        draw_polygon(drawList, angle3, draw_labels, draw_norms, draw_centrs);
        draw_polygon(drawList, angle4, draw_labels, draw_norms, draw_centrs);
        for(auto& obj:objs){
            draw_polygon(drawList,  obj.verts(), draw_labels, draw_norms, draw_centrs);
        }
        prev_drag_2 = drag_2;
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
