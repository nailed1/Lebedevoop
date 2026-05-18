#pragma once
#include <vector>
#include <utility>

enum class Color { White, Black, None };
enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn, None };

using Position = std::pair<int, int>;

class Table;

class Figure {
protected:
    Color color;
    PieceType type;
    bool hasMoved;
public:
    Figure(Color c, PieceType t) : color(c), type(t), hasMoved(false) {}
    virtual ~Figure() = default;

    Color getColor() const { return color; }
    PieceType getType() const { return type; }
    bool getHasMoved() const { return hasMoved; }
    void setHasMoved(bool v) { hasMoved = v; }

    virtual char getSymbol() const = 0;
    virtual std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const = 0;
};

class King : public Figure {
public:
    explicit King(Color c) : Figure(c, PieceType::King) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};

class Queen : public Figure {
public:
    explicit Queen(Color c) : Figure(c, PieceType::Queen) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};

class Rook : public Figure {
public:
    explicit Rook(Color c) : Figure(c, PieceType::Rook) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};

class Bishop : public Figure {
public:
    explicit Bishop(Color c) : Figure(c, PieceType::Bishop) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};

class Knight : public Figure {
public:
    explicit Knight(Color c) : Figure(c, PieceType::Knight) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};

class Pawn : public Figure {
public:
    explicit Pawn(Color c) : Figure(c, PieceType::Pawn) {}
    char getSymbol() const override;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const override;
};
