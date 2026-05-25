#pragma once
#include "table.h"
#include <vector>

struct Move {
    int  fr, fc, tr, tc;
    char promotion;   // 'Q' | 'R' | 'B' | 'N'
};

// Minimax engine with alpha-beta pruning and material+PST evaluation.
// Encapsulated as a class so the search depth is configured once at
// construction and callers do not need to pass it on every call.
class AI {
public:
    explicit AI(int searchDepth = 3);

    // Returns the best legal move for `color`. Move::fr == -1 means no moves.
    Move getBestMove(const Table& board, Color color) const;

private:
    int searchDepth;

    // Positive score = good for White, negative = good for Black.
    static int evaluate(const Table& t);

    // Collects and orders all legal moves for `color` (captures first).
    static std::vector<Move> collectMoves(const Table& t, Color color);

    static PieceType charToPromo(char p);

    int minimax(const Table& t, int depth, int alpha, int beta) const;
};
