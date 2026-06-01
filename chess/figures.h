#ifndef FIGURES_H
#define FIGURES_H

#include <vector>
#include <utility>

// Цвет фигуры. Struct-обёртка вместо enum class для совместимости с C++98.
struct Color {
    enum E { White, Black, None };
};

// Тип фигуры
struct PieceType {
    enum E { King, Queen, Rook, Bishop, Knight, Pawn, None };
};

// Координаты клетки: .first = row (0–7), .second = col (0–7)
typedef std::pair<int, int> Position;

// Forward declaration — разрывает циклическую зависимость figures.h ↔ table.h
class Table;

// Абстрактный базовый класс для всех фигур.
// Нельзя создать объект Figure напрямую — только через наследников или Figure::create().
class Figure {
protected:
    Color::E     color;
    PieceType::E type;
    bool         hasMoved; // нужно для рокировки и двойного хода пешки

public:
    Figure(Color::E c, PieceType::E t) : color(c), type(t), hasMoved(false) {}

    // virtual ~Figure() обязателен: иначе delete Figure* не вызовет деструктор наследника
    virtual ~Figure() {}

    Color::E     getColor()    const { return color; }
    PieceType::E getType()     const { return type; }
    bool         getHasMoved() const { return hasMoved; }
    void         setHasMoved(bool v) { hasMoved = v; }

    virtual char                  getSymbol()                          const = 0;
    // clone() — виртуальный конструктор копирования: создаёт копию зная только Figure*
    virtual Figure*               clone()                              const = 0;
    virtual int                   getMaterialValue()                   const = 0;
    virtual int                   getPositionalValue(int row, int col) const = 0;
    // Возвращает псевдолегальные ходы (без проверки шаха — этим занимается Table)
    virtual std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const = 0;

    // Фабричный метод: создаёт нужного наследника по типу и цвету
    static Figure* create(PieceType::E t, Color::E c);
};

class King : public Figure {
public:
    explicit King(Color::E c) : Figure(c, PieceType::King) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

class Queen : public Figure {
public:
    explicit Queen(Color::E c) : Figure(c, PieceType::Queen) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

class Rook : public Figure {
public:
    explicit Rook(Color::E c) : Figure(c, PieceType::Rook) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

class Bishop : public Figure {
public:
    explicit Bishop(Color::E c) : Figure(c, PieceType::Bishop) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

class Knight : public Figure {
public:
    explicit Knight(Color::E c) : Figure(c, PieceType::Knight) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

class Pawn : public Figure {
public:
    explicit Pawn(Color::E c) : Figure(c, PieceType::Pawn) {}
    char                  getSymbol()                          const;
    Figure*               clone()                              const;
    int                   getMaterialValue()                   const;
    int                   getPositionalValue(int row, int col) const;
    std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const;
};

#endif // FIGURES_H
