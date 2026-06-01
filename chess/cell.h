#ifndef CELL_H
#define CELL_H

#include "figures.h"

// Клетка доски. Хранит координаты и указатель на фигуру.
//
// Семантика владения: Cell хранит указатель, но delete не вызывает.
// За удаление отвечает Table — через removePieceAt и деструктор.
// Это позволяет wouldLeaveInCheck временно переставлять указатели без удаления фигур.
class Cell {
private:
    int     row, col;
    Figure* figure;
public:
    Cell() : row(0), col(0), figure(NULL) {}
    Cell(int r, int c) : row(r), col(c), figure(NULL) {}

    Figure* getFigure() const { return figure; }
    void    setFigure(Figure* f) { figure = f; }
    bool    isEmpty() const { return figure == NULL; }
    int     getRow() const { return row; }
    int     getCol() const { return col; }
};

#endif // CELL_H
