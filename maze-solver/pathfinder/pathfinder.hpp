#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <vector>
#include <imgui.h>

class Pathfinder {
public:
    static std::vector<ImVec2> SolveMazeWithDijkstra(const std::vector<std::vector<int>>& maze, ImVec2 start_pos, ImVec2 end_pos);

    static std::vector<ImVec2> SolveMazeWithAStar(const std::vector<std::vector<int>>& maze, ImVec2 start_pos, ImVec2 end_pos);

private:
    struct Node {
        int x, y, distance;
        Node(int x, int y, int distance);
        bool operator>(const Node& other) const;
    };

    struct ANode {
        int x, y;
        float g, f;
        ANode(int x, int y, float g, float f);
        bool operator>(ANode const& o) const;
    };
};

#endif // PATHFINDER_HPP