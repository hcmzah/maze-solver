#include <queue>
#include <vector>
#include <limits>
#include "image/image.hpp"

struct Node {
    int x, y, dist;
    Node(int x, int y, int dist) : x(x), y(y), dist(dist) {}
    bool operator>(const Node& other) const {
        return dist > other.dist;
    }
};

std::vector<ImVec2> Image::SolveMazeWithDijkstra(const std::vector<std::vector<int>>& maze, ImVec2 startPos, ImVec2 endPos) {
    int rows = maze.size();
    int cols = maze[0].size();

    std::vector<std::vector<int>> dist(rows, std::vector<int>(cols, std::numeric_limits<int>::max()));
    std::vector<std::vector<ImVec2>> prev(rows, std::vector<ImVec2>(cols, ImVec2(-1, -1)));
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    int startX = static_cast<int>(startPos.x);
    int startY = static_cast<int>(startPos.y);
    int endX = static_cast<int>(endPos.x);
    int endY = static_cast<int>(endPos.y);

    pq.push(Node(startX, startY, 0));
    dist[startY][startX] = 0;

    const int dx[] = { 0, 1, 0, -1 };
    const int dy[] = { -1, 0, 1, 0 };

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (curr.x == endX && curr.y == endY) {
            std::vector<ImVec2> path;
            ImVec2 p = ImVec2(endX, endY);
            while (p.x != -1 && p.y != -1) {
                path.push_back(p);
                p = prev[static_cast<int>(p.y)][static_cast<int>(p.x)];
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            if (nx >= 0 && ny >= 0 && nx < cols && ny < rows && maze[ny][nx] == 1) {
                int newDist = dist[curr.y][curr.x] + 1;
                if (newDist < dist[ny][nx]) {
                    dist[ny][nx] = newDist;
                    pq.push(Node(nx, ny, newDist));
                    prev[ny][nx] = ImVec2(curr.x, curr.y);
                }
            }
        }
    }

    return {}; // No path found
}
