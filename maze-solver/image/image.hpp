#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <string>

class Image {
public:
	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	void CleanupTexture();
	void SelectImageFromFileDialog();
	void RescaleToFit(int max_width, int max_height);

	GLuint GetTexture();
	int GetWidth();
	int GetHeight();
	ImVec2 GetSize();
	ImVec2 GetStartPosition();
	ImVec2 GetEndPosition();

	void SetWidth(int width);
	void SetHeight(int height);
	void SetStartPosition(ImVec2 start_pos);
	void SetEndPosition(ImVec2 end_pos);

private:
	GLuint texture;
	int width;
	int height;
	ImVec2 start_pos;
	ImVec2 end_pos;
};

extern Image image;

#endif // IMAGE_HPP
