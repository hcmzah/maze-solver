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

    enum class Alg { 
        Dijkstra = 0, 
        AStar
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
    double _solve_time;
    bool _show_popup;

    void SetupImGuiStyle();
    void RenderControlsPanel();
    void RenderImagePanel();
    void RenderOverlay(const ImVec2& image_pos, float img_width, float img_height);
    void HandleImageClick(const ImVec2& image_pos, float displayed_width, float displayed_height);
    void HandleZoom();
    void HandlePanning();
    ImVec2 GetCenteredPosition(const ImVec2& avail_size, float img_width, float img_height);

    float _path_alpha;
    float _path_thickness;
    int _marker_size;
    ImVec4 _path_color;
    ImVec4 _start_marker_color;
    ImVec4 _end_marker_color;

    float _zoom;
    ImVec2 _pan_offset;

    bool _bounding_box;
    ImVec4 _bounding_box_color;

    Alg _algorithm;

    void RenderAdvancedSettings();
};

#endif // GUI_HPP