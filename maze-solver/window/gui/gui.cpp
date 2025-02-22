#include "gui.hpp"

#include "../../image/image.hpp"
#include "../../pathfinder/pathfinder.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

GUI::GUI() {
    _running = true;
    _current_mode = PositionMode::None;
    _image_texture = 0;
}

GUI::~GUI() {
    Shutdown();
}

Image image;
Pathfinder pathfinder;

void GUI::Init(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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

std::vector<ImVec2> solvedPath;
std::vector<std::vector<int>> maze;

void GUI::Render() {
    RenderMainWindow();

    if (_image_texture) {
        image.DrawMarkersOnImage();
        if (!_solved_path.empty()) {
            image.DrawPathOnImage(_solved_path);
        }

        RenderImageWindow();
    }
}

void GUI::RenderMainWindow() {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
    ImGui::Begin("Maze Solver", &_running, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Image size: %dpx x %dpx", image.GetWidth(), image.GetHeight());

    if (ImGui::Button("Load Image")) {
        image.SelectImageFromFileDialog();
        _image_texture = image.GetTexture();

        if (_image_texture == 0) {
            std::cerr << "[ERROR] Image texture is null!" << std::endl;
        }
        else {
            _maze = image.ConvertToMazeGrid();
        }
    }

    if (_image_texture) {
        if (ImGui::Button("Choose Start Position")) _current_mode = PositionMode::SetStart;
        ImGui::SameLine();
        ImGui::Text("(%.f, %.f)", image.GetStartPosition().x, image.GetStartPosition().y);

        if (ImGui::Button("Choose End Position")) _current_mode = PositionMode::SetEnd;
        ImGui::SameLine();
        ImGui::Text("(%.f, %.f)", image.GetEndPosition().x, image.GetEndPosition().y);

        if (ImGui::Button("Solve Maze")) {
            _solved_path.clear();
            image.ReloadOriginalImage();

            _solved_path = pathfinder.SolveMazeWithDijkstra(_maze, image.GetStartPosition(), image.GetEndPosition());

            if (!_solved_path.empty()) {
                image.DrawPathOnImage(_solved_path);
            }
        }
    }

    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
    ImGui::End();
}

void GUI::RenderImageWindow() {
    ImGui::Begin("Selected Image", &_running, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    if (!_maze.empty() && !_maze[0].empty()) {
        ImVec2 image_pos = ImGui::GetCursorScreenPos();
        ImGui::Image((void*)(intptr_t)_image_texture, ImVec2(image.GetWidth(), image.GetHeight()));
        HandleImageClick(image_pos);
    }

    ImGui::End();
}

void GUI::HandleImageClick(const ImVec2& image_pos) {
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();

        float img_x = mouse_pos.x - image_pos.x;
        float img_y = mouse_pos.y - image_pos.y;

        int grid_x = static_cast<int>(img_x / (image.GetWidth() / static_cast<float>(_maze[0].size())));
        int grid_y = static_cast<int>(img_y / (image.GetHeight() / static_cast<float>(_maze.size())));

        if (grid_x >= 0 && grid_y >= 0 && grid_x < _maze[0].size() && grid_y < _maze.size()) {
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
}

bool GUI::IsRunning() const {
    return _running;
}