#pragma once
#include "table.h"

struct Move {
    int  fr, fc, tr, tc;
    char promotion;   // 'Q' | 'R' | 'B' | 'N'
};

// Returns the best move for `color` searching `depth` half-moves ahead.
// Uses minimax with alpha-beta pruning and a material+PST evaluation.
// Returns Move with fr == -1 if no legal moves exist.
Move getBestMove(const Table& table, Color color, int depth);
