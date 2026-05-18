#pragma once
#include "cell.h"
#include <vector>
#include <memory>

class Table {
private:
    mutable Cell cells[8][8]; // mutable to allow save/restore in const simulation methods
    std::vector<std::unique_ptr<Figure>> pieces;   // owns active pieces
    std::vector<std::unique_ptr<Figure>> captured; // owns captured pieces

    Color currentTurn;
    Position enPassantTarget; // {-1,-1} if none
    bool gameOver;
    Color winner;
    bool isDraw;

    bool inBounds(int r, int c) const { return r >= 0 && r < 8 && c >= 0 && c < 8; }

    bool wouldLeaveInCheck(int fr, int fc, int tr, int tc) const;

    void addPiece(std::unique_ptr<Figure> fig, int r, int c);
    void removePieceAt(int r, int c);

public:
    Table();
    Table(const Table& other);   // deep copy — used by the AI search tree
    ~Table() = default;

    void initialize();
    void display() const;

    Figure* getFigureAt(int r, int c) const;
    bool isInBounds(int r, int c) const { return inBounds(r, c); }
    Position getEnPassantTarget() const { return enPassantTarget; }

    bool isSquareAttacked(int r, int c, Color byColor) const;
    bool isInCheck(Color color) const;

    std::vector<Position> getValidMoves(int r, int c) const;

    // Returns false if move is illegal. Promotion defaults to Queen.
    bool makeMove(int fr, int fc, int tr, int tc, PieceType promotion = PieceType::Queen);

    bool isPawnPromotion(int fr, int fc, int tr, int tc) const;

    Color getCurrentTurn() const { return currentTurn; }
    bool isGameOver() const { return gameOver; }
    Color getWinner() const { return winner; }
    bool getIsDraw() const { return isDraw; }
};
