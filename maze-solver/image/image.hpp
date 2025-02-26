#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <vector>

class Image {
public:
    Image();
    ~Image();

    bool LoadTextureFromFile(const std::string& filename);
    void CleanupTexture();
    void SelectImageFromFileDialog();

    std::vector<std::vector<int>> ConvertToMazeGrid();

    GLuint GetTexture() const;
    ImVec2 GetStartPosition() const;
    ImVec2 GetEndPosition() const;
    int GetWidth() const;
    int GetHeight() const;

    void SetStartPosition(ImVec2 start_pos);
    void SetEndPosition(ImVec2 end_pos);

private:
    void UpdateTexture();
    std::vector<unsigned char> ExtractPixelData();

    GLuint _texture;
    int _width;
    int _height;
    ImVec2 _start_pos;
    ImVec2 _end_pos;

    std::vector<unsigned char> _image_data;
};

#endif // IMAGE_HPP