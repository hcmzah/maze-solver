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
    std::vector<std::vector<int>> maze_grid(_height, std::vector<int>(_width, 0));

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            int index = (y * _width + x) * 4;
            unsigned char r = pixel_data[index];
            unsigned char g = pixel_data[index + 1];
            unsigned char b = pixel_data[index + 2];

            bool is_white = (r > 150 && g > 150 && b > 150);
            maze_grid[y][x] = is_white ? 1 : 0;
        }
    }

    int min_x = _width, max_x = 0;
    int min_y = _height, max_y = 0;

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            if (maze_grid[y][x] == 0) { // wall
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            if (x < min_x || x > max_x || y < min_y || y > max_y) {
                maze_grid[y][x] = 0;
            }
        }
    }

    return maze_grid;
}

std::pair<ImVec2, ImVec2> Image::CalculateMazeBoundingBox() const {
    int min_x = _width, min_y = _height;
    int max_x = 0, max_y = 0;

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            int index = (y * _width + x) * 4;
            const unsigned char& r = _image_data[index];
            const unsigned char& g = _image_data[index + 1];
            const unsigned char& b = _image_data[index + 2];

            bool is_walkable = (r > 150 && g > 150 && b > 150);
            if (!is_walkable) {
                min_x = std::min(min_x, x);
                min_y = std::min(min_y, y);
                max_x = std::max(max_x, x);
                max_y = std::max(max_y, y);
            }
        }
    }

    return { ImVec2(min_x, min_y), ImVec2(max_x, max_y) };
}


void Image::ApplyGreyscaleFilter() {
    if (_image_data.empty()) return;

    for (size_t i = 0; i < _image_data.size(); i += 4) {
        unsigned char r = _image_data[i];
        unsigned char g = _image_data[i + 1];
        unsigned char b = _image_data[i + 2];

        // Apply the luminance formula
        unsigned char grey = static_cast<unsigned char>(0.299f * r + 0.587f * g + 0.114f * b);

        _image_data[i] = grey;     // Red
        _image_data[i + 1] = grey; // Green
        _image_data[i + 2] = grey; // Blue
        // _image_data[i + 3] stays the same (Alpha)
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