#include "gui.hpp"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "../../image/image.hpp"

GUI gui = GUI();

void GUI::Init(GLFWwindow* window) {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.IniFilename = NULL;
    io.LogFilename = NULL;

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

    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_context);
    }

    glfwSwapBuffers(window);
}

void GUI::Render() {
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    #pragma region MainWindow
    ImGui::Begin("Maze Solver", &gui.running, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Image size: %ix%i", image.GetWidth(), image.GetHeight());

    if (ImGui::Button("Load Image")) {
        image.SelectImageFromFileDialog();
    }

    ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

    ImGui::End();
    #pragma endregion

    GLuint image_texture = image.GetTexture();
    if (image_texture) {
        image.RescaleToFit(800, 800); // Max size 800x800 and maintain aspect ratio
        ImGui::SetNextWindowSize(CalculateImageWindowSize(image.GetWidth(), image.GetHeight()));

        #pragma region ImageWindow
        ImGui::Begin("Selected Image", &gui.running, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::Image((void*)(intptr_t)image_texture, ImVec2(image.GetWidth(), image.GetHeight()));

        ImGui::End();
        #pragma endregion
    }
}

ImVec2 GUI::CalculateImageWindowSize(int image_width, int image_height) {
    ImGuiStyle& style = ImGui::GetStyle();

    // Calculate padding and other window elements
    float padding_x = style.WindowPadding.x * 2;
    float padding_y = style.WindowPadding.y * 2;
    float frame_padding = style.FramePadding.y * 2;
    float title_bar_height = ImGui::GetFrameHeight();

    // Add padding to the image size to get the total window size
    float window_width = image_width + padding_x;
    float window_height = image_height + padding_y + frame_padding + title_bar_height;

    return ImVec2(window_width, window_height);
}

void GUI::SetupImGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 1.0f;
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.WindowRounding = 11.5f;
    style.WindowBorderSize = 0.0f;
    style.WindowMinSize = ImVec2(20.0f, 20.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Right;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(20.0f, 3.4f);
    style.FrameRounding = 11.9f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(4.3f, 5.5f);
    style.ItemInnerSpacing = ImVec2(7.1f, 1.8f);
    style.CellPadding = ImVec2(12.1f, 9.2f);
    style.IndentSpacing = 0.0f;
    style.ColumnsMinSpacing = 4.9f;
    style.ScrollbarSize = 11.6f;
    style.ScrollbarRounding = 15.9f;
    style.GrabMinSize = 3.7f;
    style.GrabRounding = 20.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.27f, 0.32f, 0.45f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.10f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.80f, 0.50f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.19f, 0.20f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.14f, 0.16f, 0.21f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.13f, 0.15f, 0.19f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.16f, 0.18f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.16f, 0.18f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.27f, 0.57f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9f, 0.7f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.20f, 0.23f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.0f, 1.0f, 0.0f, 0.9f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}

bool GUI::IsRunning() {
    return this->running;
}