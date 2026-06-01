#ifndef AI_H
#define AI_H

#include "table.h"
#include <vector>

// struct в C++ идентичен class, но поля по умолчанию public
struct Move {
    int  fr, fc; // from row, from col
    int  tr, tc; // to row, to col
    char promotion; // 'Q' 'R' 'B' 'N' — фигура при превращении пешки
};

// ИИ на основе минимакса с альфа-бета отсечением.
class AI {
public:
    explicit AI(int searchDepth = 3);
    Move getBestMove(const Table& board, Color::E color) const;

private:
    int searchDepth;

    static int evaluate(const Table& t);
    // Собирает все ходы цвета color; раскрывает превращение в 4 варианта; сортирует взятия первыми
    static std::vector<Move> collectMoves(const Table& t, Color::E color);
    static PieceType::E charToPromo(char p);
    int minimax(const Table& t, int depth, int alpha, int beta) const;
};

#endif // AI_H
