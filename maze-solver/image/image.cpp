#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <tinyfiledialogs.h>
#include <iostream>

Image image = Image();

bool Image::LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int channels;
    unsigned char* data = stbi_load(filename, out_width, out_height, &channels, 4); // Load image as RGBA

    if (data == nullptr) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return false;
    }

    glGenTextures(1, out_texture);
    glBindTexture(GL_TEXTURE_2D, *out_texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *out_width, *out_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Free image data
    stbi_image_free(data);

    return true;
}

void Image::CleanupTexture() {
    if (this->texture) {
        glDeleteTextures(1, &this->texture);
        this->texture = 0;
    }
}

void Image::SelectImageFromFileDialog() {
    const char* file_filter[3] = { "*.png", "*.jpg", "*.jpeg"};

    const char* file_path_name = tinyfd_openFileDialog(
        "Choose Image",   // Title
        "",               // Default path
        3,                // Number of filter patterns
        file_filter,      // Filter patterns
        NULL,             // Single filter description
        0                 // Allow multi-selection? (0 = no)
    );

    // Load the image
    if (file_path_name) {
        CleanupTexture(); // Free the previous texture

        image.LoadTextureFromFile(file_path_name, &this->texture, &this->width, &this->height);
    }
}

GLuint Image::GetTexture() {
    return this->texture;
}

int Image::GetWidth() {
    return this->width;
}

int Image::GetHeight() {
    return this->height;
}