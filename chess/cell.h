#pragma once
#include "figures.h"

class Cell {
private:
    int row, col;
    Figure* figure; // non-owning pointer
public:
    Cell() : row(0), col(0), figure(nullptr) {}
    Cell(int r, int c) : row(r), col(c), figure(nullptr) {}

    Figure* getFigure() const { return figure; }
    void setFigure(Figure* f) { figure = f; }
    bool isEmpty() const { return figure == nullptr; }
    int getRow() const { return row; }
    int getCol() const { return col; }
};
