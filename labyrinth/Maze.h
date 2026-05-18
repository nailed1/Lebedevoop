#ifndef MAZE_H
#define MAZE_H

#include "Cell.h"
#include <vector>

class Maze {
public:
    enum CellType { EMPTY = 0, WALL = 1, START = 2, END = 3 };

    Maze(int w, int h)
        : width(w), height(h), startSet(false), endSet(false),
          grid(h, std::vector<int>(w, EMPTY)) {}

    int  getWidth()  const { return width; }
    int  getHeight() const { return height; }
    Cell getStart()  const { return startCell; }
    Cell getEnd()    const { return endCell; }
    bool hasStart()  const { return startSet; }
    bool hasEnd()    const { return endSet; }

    int  getCell(int x, int y)      const;
    int  getCell(const Cell& c)     const { return getCell(c.x, c.y); }
    bool isWalkable(int x, int y)   const;
    bool isWalkable(const Cell& c)  const { return isWalkable(c.x, c.y); }
    void setCell(int x, int y, int type);

private:
    int  width, height;
    bool startSet, endSet;
    Cell startCell, endCell;
    std::vector<std::vector<int>> grid;
};

#endif
