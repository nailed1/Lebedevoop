#ifndef AI_H
#define AI_H

#include "table.h"
#include <vector>

// Описание хода: откуда, куда, во что превращается пешка
struct Move {
    int  fr, fc, tr, tc;
    char promotion; // 'Q' | 'R' | 'B' | 'N'
};

// Шахматный ИИ на основе минимакса с альфа-бета отсечением
class AI {
public:
    explicit AI(int searchDepth = 3);

    // Возвращает лучший ход для заданного цвета
    Move getBestMove(const Table& board, Color::E color) const;

private:
    int searchDepth;

    // Статическая оценка позиции (положительная = выгодна белым)
    static int evaluate(const Table& t);

    // Собирает все легальные ходы для заданного цвета
    static std::vector<Move> collectMoves(const Table& t, Color::E color);

    static PieceType::E charToPromo(char p);

    // Рекурсивный поиск минимакса с отсечением
    int minimax(const Table& t, int depth, int alpha, int beta) const;
};

#endif // AI_H
