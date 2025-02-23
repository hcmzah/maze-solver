#include "gui.hpp"
#include "../../image/image.hpp"
#include "../../pathfinder/pathfinder.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

Image image;
Pathfinder pathfinder;

GUI::GUI() {
    _running = true;
    _current_mode = PositionMode::None;
    _image_texture = 0;
}

GUI::~GUI() {
    Shutdown();
}

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

void GUI::Render() {
    ImGui::Begin("Maze Solver", &_running, ImGuiWindowFlags_NoCollapse);
    ImGui::Columns(2, nullptr, true);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.3f);

    RenderControlsPanel();
    ImGui::NextColumn();
    RenderImagePanel();

    ImGui::End();
}

void GUI::RenderControlsPanel() {
    ImGui::BeginChild("Controls");
    ImGui::Text("github.com/hcmzah/maze-solver");
    ImGui::Separator();

    ImGui::Text("Image Size: %dpx x %dpx", image.GetWidth(), image.GetHeight());

    if (ImGui::Button("Load Image", ImVec2(-1, 35))) {
        image.SelectImageFromFileDialog();
        _image_texture = image.GetTexture();
        if (_image_texture == 0) {
            std::cerr << "[ERROR] Image texture is null!" << std::endl;
        }
        else {
            _maze = image.ConvertToMazeGrid();
            std::cout << "[INFO] Maze grid loaded: " << _maze.size() << "x" << _maze[0].size() << "\n";
        }
    }

    if (ImGui::Button("Reset Image", ImVec2(-1, 35))) {
        image.ReloadOriginalImage();
    }

    ImGui::Separator();
    ImGui::Text("Set Positions:");

    if (ImGui::Button("Choose Start", ImVec2(-1, 35))) {
        _current_mode = PositionMode::SetStart;
    }
    ImGui::Text("(%.f, %.f)", image.GetStartPosition().x, image.GetStartPosition().y);

    if (ImGui::Button("Choose End", ImVec2(-1, 35))) {
        _current_mode = PositionMode::SetEnd;
    }
    ImGui::Text("(%.f, %.f)", image.GetEndPosition().x, image.GetEndPosition().y);

    if (ImGui::Button("Solve Maze")) {
        _solved_path.clear();
        image.ReloadOriginalImage();
        _solved_path = pathfinder.SolveMazeWithDijkstra(_maze, image.GetStartPosition(), image.GetEndPosition());

        if (!_solved_path.empty()) {
            std::cout << "[SUCCESS] Path found with " << _solved_path.size() << " steps.\n";
            image.DrawPathOnImage(_solved_path);
        }
        else {
            std::cerr << "[ERROR] No path found!\n";
        }
    }

    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::EndChild();
}

void GUI::RenderImagePanel() {
    ImGui::BeginChild("ImagePanel");

    if (!_maze.empty() && !_maze[0].empty()) {
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        float aspect_ratio = (float)image.GetWidth() / image.GetHeight();
        float img_width = available_size.x;
        float img_height = img_width / aspect_ratio;

        if (img_height > available_size.y) {
            img_height = available_size.y;
            img_width = img_height * aspect_ratio;
        }

        ImGui::SetCursorPosX((available_size.x - img_width) * 0.5f);
        ImVec2 image_pos = ImGui::GetCursorScreenPos();
        ImGui::Image((void*)(intptr_t)_image_texture, ImVec2(img_width, img_height));

        HandleImageClick(image_pos, img_width, img_height);
        image.DrawMarkersOnImage();
    }
    else {
        ImGui::Text("No image loaded...");
    }

    ImGui::EndChild();
}

void GUI::HandleImageClick(const ImVec2& image_pos, float displayed_width, float displayed_height) {
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        ImVec2 mouse_pos = ImGui::GetMousePos();

        float img_x = mouse_pos.x - image_pos.x;
        float img_y = mouse_pos.y - image_pos.y;

        int grid_x = static_cast<int>(img_x * _maze[0].size() / displayed_width);
        int grid_y = static_cast<int>(img_y * _maze.size() / displayed_height);

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
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.FrameRounding = 5.0f;
    style.GrabRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.FramePadding = ImVec2(10, 5);
    style.ItemSpacing = ImVec2(8, 6);
    style.WindowPadding = ImVec2(12, 12);
    style.ScrollbarSize = 15.0f;
    style.FrameBorderSize = 1.0f;
}

bool GUI::IsRunning() const {
    return _running;
}
