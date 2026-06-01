#ifndef TABLE_H
#define TABLE_H

#include "cell.h"
#include <vector>

// Шахматная доска.
//
// Владение фигурами: cells[8][8] — единственный владелец всех Figure*.
// Взятие: removePieceAt = O(1) delete. Копирование: 64 clone(), без лишних векторов.
class Table {
private:
    // mutable: wouldLeaveInCheck — const-метод, но временно двигает фигуры
    mutable Cell cells[8][8];

    Color::E currentTurn;
    Position enPassantTarget; // (-1,-1) если взятие на проходе недоступно
    bool     gameOver;
    Color::E winner;
    bool     isDraw;

    bool inBounds(int r, int c) const { return r >= 0 && r < 8 && c >= 0 && c < 8; }

    bool wouldLeaveInCheck(int fr, int fc, int tr, int tc) const;
    void addPiece(Figure* fig, int r, int c);
    void removePieceAt(int r, int c);

public:
    Table();
    Table(const Table& other); // глубокое копирование: clone() для каждой фигуры
    ~Table();

    void initialize();
    void display() const;

    Figure*  getFigureAt(int r, int c) const;
    bool     isInBounds(int r, int c) const { return inBounds(r, c); }
    Position getEnPassantTarget() const { return enPassantTarget; }

    bool isSquareAttacked(int r, int c, Color::E byColor) const;
    bool isInCheck(Color::E color) const;

    std::vector<Position> getValidMoves(int r, int c) const;

    bool makeMove(int fr, int fc, int tr, int tc,
                  PieceType::E promotion = PieceType::Queen);

    void clearBoard();
    void placePiece(PieceType::E t, Color::E c, int row, int col);

    Color::E getCurrentTurn() const { return currentTurn; }
    bool     isGameOver()     const { return gameOver; }
    Color::E getWinner()      const { return winner; }
    bool     getIsDraw()      const { return isDraw; }
};

#endif // TABLE_H
