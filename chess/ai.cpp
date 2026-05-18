#include "ai.h"
#include <vector>
#include <algorithm>
#include <climits>

//  Material values (centipawns) 
static int pieceValue(PieceType t) {
    switch (t) {
        case PieceType::Pawn:   return 100;
        case PieceType::Knight: return 320;
        case PieceType::Bishop: return 330;
        case PieceType::Rook:   return 500;
        case PieceType::Queen:  return 900;
        case PieceType::King:   return 20000;
        default:                return 0;
    }
}

//  Piece-square tables ─
// Indexed [row][col] where row 0 = rank 1 (white's back rank).
// For white pieces use [r][c] directly.
// For black pieces mirror the row: use [7-r][c].

static const int PST_PAWN[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },  // rank 1 – impossible for pawns
    {  5, 10, 10,-20,-20, 10, 10,  5 },  // rank 2 – starting row; penalise centre blocks
    {  5, -5,-10,  0,  0,-10, -5,  5 },  // rank 3
    {  0,  0,  0, 20, 20,  0,  0,  0 },  // rank 4 – centre push bonus
    {  5,  5, 10, 25, 25, 10,  5,  5 },  // rank 5
    { 10, 10, 20, 30, 30, 20, 10, 10 },  // rank 6 – advanced pawns valuable
    { 50, 50, 50, 50, 50, 50, 50, 50 },  // rank 7 – about to promote
    {  0,  0,  0,  0,  0,  0,  0,  0 },  // rank 8 – promotion handled separately
};

static const int PST_KNIGHT[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50 },
    {-40,-20,  0,  5,  5,  0,-20,-40 },
    {-30,  5, 10, 15, 15, 10,  5,-30 },
    {-30,  0, 15, 20, 20, 15,  0,-30 },
    {-30,  5, 15, 20, 20, 15,  5,-30 },
    {-30,  0, 10, 15, 15, 10,  0,-30 },
    {-40,-20,  0,  0,  0,  0,-20,-40 },
    {-50,-40,-30,-30,-30,-30,-40,-50 },
};

static const int PST_BISHOP[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20 },
    {-10,  5,  0,  0,  0,  0,  5,-10 },
    {-10, 10, 10, 10, 10, 10, 10,-10 },
    {-10,  0, 10, 10, 10, 10,  0,-10 },
    {-10,  5,  5, 10, 10,  5,  5,-10 },
    {-10,  0,  5, 10, 10,  5,  0,-10 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-20,-10,-10,-10,-10,-10,-10,-20 },
};

static const int PST_ROOK[8][8] = {
    {  0,  0,  0,  5,  5,  0,  0,  0 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    { -5,  0,  0,  0,  0,  0,  0, -5 },
    {  5, 10, 10, 10, 10, 10, 10,  5 },
    {  0,  0,  0,  0,  0,  0,  0,  0 },
};

static const int PST_QUEEN[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20 },
    {-10,  0,  5,  0,  0,  0,  0,-10 },
    {-10,  5,  5,  5,  5,  5,  0,-10 },
    {  0,  0,  5,  5,  5,  5,  0, -5 },
    { -5,  0,  5,  5,  5,  5,  0, -5 },
    {-10,  0,  5,  5,  5,  5,  0,-10 },
    {-10,  0,  0,  0,  0,  0,  0,-10 },
    {-20,-10,-10, -5, -5,-10,-10,-20 },
};

static const int PST_KING[8][8] = {
    { 20, 30, 10,  0,  0, 10, 30, 20 },  // castled king safety bonus
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
};

static int pst(PieceType t, Color col, int r, int c) {
    int row = (col == Color::White) ? r : (7 - r);
    switch (t) {
        case PieceType::Pawn:   return PST_PAWN  [row][c];
        case PieceType::Knight: return PST_KNIGHT[row][c];
        case PieceType::Bishop: return PST_BISHOP[row][c];
        case PieceType::Rook:   return PST_ROOK  [row][c];
        case PieceType::Queen:  return PST_QUEEN [row][c];
        case PieceType::King:   return PST_KING  [row][c];
        default:                return 0;
    }
}

//  Static evaluation ─
// Positive = good for White, negative = good for Black.
static int evaluate(const Table& t) {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = t.getFigureAt(r, c);
            if (!f) continue;
            int v = pieceValue(f->getType()) + pst(f->getType(), f->getColor(), r, c);
            score += (f->getColor() == Color::White) ? v : -v;
        }
    }
    return score;
}

//  Move list for one side 
static std::vector<Move> collectMoves(const Table& t, Color color) {
    std::vector<Move> moves;
    moves.reserve(40);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = t.getFigureAt(r, c);
            if (!f || f->getColor() != color) continue;

            for (auto& [tr, tc] : t.getValidMoves(r, c)) {
                bool isPromo = (f->getType() == PieceType::Pawn && (tr == 0 || tr == 7));
                if (isPromo) {
                    // Explore all promotions; queen is first (most likely best)
                    for (char p : {'Q', 'R', 'B', 'N'})
                        moves.push_back({r, c, tr, tc, p});
                } else {
                    moves.push_back({r, c, tr, tc, 'Q'});
                }
            }
        }
    }

    // Move ordering: captures first (higher victim value = earlier).
    // Improves alpha-beta pruning significantly.
    std::stable_sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        Figure* va = t.getFigureAt(a.tr, a.tc);
        Figure* vb = t.getFigureAt(b.tr, b.tc);
        return (va ? pieceValue(va->getType()) : 0) >
               (vb ? pieceValue(vb->getType()) : 0);
    });

    return moves;
}

//  Helpers ─
static PieceType charToPromo(char p) {
    switch (p) {
        case 'R': return PieceType::Rook;
        case 'B': return PieceType::Bishop;
        case 'N': return PieceType::Knight;
        default:  return PieceType::Queen;
    }
}

//  Minimax with alpha-beta ─
// Score convention: positive = good for White, negative = good for Black.
// `depth` is remaining half-moves; White maximises, Black minimises.
static int minimax(const Table& t, int depth, int alpha, int beta) {
    // Terminal: game ended after the previous move
    if (t.isGameOver()) {
        if (t.getIsDraw()) return 0;
        return (t.getWinner() == Color::White) ? 100000 : -100000;
    }

    if (depth == 0) return evaluate(t);

    Color color = t.getCurrentTurn();
    bool  maxing = (color == Color::White);
    auto  moves  = collectMoves(t, color);

    if (moves.empty()) return evaluate(t);   // shouldn't happen if gameOver is set correctly

    int best = maxing ? INT_MIN : INT_MAX;

    for (auto& m : moves) {
        Table copy = t;                           // deep copy (see Table copy constructor)
        copy.makeMove(m.fr, m.fc, m.tr, m.tc, charToPromo(m.promotion));

        int val = minimax(copy, depth - 1, alpha, beta);

        if (maxing) {
            if (val > best) best = val;
            if (best > alpha) alpha = best;
        } else {
            if (val < best) best = val;
            if (best < beta) beta = best;
        }

        if (beta <= alpha) break;   // alpha-beta cut-off
    }

    return best;
}

//  Public API 
Move getBestMove(const Table& table, Color color, int depth) {
    auto moves = collectMoves(table, color);
    if (moves.empty()) return {-1, -1, -1, -1, 'Q'};

    bool maxing = (color == Color::White);
    int  best   = maxing ? INT_MIN : INT_MAX;
    Move bestMove = moves[0];

    int alpha = INT_MIN;
    int beta  = INT_MAX;

    for (auto& m : moves) {
        Table copy = table;
        copy.makeMove(m.fr, m.fc, m.tr, m.tc, charToPromo(m.promotion));

        int val = minimax(copy, depth - 1, alpha, beta);

        bool better = maxing ? (val > best) : (val < best);
        if (better) {
            best     = val;
            bestMove = m;
        }

        if (maxing) alpha = std::max(alpha, best);
        else        beta  = std::min(beta,  best);
    }

    return bestMove;
}
