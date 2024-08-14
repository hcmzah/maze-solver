#include "window/window.hpp"
#include "window/gui/gui.hpp"

int main() {
    GLFWwindow* window = Window::CreateWindow(1, 1, "Maze Solver");
    if (!window) {
        return -1;
    }

    GUI::Init(window);

    while (!Window::ShouldClose(window) && show_window) {
        Window::PollEvents();

        GUI::BeginFrame();
        GUI::Render();
        GUI::EndFrame(window);
    }

    GUI::Shutdown();
    Window::DestroyWindow(window);

    return 0;
}