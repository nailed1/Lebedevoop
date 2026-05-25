/**
 * chess_bridge.cpp
 * C-compatible API wrapping the C++ chess engine (Table / Figure / AI).
 * Compiled as a shared library; loaded by chess_gui.py via ctypes.
 */
#include "table.h"
#include "ai.h"
#include <cstring>

extern "C" {

/*  Lifecycle  */

void* chess_create() {
    Table* t = new Table();
    t->initialize();
    return t;
}

void chess_destroy(void* handle) {
    delete static_cast<Table*>(handle);
}

void chess_reset(void* handle) {
    static_cast<Table*>(handle)->initialize();
}

/*  Board snapshot  */

/**
 * Fills `out` with 64 chars (+NUL) representing the board.
 * Index = row*8 + col  (row 0 = rank 1, col 0 = file a).
 * Piece chars: K Q R B N P (white upper, black lower), '.' for empty.
 */
void chess_get_board(void* handle, char* out) {
    Table* t = static_cast<Table*>(handle);
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            Figure* f = t->getFigureAt(r, c);
            out[r * 8 + c] = f ? f->getSymbol() : '.';
        }
    out[64] = '\0';
}

/*  Turn / state  */

/** Returns 'W' or 'B'. */
char chess_get_turn(void* handle) {
    return static_cast<Table*>(handle)->getCurrentTurn() == Color::White ? 'W' : 'B';
}

int chess_is_game_over(void* handle) {
    return static_cast<Table*>(handle)->isGameOver() ? 1 : 0;
}

int chess_is_draw(void* handle) {
    return static_cast<Table*>(handle)->getIsDraw() ? 1 : 0;
}

/** Returns 'W', 'B', or 'N' (none). */
char chess_get_winner(void* handle) {
    Color w = static_cast<Table*>(handle)->getWinner();
    if (w == Color::White) return 'W';
    if (w == Color::Black) return 'B';
    return 'N';
}

/** color: 'W' or 'B'.  Returns 1 if that side is in check. */
int chess_is_in_check(void* handle, char color) {
    Color c = (color == 'W') ? Color::White : Color::Black;
    return static_cast<Table*>(handle)->isInCheck(c) ? 1 : 0;
}

/*  Move generation  */

/**
 * Get legal destinations for the piece at (r, c).
 * `positions` must point to a buffer of at least 56 ints (28 squares x 2).
 * Returns the number of moves; writes return_value*2 ints.
 */
int chess_get_valid_moves(void* handle, int r, int c, int* positions) {
    std::vector<Position> moves = static_cast<Table*>(handle)->getValidMoves(r, c);
    int n = 0;
    for (size_t i = 0; i < moves.size(); i++) {
        positions[n * 2]     = moves[i].first;
        positions[n * 2 + 1] = moves[i].second;
        n++;
    }
    return n;
}

/*  Execute move  */

/**
 * Make a move from (fr, fc) to (tr, tc).
 * promotion: 'Q'|'R'|'B'|'N' (only relevant for pawn promotion).
 * Returns 1 on success, 0 if illegal.
 */
int chess_make_move(void* handle, int fr, int fc, int tr, int tc, char promotion) {
    PieceType promo;
    switch (promotion) {
        case 'R': promo = PieceType::Rook;   break;
        case 'B': promo = PieceType::Bishop; break;
        case 'N': promo = PieceType::Knight; break;
        default:  promo = PieceType::Queen;  break;
    }
    return static_cast<Table*>(handle)->makeMove(fr, fc, tr, tc, promo) ? 1 : 0;
}

/*  AI  */

/**
 * Find the best move for `color` at the given search depth (3 recommended).
 * out[0..3] = fr, fc, tr, tc
 * out[4]    = promotion as ASCII int ('Q'=81, 'R'=82, 'B'=66, 'N'=78)
 * Returns 1 if a move was found, 0 if no legal moves exist.
 */
int chess_get_best_move(void* handle, char color, int depth, int* out) {
    Color  c = (color == 'W') ? Color::White : Color::Black;
    Move   m = AI(depth).getBestMove(*static_cast<Table*>(handle), c);
    if (m.fr < 0) return 0;
    out[0] = m.fr;
    out[1] = m.fc;
    out[2] = m.tr;
    out[3] = m.tc;
    out[4] = static_cast<int>(m.promotion);
    return 1;
}

} // extern "C"
