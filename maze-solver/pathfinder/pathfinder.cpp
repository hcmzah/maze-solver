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

Pathfinder::ANode::ANode(int x, int y, float g, float f)
    : x(x), y(y), g(g), f(f) {
}

bool Pathfinder::ANode::operator>(ANode const& o) const {
    return f > o.f;
}

std::vector<ImVec2> Pathfinder::SolveMazeWithAStar(const std::vector<std::vector<int>>& maze, ImVec2 start_pos, ImVec2 end_pos)
{
    int rows = maze.size();
    int cols = maze[0].size();

    int sx = int(start_pos.x), sy = int(start_pos.y);
    int ex = int(end_pos.x), ey = int(end_pos.y);

    std::vector<std::vector<float>> g(rows, std::vector<float>(cols, std::numeric_limits<float>::infinity()));
    std::vector<std::vector<ImVec2>> prev(rows, std::vector<ImVec2>(cols, ImVec2(-1, -1)));

    auto heuristic = [&](int x, int y) {
        return float(std::abs(x - ex) + std::abs(y - ey));
    };

    std::priority_queue<ANode, std::vector<ANode>, std::greater<ANode>> open_set;

    g[sy][sx] = 0.0f;
    open_set.emplace(sx, sy, 0.0f, heuristic(sx, sy));

    int dx[4] = { 0, 1, 0, -1 };
    int dy[4] = { -1, 0, 1, 0 };

    while (!open_set.empty()) {
        ANode cur = open_set.top(); open_set.pop();

        if (cur.x == ex && cur.y == ey) {
            std::vector<ImVec2> path;
            ImVec2 p{ float(ex), float(ey) };

            while (p.x >= 0 && p.y >= 0) {
                path.push_back(p);
                p = prev[int(p.y)][int(p.x)];
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        for (int i = 0; i < 4; ++i) {
            int nx = cur.x + dx[i];
            int ny = cur.y + dy[i];

            if (nx < 0 || ny < 0 || nx >= cols || ny >= rows)
                continue;

            if (maze[ny][nx] == 0)
                continue;

            float tentative_g = cur.g + 1.0f;
            if (tentative_g < g[ny][nx]) {
                g[ny][nx] = tentative_g;
                prev[ny][nx] = ImVec2(float(cur.x), float(cur.y));

                float f = tentative_g + heuristic(nx, ny);
                open_set.emplace(nx, ny, tentative_g, f);
            }
        }
    }

    return {}; // no path
}