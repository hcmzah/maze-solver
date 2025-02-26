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