#ifndef GUI_HPP
#define GUI_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>

class GUI {
public:
    static void Init(GLFWwindow* window);
    static void Shutdown();
    static void BeginFrame();
    static void EndFrame(GLFWwindow* window);
    static void Render();
    static void SetupImGuiStyle();
};

extern bool show_window;

#endif // GUI_HPP
