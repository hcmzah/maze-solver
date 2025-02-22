#include "image.hpp"

#include <tinyfiledialogs.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Image::Image() {
    _width = 0;
    _height = 0;
    _texture = 0;
    _start_pos = ImVec2(0, 0);
    _end_pos = ImVec2(0, 0);
}

Image::~Image() {
    CleanupTexture();
}

bool Image::LoadTextureFromFile(const std::string& filename) {
    int channels;
    unsigned char* data = stbi_load(filename.c_str(), &_width, &_height, &channels, 4); // Load as RGBA

    if (!data) {
        std::cerr << "[ERROR] Failed to load image: " << filename << std::endl;
        return false;
    }

    glGenTextures(1, &_texture);
    if (_texture == 0) {
        std::cerr << "[ERROR] glGenTextures failed!" << std::endl;
        stbi_image_free(data);
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);

    _image_data.assign(data, data + (_width * _height * 4));
    _original_image_data = _image_data;

    stbi_image_free(data);
    return true;
}

void Image::CleanupTexture() {
    if (_texture) {
        glDeleteTextures(1, &_texture);
        _texture = 0;
    }
}

void Image::SelectImageFromFileDialog() {
    const char* file_filter[3] = { "*.png", "*.jpg", "*.jpeg" };
    const char* file_path = tinyfd_openFileDialog("Choose Image", "", 3, file_filter, nullptr, 0);

    if (file_path) {
        CleanupTexture();
        LoadTextureFromFile(file_path);
    }
}

void Image::UpdateTexture() {
    if (_image_data.empty()) return;

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _image_data.data());
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Image::SetPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (x < 0 || y < 0 || x >= _width || y >= _height) return;
    int index = (y * _width + x) * 4;
    _image_data[index] = r;
    _image_data[index + 1] = g;
    _image_data[index + 2] = b;
    _image_data[index + 3] = a;
}

void Image::ReloadOriginalImage() {
    if (!_original_image_data.empty()) {
        _image_data = _original_image_data;
        UpdateTexture();
    }
    else {
        std::cerr << "[ERROR] Original image data not available!" << std::endl;
    }
}

std::vector<unsigned char> Image::ExtractPixelData() {
    if (_texture == 0) {
        std::cerr << "[ERROR] Texture is not initialized!" << std::endl;
        return {};
    }

    glBindTexture(GL_TEXTURE_2D, _texture);
    int image_size = _width * _height * 4; // 4 bytes per pixel (RGBA)
    std::vector<unsigned char> pixel_data(image_size);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    return pixel_data;
}

std::vector<std::vector<int>> Image::ConvertToMazeGrid() {
    std::vector<unsigned char> pixel_data = ExtractPixelData();
    std::vector<std::vector<int>> maze_grid(_height, std::vector<int>(_width));

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            int index = (y * _width + x) * 4;
            unsigned char r = pixel_data[index];
            unsigned char g = pixel_data[index + 1];
            unsigned char b = pixel_data[index + 2];

            // Treat white as walkable (1) and black as walls (0)
            maze_grid[y][x] = (r > 200 && g > 200 && b > 200) ? 1 : 0;
        }
    }

    return maze_grid;
}

void Image::DrawMarkersOnImage() {
    if (_image_data.empty()) return;

    unsigned char green[] = { 0, 255, 0, 255 }; // Start marker color
    unsigned char red[] = { 255, 0, 0, 255 };   // End marker color
    int marker_size = 5;

    int start_x = static_cast<int>(_start_pos.x);
    int start_y = static_cast<int>(_start_pos.y);
    int end_x = static_cast<int>(_end_pos.x);
    int end_y = static_cast<int>(_end_pos.y);

    for (int dy = -marker_size; dy <= marker_size; ++dy) {
        for (int dx = -marker_size; dx <= marker_size; ++dx) {
            if (dx * dx + dy * dy <= marker_size * marker_size) {
                SetPixel(start_x + dx, start_y + dy, green[0], green[1], green[2], green[3]);
                SetPixel(end_x + dx, end_y + dy, red[0], red[1], red[2], red[3]);
            }
        }
    }
    UpdateTexture();
}

void Image::DrawPathOnImage(const std::vector<ImVec2>& path, float thickness) {
    if (_image_data.empty()) return;

    unsigned char blue[] = { 0, 0, 255, 255 };

    for (size_t i = 1; i < path.size(); ++i) {
        int x1 = static_cast<int>(path[i - 1].x);
        int y1 = static_cast<int>(path[i - 1].y);
        int x2 = static_cast<int>(path[i].x);
        int y2 = static_cast<int>(path[i].y);

        int dx = std::abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -std::abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (true) {
            for (int tx = -thickness; tx <= thickness; ++tx) {
                for (int ty = -thickness; ty <= thickness; ++ty) {
                    SetPixel(x1 + tx, y1 + ty, blue[0], blue[1], blue[2], blue[3]);
                }
            }
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }
    UpdateTexture();
}

GLuint Image::GetTexture() const {
    return _texture;
}

int Image::GetWidth() const {
    return _width;
}

int Image::GetHeight() const {
    return _height;
}

ImVec2 Image::GetStartPosition() const {
    return _start_pos;
}

ImVec2 Image::GetEndPosition() const {
    return _end_pos;
}

void Image::SetStartPosition(ImVec2 start_pos) {
    _start_pos = start_pos;
}

void Image::SetEndPosition(ImVec2 end_pos) {
    _end_pos = end_pos;
}