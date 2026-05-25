#pragma once
#include <vector>
#include <utility>
#include <memory>

enum class Color    { White, Black, None };
enum class PieceType { King, Queen, Rook, Bishop, Knight, Pawn, None };

using Position = std::pair<int, int>;

class Table;

// -
// Abstract base — every chess piece must implement these five contracts.
//
// Design rationale:
//   • clone()              — virtual copy constructor: eliminates switch-case
//                            duplication in Table's deep-copy and in any
//                            future serialisation code.
//   • getMaterialValue()   — each piece knows its own centipawn worth; AI no
//                            longer needs a separate type→value switch.
//   • getPositionalValue() — PST lookup lives in the piece, so adding a new
//                            piece type never requires touching AI code.
//   • create()             — factory method: the only place that maps
//                            PieceType → concrete class; all other code uses
//                            clone() or this factory.
// -
class Figure {
protected:
    Color     color;
    PieceType type;
    bool      hasMoved;

public:
    Figure(Color c, PieceType t) : color(c), type(t), hasMoved(false) {}
    virtual ~Figure() = default;

    Color     getColor()    const { return color; }
    PieceType getType()     const { return type; }
    bool      getHasMoved() const { return hasMoved; }
    void      setHasMoved(bool v) { hasMoved = v; }

    virtual char                    getSymbol()                          const = 0;
    virtual std::unique_ptr<Figure> clone()                              const = 0;
    virtual int                     getMaterialValue()                   const = 0;
    virtual int                     getPositionalValue(int row, int col) const = 0;
    virtual std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const = 0;

    // Factory: single switch — adding a new piece means adding one case here.
    static std::unique_ptr<Figure> create(PieceType t, Color c);
};


class King : public Figure {
public:
    explicit King(Color c) : Figure(c, PieceType::King) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};

class Queen : public Figure {
public:
    explicit Queen(Color c) : Figure(c, PieceType::Queen) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};

class Rook : public Figure {
public:
    explicit Rook(Color c) : Figure(c, PieceType::Rook) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};

class Bishop : public Figure {
public:
    explicit Bishop(Color c) : Figure(c, PieceType::Bishop) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};

class Knight : public Figure {
public:
    explicit Knight(Color c) : Figure(c, PieceType::Knight) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};

class Pawn : public Figure {
public:
    explicit Pawn(Color c) : Figure(c, PieceType::Pawn) {}
    char                    getSymbol()                          const override;
    std::unique_ptr<Figure> clone()                              const override;
    int                     getMaterialValue()                   const override;
    int                     getPositionalValue(int row, int col) const override;
    std::vector<Position>   getPseudoMoves(int r, int c, const Table& t) const override;
};
