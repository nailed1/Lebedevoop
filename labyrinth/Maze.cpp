#include "Maze.h"

int Maze::getCell(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height)
        return grid[y][x];
    return WALL;
}

bool Maze::isWalkable(int x, int y) const {
    return getCell(x, y) != WALL;
}

void Maze::setCell(int x, int y, int type) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        grid[y][x] = type;
        if (type == START) { startCell = Cell(x, y); startSet = true; }
        if (type == END)   { endCell   = Cell(x, y); endSet   = true; }
    }
}
