#include "MazeSolver.h"
#include <algorithm>
#include <queue>

static const int dx[] = {0, 0, -1, 1};
static const int dy[] = {-1, 1,  0, 0};

Path MazeSolver::unroll(const std::vector<std::vector<int>>& dist,
                        const Cell& start, const Cell& end) const {
    Path result;
    Cell curr = end;
    result.cells.push_back(curr);

    const int h = static_cast<int>(dist.size());
    const int w = static_cast<int>(dist[0].size());

    // Идём от финиша к старту по убывающим расстояниям
    while (curr != start) {
        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];
            if (nx >= 0 && nx < w && ny >= 0 && ny < h &&
                dist[ny][nx] == dist[curr.y][curr.x] - 1) {
                curr = Cell(nx, ny);
                result.cells.push_back(curr);
                break;
            }
        }
    }

    std::reverse(result.cells.begin(), result.cells.end());
    return result;
}

Path MazeSolver::shortestPath() {
    const int w      = maze.getWidth();
    const int h      = maze.getHeight();
    const Cell start = maze.getStart();
    const Cell end   = maze.getEnd();

    // dist[y][x] = кратчайшее расстояние от старта, -1 = не посещено
    std::vector<std::vector<int>> dist(h, std::vector<int>(w, -1));
    std::queue<Cell> q;

    dist[start.y][start.x] = 0;
    q.push(start);

    while (!q.empty()) {
        Cell curr = q.front();
        q.pop();

        if (curr == end)
            return unroll(dist, start, end);

        for (int i = 0; i < 4; ++i) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];
            if (maze.isWalkable(nx, ny) && dist[ny][nx] == -1) {
                dist[ny][nx] = dist[curr.y][curr.x] + 1;
                q.push(Cell(nx, ny));
            }
        }
    }

    return Path(); // путь не найден
}
