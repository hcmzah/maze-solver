#include "pathfinder.hpp"

#include <queue>
#include <limits>
#include <algorithm>

Pathfinder::Node::Node(int x, int y, int distance)
    : x(x), y(y), distance(distance) {}

bool Pathfinder::Node::operator>(const Pathfinder::Node& other) const {
    return distance > other.distance;
}

std::vector<ImVec2> Pathfinder::SolveMazeWithDijkstra(const std::vector<std::vector<int>>& maze, ImVec2 start_pos, ImVec2 end_pos) {
    int rows = maze.size();
    int cols = maze[0].size();

    std::vector<std::vector<int>> distance(rows, std::vector<int>(cols, std::numeric_limits<int>::max()));
    std::vector<std::vector<ImVec2>> previous(rows, std::vector<ImVec2>(cols, ImVec2(-1, -1)));
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> priority_queue;

    int start_x = static_cast<int>(start_pos.x);
    int start_y = static_cast<int>(start_pos.y);
    int end_x = static_cast<int>(end_pos.x);
    int end_y = static_cast<int>(end_pos.y);

    priority_queue.push(Node(start_x, start_y, 0));
    distance[start_y][start_x] = 0;

    const int dx[] = { 0, 1, 0, -1 };
    const int dy[] = { -1, 0, 1, 0 };

    while (!priority_queue.empty()) {
        Node current = priority_queue.top();
        priority_queue.pop();

        if (current.x == end_x && current.y == end_y) {
            std::vector<ImVec2> path;
            ImVec2 position = ImVec2(end_x, end_y);

            while (position.x != -1 && position.y != -1) {
                path.push_back(position);
                position = previous[static_cast<int>(position.y)][static_cast<int>(position.x)];
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int next_x = current.x + dx[i];
            int next_y = current.y + dy[i];

            if (next_x >= 0 && next_y >= 0 && next_x < cols && next_y < rows && maze[next_y][next_x] == 1) {
                int new_distance = distance[current.y][current.x] + 1;

                if (new_distance < distance[next_y][next_x]) {
                    distance[next_y][next_x] = new_distance;
                    priority_queue.push(Node(next_x, next_y, new_distance));
                    previous[next_y][next_x] = ImVec2(current.x, current.y);
                }
            }
        }
    }

    return {}; // No path found
}