#ifndef MAZESOLVER_H
#define MAZESOLVER_H

#include "Maze.h"
#include "Cell.h"
#include <vector>

struct Path {
    std::vector<Cell> cells;
    bool found() const { return !cells.empty(); }
};

class MazeSolver {
public:
    explicit MazeSolver(const Maze& maze) : maze(maze) {}
    Path shortestPath();

private:
    const Maze& maze;
    Path unroll(const std::vector<std::vector<int>>& dist,
                const Cell& start, const Cell& end) const;
};

#endif
