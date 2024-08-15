#ifndef GUI_HPP
#define GUI_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>

class GUI {
public:
    enum PositionMode {
        None = 0,
        SetStart,
        SetEnd
    };

    static void Init(GLFWwindow* window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame(GLFWwindow* window);
    static void Render();
    static void SetupImGuiStyle();

    static ImVec2 CalculateImageWindowSize(int image_width, int image_height);

    static void HandleImageClick(const ImVec2& image_pos, const ImVec2& image_size, PositionMode& current_mode);
    static void DrawMarkers(ImDrawList* draw_list, const ImVec2& image_pos, const ImVec2& start_pos, const ImVec2& end_pos);

    bool IsRunning();

private:
    bool running = true;
};

extern GUI gui;

#endif // GUI_HPP
