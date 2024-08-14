#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <GLFW/glfw3.h>
#include <string>

class Image {
public:
	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	void CleanupTexture();
	void SelectImageFromFileDialog();

	GLuint GetTexture();
	int GetWidth();
	int GetHeight();

private:
	GLuint texture;
	int width;
	int height;
};

extern Image image;

#endif // IMAGE_HPP
