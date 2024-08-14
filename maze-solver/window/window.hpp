#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

class Window {
public:
    static GLFWwindow* CreateWindow(int width, int height, const char* title);
    static void DestroyWindow(GLFWwindow* window);
    static void PollEvents();
    static bool ShouldClose(GLFWwindow* window);
};

#endif // WINDOW_HPP
