#ifndef TABLE_H
#define TABLE_H

#include "cell.h"
#include <vector>

// Шахматная доска.
//
// Владение фигурами: каждая клетка cells[r][c] является единственным
// владельцем своей фигуры. Нет отдельных векторов pieces/captured —
// Table::~Table просто удаляет всё, что есть в клетках.
//
// Преимущество: удаление фигуры (взятие) — O(1) delete, без линейного поиска.
// Копирование доски (нужно в каждом узле минимакса) — ровно 64 итерации,
// без лишнего копирования вектора снятых фигур.
class Table {
private:
    // [?] mutable — позволяет изменять поле даже в const-методе.
    //     wouldLeaveInCheck объявлен const (не меняет "логическое" состояние доски),
    //     но физически переставляет указатели для симуляции хода.
    //     Без mutable компилятор запретил бы любое изменение поля внутри const-метода.
    mutable Cell cells[8][8];

    Color::E currentTurn;
    Position enPassantTarget; // клетка взятия на проходе, (-1,-1) если нет
    bool     gameOver;
    Color::E winner;
    bool     isDraw;

    bool inBounds(int r, int c) const { return r >= 0 && r < 8 && c >= 0 && c < 8; }

    // Проверяет, оставит ли ход своего короля под шахом (временно симулирует ход)
    bool wouldLeaveInCheck(int fr, int fc, int tr, int tc) const;

    // Помещает фигуру на клетку (не удаляет то, что там было — вызывающий обязан
    // сначала очистить клетку через removePieceAt)
    void addPiece(Figure* fig, int r, int c);

    // Удаляет фигуру с клетки и сразу освобождает память
    void removePieceAt(int r, int c);

public:
    Table();

    // [?] Конструктор копирования объявлен явно — нужно глубокое копирование.
    //     Стандартный (сгенерированный компилятором) скопировал бы только указатели в cells:
    //     оба объекта Table владели бы одними фигурами → double free при деструкции.
    //     Здесь: для каждой занятой клетки вызываем orig->clone() — независимая копия.
    Table(const Table& other);

    // [?] Деструктор явный — освобождает все фигуры на доске.
    //     Без него фигуры (created via new) утекли бы при уничтожении Table.
    ~Table();

    void initialize(); // расставляет начальную позицию
    void display()   const; // вывод в консоль

    Figure*  getFigureAt(int r, int c) const;
    bool     isInBounds(int r, int c) const { return inBounds(r, c); }
    Position getEnPassantTarget() const { return enPassantTarget; }

    bool isSquareAttacked(int r, int c, Color::E byColor) const;
    bool isInCheck(Color::E color) const;

    std::vector<Position> getValidMoves(int r, int c) const;

    bool makeMove(int fr, int fc, int tr, int tc,
                  PieceType::E promotion = PieceType::Queen);

    // Для загрузки позиций из файлов задач
    void clearBoard();
    void placePiece(PieceType::E t, Color::E c, int row, int col);

    Color::E getCurrentTurn() const { return currentTurn; }
    bool     isGameOver()     const { return gameOver; }
    Color::E getWinner()      const { return winner; }
    bool     getIsDraw()      const { return isDraw; }
};

#endif // TABLE_H
