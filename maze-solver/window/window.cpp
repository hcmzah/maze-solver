#include "window.hpp"
#include <iostream>

GLFWwindow* Window::CreateWindow(int width, int height, const char* title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // No window borders or controls
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // Transparent framebuffer

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

void Window::DestroyWindow(GLFWwindow* window) {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose(GLFWwindow* window) {
    return glfwWindowShouldClose(window);
}
