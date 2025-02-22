#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Image {
public:
    Image();
    ~Image();

    bool LoadTextureFromFile(const std::string& filename);
    void CleanupTexture();
    void SelectImageFromFileDialog();

    void DrawMarkersOnImage();
    void DrawPathOnImage(const std::vector<ImVec2>& path, float thickness = 1.0f);

    void ReloadOriginalImage();
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
    void SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
    std::vector<unsigned char> ExtractPixelData();

    GLuint _texture;
    int _width;
    int _height;
    ImVec2 _start_pos;
    ImVec2 _end_pos;

    std::vector<unsigned char> _image_data;
    std::vector<unsigned char> _original_image_data;
};

#endif // IMAGE_HPP