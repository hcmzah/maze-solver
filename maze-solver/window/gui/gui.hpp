#ifndef GUI_HPP
#define GUI_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <vector>

class GUI {
public:
    enum class PositionMode {
        None = 0,
        SetStart,
        SetEnd
    };

    GUI();
    ~GUI();

    void Init(GLFWwindow* window);
    void Shutdown();
    void BeginFrame();
    void EndFrame(GLFWwindow* window);
    void Render();
    bool IsRunning() const;

private:
    bool _running;
    PositionMode _current_mode;
    std::vector<ImVec2> _solved_path;
    std::vector<std::vector<int>> _maze;
    GLuint _image_texture;

    void SetupImGuiStyle();
    void RenderControlsPanel();
    void RenderImagePanel();
    void HandleImageClick(const ImVec2& image_pos, float displayed_width, float displayed_height);
};

#endif // GUI_HPP