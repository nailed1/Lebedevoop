#include "figures.h"
#include "table.h"

// ---- King ----

char King::getSymbol() const { return color == Color::White ? 'K' : 'k'; }

std::vector<Position> King::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    Color enemy = (color == Color::White) ? Color::Black : Color::White;

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (!t.isInBounds(nr, nc)) continue;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig || fig->getColor() != color)
                moves.push_back({nr, nc});
        }
    }

    // Castling: king must not have moved and must not currently be in check
    if (!hasMoved && !t.isSquareAttacked(r, c, enemy)) {
        // Kingside: rook at col c+3, empty c+1 and c+2, path not attacked
        Figure* kr = t.getFigureAt(r, c + 3);
        if (kr && kr->getType() == PieceType::Rook && kr->getColor() == color && !kr->getHasMoved() &&
            !t.getFigureAt(r, c + 1) && !t.getFigureAt(r, c + 2) &&
            !t.isSquareAttacked(r, c + 1, enemy) && !t.isSquareAttacked(r, c + 2, enemy)) {
            moves.push_back({r, c + 2});
        }
        // Queenside: rook at col c-4, empty c-1, c-2, c-3, king path not attacked
        Figure* qr = t.getFigureAt(r, c - 4);
        if (qr && qr->getType() == PieceType::Rook && qr->getColor() == color && !qr->getHasMoved() &&
            !t.getFigureAt(r, c - 1) && !t.getFigureAt(r, c - 2) && !t.getFigureAt(r, c - 3) &&
            !t.isSquareAttacked(r, c - 1, enemy) && !t.isSquareAttacked(r, c - 2, enemy)) {
            moves.push_back({r, c - 2});
        }
    }

    return moves;
}

// ---- Queen ----

char Queen::getSymbol() const { return color == Color::White ? 'Q' : 'q'; }

std::vector<Position> Queen::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& d : dirs) {
        for (int i = 1; i < 8; i++) {
            int nr = r + d[0] * i, nc = c + d[1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back({nr, nc});
            } else {
                if (fig->getColor() != color) moves.push_back({nr, nc});
                break;
            }
        }
    }
    return moves;
}

// ---- Rook ----

char Rook::getSymbol() const { return color == Color::White ? 'R' : 'r'; }

std::vector<Position> Rook::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& d : dirs) {
        for (int i = 1; i < 8; i++) {
            int nr = r + d[0] * i, nc = c + d[1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back({nr, nc});
            } else {
                if (fig->getColor() != color) moves.push_back({nr, nc});
                break;
            }
        }
    }
    return moves;
}

// ---- Bishop ----

char Bishop::getSymbol() const { return color == Color::White ? 'B' : 'b'; }

std::vector<Position> Bishop::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& d : dirs) {
        for (int i = 1; i < 8; i++) {
            int nr = r + d[0] * i, nc = c + d[1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back({nr, nc});
            } else {
                if (fig->getColor() != color) moves.push_back({nr, nc});
                break;
            }
        }
    }
    return moves;
}

// ---- Knight ----

char Knight::getSymbol() const { return color == Color::White ? 'N' : 'n'; }

std::vector<Position> Knight::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int jumps[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
    for (auto& j : jumps) {
        int nr = r + j[0], nc = c + j[1];
        if (!t.isInBounds(nr, nc)) continue;
        Figure* fig = t.getFigureAt(nr, nc);
        if (!fig || fig->getColor() != color)
            moves.push_back({nr, nc});
    }
    return moves;
}

// ---- Pawn ----

char Pawn::getSymbol() const { return color == Color::White ? 'P' : 'p'; }

std::vector<Position> Pawn::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dir = (color == Color::White) ? 1 : -1;
    int startRow = (color == Color::White) ? 1 : 6;

    // Forward one square
    if (t.isInBounds(r + dir, c) && !t.getFigureAt(r + dir, c)) {
        moves.push_back({r + dir, c});
        // Double push from starting row
        if (r == startRow && !t.getFigureAt(r + 2 * dir, c))
            moves.push_back({r + 2 * dir, c});
    }

    // Diagonal captures and en passant
    for (int dc : {-1, 1}) {
        int nr = r + dir, nc = c + dc;
        if (!t.isInBounds(nr, nc)) continue;
        Figure* fig = t.getFigureAt(nr, nc);
        if (fig && fig->getColor() != color)
            moves.push_back({nr, nc});
        Position ep = t.getEnPassantTarget();
        if (ep.first == nr && ep.second == nc)
            moves.push_back({nr, nc});
    }

    return moves;
}
