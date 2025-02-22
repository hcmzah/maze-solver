#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include <vector>
#include <imgui.h>

class Pathfinder {
public:
    static std::vector<ImVec2> SolveMazeWithDijkstra(const std::vector<std::vector<int>>& maze, ImVec2 start_pos, ImVec2 end_pos);

private:
    struct Node {
        int x, y, distance;
        Node(int x, int y, int distance);
        bool operator>(const Node& other) const;
    };
};

#endif // PATHFINDER_HPP