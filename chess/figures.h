#ifndef FIGURES_H
#define FIGURES_H

#include <vector>
#include <utility>

// [?] Почему не просто enum? — в C++98 нет enum class, имена White/Black попали бы в глобальный
//     namespace и конфликтовали с чужими именами. Struct-обёртка изолирует их в Color::White.
struct Color {
    enum E { White, Black, None };
};

// Тип фигуры
struct PieceType {
    enum E { King, Queen, Rook, Bishop, Knight, Pawn, None };
};

typedef std::pair<int, int> Position;

// [?] Прямое объявление (forward declaration) — разрывает циклическую зависимость:
//     figures.h включает table.h, а table.h включает figures.h. Без этого — бесконечный цикл.
//     Компилятору достаточно знать, что Table — это класс, чтобы принять Table& в параметре.
class Table;

// Базовый класс для всех шахматных фигур
class Figure {
protected:
    // [?] protected, не private — наследники (King, Queen...) обращаются к этим полям напрямую.
    //     private запретил бы доступ даже наследникам.
    Color::E     color;
    PieceType::E type;
    bool         hasMoved;

public:
    // [?] Список инициализации (: color(c), type(t), hasMoved(false)) — поля инициализируются
    //     до входа в тело конструктора. Для const-полей и ссылок это единственный способ.
    Figure(Color::E c, PieceType::E t) : color(c), type(t), hasMoved(false) {}

    // [?] Виртуальный деструктор — ОБЯЗАТЕЛЕН у базового класса с виртуальными методами.
    //     Без virtual: delete Figure* вызовет только ~Figure(), но не ~King() — утечка памяти.
    virtual ~Figure() {}

    Color::E     getColor()    const { return color; }
    PieceType::E getType()     const { return type; }
    bool         getHasMoved() const { return hasMoved; }
    void         setHasMoved(bool v) { hasMoved = v; }

    // [?] = 0 — чисто виртуальные методы, делают Figure абстрактным классом.
    //     Нельзя написать: Figure f; — ошибка компиляции.
    //     Каждый наследник ОБЯЗАН переопределить все эти методы.
    virtual char                  getSymbol()                          const = 0;

    // [?] Паттерн clone() — виртуальный конструктор копирования.
    //     Зная только Figure*, невозможно вызвать правильный copy constructor.
    //     clone() через vtable сам знает реальный тип и создаёт King/Queen/... нужного вида.
    virtual Figure*               clone()                              const = 0;
    virtual int                   getMaterialValue()                   const = 0;
    virtual int                   getPositionalValue(int row, int col) const = 0;
    virtual std::vector<Position> getPseudoMoves(int r, int c, const Table& t) const = 0;

    // [?] Паттерн "Фабричный метод" (Factory Method) — создаёт объект нужного типа,
    //     не раскрывая вызывающему коду, какой конкретный класс создаётся.
    //     static — вызывается без объекта: Figure::create(PieceType::King, Color::White)
    static Figure* create(PieceType::E t, Color::E c);
};


class King : public Figure {
public:
    // [?] explicit — запрещает неявное преобразование: King k = Color::White; // ошибка.
    //     Без explicit компилятор мог бы тихо создать King там, где ожидался Color::E.
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
