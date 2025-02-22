#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <GLFW/glfw3.h>

class Window {
public:
    GLFWwindow* CreateWindow(int width, int height, const char* title);
    void DestroyWindow(GLFWwindow* window);
    void PollEvents();
    bool ShouldClose(GLFWwindow* window);
};

#endif // WINDOW_HPP