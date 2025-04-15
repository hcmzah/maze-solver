#include "gui.hpp"

#include "../../image/image.hpp"
#include "../../pathfinder/pathfinder.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <format>
#include <algorithm>

Image image;
Pathfinder pathfinder;

GUI::GUI() {
    _running = true;
    _current_mode = PositionMode::None;
    _image_texture = 0;
    _zoom = 1.0f;
    _pan_offset = ImVec2(0.0f, 0.0f);
    _path_alpha = 0.8f;
    _path_thickness = 2.0f;
    _marker_size = 5;
    _path_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    _start_marker_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    _end_marker_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
}

GUI::~GUI() {
    Shutdown();
}

void GUI::Init(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    SetupImGuiStyle();
}

void GUI::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    image.CleanupTexture();
}

void GUI::BeginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::EndFrame(GLFWwindow* window) {
    ImGui::Render();
    int display_width, display_height;
    glfwGetFramebufferSize(window, &display_width, &display_height);
    glViewport(0, 0, display_width, display_height);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_context);
    }

    glfwSwapBuffers(window);
}

void GUI::Render() {
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_Once);
    ImGui::Begin("Maze Solver", &_running, ImGuiWindowFlags_NoCollapse);

    ImGui::Columns(2, nullptr, true);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.35f);

    RenderControlsPanel();
    ImGui::NextColumn();
    RenderImagePanel();

    ImGui::End();
}

void GUI::RenderControlsPanel() {
    ImGui::BeginChild("Controls", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysUseWindowPadding);

    ImGui::TextLinkOpenURL("github.com/hcmzah/maze-solver", "https://github.com/hcmzah/maze-solver");
    ImGui::Separator();
    ImGui::Text("Image Size: %dpx x %dpx", image.GetWidth(), image.GetHeight());

    if (ImGui::Button("Load Image", ImVec2(-1, 35))) {
        image.SelectImageFromFileDialog();
        _image_texture = image.GetTexture();
        if (_image_texture) {
            _maze = image.ConvertToMazeGrid();
        }
    }

    if (ImGui::Button("Reset Image Position", ImVec2(-1, 35))) {
        _zoom = 1.0f;
        _pan_offset = ImVec2(0.0f, 0.0f);
    }

    ImGui::Separator();

    if (_image_texture) {
        ImGui::Text("Set Positions:");

        std::string start_btn_label = std::format("Start ({}, {})", image.GetStartPosition().x, image.GetStartPosition().y);
        if (ImGui::Button(start_btn_label.c_str(), ImVec2(-1, 35))) {
            _current_mode = PositionMode::SetStart;
        }

        std::string end_btn_label = std::format("End ({}, {})", image.GetEndPosition().x, image.GetEndPosition().y);
        if (ImGui::Button(end_btn_label.c_str(), ImVec2(-1, 35))) {
            _current_mode = PositionMode::SetEnd;
        }

        if (ImGui::Button("Solve Maze")) {
            _solved_path.clear();
            _solved_path = pathfinder.SolveMazeWithDijkstra(_maze, image.GetStartPosition(), image.GetEndPosition());
        }

        ImGui::Separator();

        RenderAdvancedSettings();
    }

    ImGui::EndChild();
}


void GUI::RenderImagePanel() {
    ImGui::BeginChild("ImagePanel", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (_maze.empty() || _maze[0].empty()) {
        ImGui::Text("No image loaded...");
        ImGui::EndChild();
        return;
    }

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float aspect_ratio = (float)image.GetWidth() / image.GetHeight();

    HandleZoom();
    HandlePanning();

    float img_width = available_size.x * _zoom;
    float img_height = img_width / aspect_ratio;
    ImVec2 image_pos = GetCenteredPosition(available_size, img_width, img_height);

    ImGui::SetCursorPos(image_pos);
    ImVec2 screen_pos = ImGui::GetCursorScreenPos();

    ImGui::Image((void*)(intptr_t)_image_texture, ImVec2(img_width, img_height));
    RenderOverlay(screen_pos, img_width, img_height);
    HandleImageClick(screen_pos, img_width, img_height);

    ImGui::EndChild();
}

void GUI::HandleZoom() {
    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
        return;
    }

    float scroll = ImGui::GetIO().MouseWheel;
    if (scroll != 0.0f) {
        _zoom = std::clamp(_zoom + scroll * 0.1f, 0.5f, 3.0f);
    }
}

void GUI::HandlePanning() {
    if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
        return;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        _pan_offset.x += delta.x;
        _pan_offset.y += delta.y;
    }
}

ImVec2 GUI::GetCenteredPosition(const ImVec2& avail_size, float img_width, float img_height) {
    return ImVec2((avail_size.x - img_width) * 0.5f + _pan_offset.x, (avail_size.y - img_height) * 0.5f + _pan_offset.y);
}

void GUI::RenderOverlay(const ImVec2& image_pos, float displayed_width, float displayed_height) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto GridToScreen = [&](ImVec2 grid_pos) {
        return ImVec2(image_pos.x + (grid_pos.x / image.GetWidth()) * displayed_width,
            image_pos.y + (grid_pos.y / image.GetHeight()) * displayed_height);
    };

    draw_list->AddCircleFilled(GridToScreen(image.GetStartPosition()), _marker_size, ImGui::ColorConvertFloat4ToU32(_start_marker_color));
    draw_list->AddCircleFilled(GridToScreen(image.GetEndPosition()), _marker_size, ImGui::ColorConvertFloat4ToU32(_end_marker_color));

    _path_color.w = _path_alpha;
    for (size_t i = 1; i < _solved_path.size(); ++i) {
        draw_list->AddLine(GridToScreen(_solved_path[i - 1]), GridToScreen(_solved_path[i]), ImGui::ColorConvertFloat4ToU32(_path_color), _path_thickness);
    }
}

void GUI::RenderAdvancedSettings() {
    if (ImGui::CollapsingHeader("Advanced Settings")) {
        ImGui::Text("Path Settings");
        ImGui::SliderFloat("Alpha", &_path_alpha, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Thickness", &_path_thickness, 0.5f, 5.0f, "%.1f");
        ImGui::ColorEdit3("Path Color", (float*)&_path_color);

        ImGui::Separator();
        ImGui::Text("Marker Settings");
        ImGui::SliderInt("Marker Size", &_marker_size, 1, 15);
        ImGui::ColorEdit3("Start Marker", (float*)&_start_marker_color);
        ImGui::ColorEdit3("End Marker", (float*)&_end_marker_color);

        ImGui::Separator();
        if (ImGui::Button("Reset Defaults")) {
            _path_alpha = 0.8f;
            _path_thickness = 2.0f;
            _marker_size = 5;
            _path_color = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
            _start_marker_color = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
            _end_marker_color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
}

void GUI::HandleImageClick(const ImVec2& image_pos, float displayed_width, float displayed_height)
{
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        ImVec2 mouse_screen = ImGui::GetMousePos();

        float local_x = mouse_screen.x - image_pos.x;
        float local_y = mouse_screen.y - image_pos.y;

        float frac_x = local_x / displayed_width;
        float frac_y = local_y / displayed_height;

        int grid_x = static_cast<int>(frac_x * image.GetWidth());
        int grid_y = static_cast<int>(frac_y * image.GetHeight());

        if (grid_x >= 0 && grid_x < image.GetWidth() &&
            grid_y >= 0 && grid_y < image.GetHeight())
        {
            if (_current_mode == PositionMode::SetStart) {
                image.SetStartPosition(ImVec2(grid_x, grid_y));
                _current_mode = PositionMode::None;
            }
            else if (_current_mode == PositionMode::SetEnd) {
                image.SetEndPosition(ImVec2(grid_x, grid_y));
                _current_mode = PositionMode::None;
            }
        }
    }
}

void GUI::SetupImGuiStyle() {
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 5.0f;

    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 8);
    style.WindowPadding = ImVec2(15, 15);
    style.ScrollbarSize = 14.0f;
    style.FrameBorderSize = 1.0f;

    ImVec4 raisin_black = ImVec4(44 / 255.0f, 43 / 255.0f, 60 / 255.0f, 1.0f); // #2C2B3C (Title Bar & Dropdown)
    ImVec4 rich_black = ImVec4(18 / 255.0f, 20 / 255.0f, 32 / 255.0f, 1.0f);   // #121420 (Background)
    ImVec4 onyx = ImVec4(64 / 255.0f, 63 / 255.0f, 76 / 255.0f, 1.0f);         // #403F4C (Secondary buttons)
    ImVec4 indian_red = ImVec4(183 / 255.0f, 109 / 255.0f, 104 / 255.0f, 1.0f); // #B76D68 (Primary buttons)
    ImVec4 text_color = ImVec4(224 / 255.0f, 224 / 255.0f, 224 / 255.0f, 1.0f); // #E0E0E0 (Text)

    style.Colors[ImGuiCol_WindowBg] = rich_black;
    style.Colors[ImGuiCol_ChildBg] = raisin_black;
    style.Colors[ImGuiCol_FrameBg] = raisin_black;
    style.Colors[ImGuiCol_FrameBgHovered] = indian_red;
    style.Colors[ImGuiCol_FrameBgActive] = indian_red;
    style.Colors[ImGuiCol_Button] = indian_red;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(indian_red.x * 1.2f, indian_red.y * 1.2f, indian_red.z * 1.2f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(indian_red.x * 0.8f, indian_red.y * 0.8f, indian_red.z * 0.8f, 1.0f);
    style.Colors[ImGuiCol_Text] = text_color;
    style.Colors[ImGuiCol_ScrollbarBg] = raisin_black;
    style.Colors[ImGuiCol_ScrollbarGrab] = onyx;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = indian_red;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(indian_red.x * 0.8f, indian_red.y * 0.8f, indian_red.z * 0.8f, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = raisin_black;
    style.Colors[ImGuiCol_TitleBgActive] = raisin_black;
    style.Colors[ImGuiCol_TitleBgCollapsed] = raisin_black;
    style.Colors[ImGuiCol_Header] = raisin_black;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(raisin_black.x * 1.2f, raisin_black.y * 1.2f, raisin_black.z * 1.2f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(raisin_black.x * 0.8f, raisin_black.y * 0.8f, raisin_black.z * 0.8f, 1.0f);
}

bool GUI::IsRunning() const {
    return _running;
}