#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <imgui.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class Image {
public:
	bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height);
	void CleanupTexture();
	void SelectImageFromFileDialog();
	void RescaleToFit(int max_width, int max_height);
	std::vector<unsigned char> ExtractPixelData();
	std::vector<std::vector<int>> ConvertToMazeGrid();

	std::vector<ImVec2> AdjustPathToCenters(const std::vector<ImVec2>& path, float cellWidth, float cellHeight);
	std::vector<ImVec2> SolveMazeWithDijkstra(const std::vector<std::vector<int>>& maze, ImVec2 startPos, ImVec2 endPos);
	std::vector<ImVec2> CalculatePrecisePath(const std::vector<ImVec2>& path, float cellWidth, float cellHeight);
	std::vector<ImVec2> CalculateSharpPath(const std::vector<ImVec2>& path, float cellWidth, float cellHeight);


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
