#include "window/window.hpp"
#include "window/gui/gui.hpp"

int main() {

    Window window;
    GLFWwindow* glf_window = window.CreateWindow(1, 1, "Maze Solver");
    if (!glf_window) {
        return -1;
    }

    GUI gui;
    gui.Init(glf_window);

    while (!window.ShouldClose(glf_window) && gui.IsRunning()) {
        window.PollEvents();

        gui.BeginFrame();
        gui.Render();
        gui.EndFrame(glf_window);
    }

    gui.Shutdown();
    window.DestroyWindow(glf_window);

    return 0;
}