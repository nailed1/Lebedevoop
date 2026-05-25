#include "figures.h"
#include "table.h"

// -
// Piece-square tables (PST) — indexed [row][col], row 0 = rank 1.
// Positive values favour white; for black pieces the row is mirrored.
// Centralised here so that adding a new piece only requires adding a new
// table and implementing getPositionalValue() in the new class.
// -

static const int PST_PAWN[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0 },
    {  5, 10, 10,-20,-20, 10, 10,  5 },
    {  5, -5,-10,  0,  0,-10, -5,  5 },
    {  0,  0,  0, 20, 20,  0,  0,  0 },
    {  5,  5, 10, 25, 25, 10,  5,  5 },
    { 10, 10, 20, 30, 30, 20, 10, 10 },
    { 50, 50, 50, 50, 50, 50, 50, 50 },
    {  0,  0,  0,  0,  0,  0,  0,  0 },
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
    { 20, 30, 10,  0,  0, 10, 30, 20 },
    { 20, 20,  0,  0,  0,  0, 20, 20 },
    {-10,-20,-20,-20,-20,-20,-20,-10 },
    {-20,-30,-30,-40,-40,-30,-30,-20 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
    {-30,-40,-40,-50,-50,-40,-40,-30 },
};

// -
// Factory — only place that maps PieceType to a concrete class.
// -
std::unique_ptr<Figure> Figure::create(PieceType t, Color c) {
    switch (t) {
        case PieceType::King:   return std::unique_ptr<Figure>(new King(c));
        case PieceType::Queen:  return std::unique_ptr<Figure>(new Queen(c));
        case PieceType::Rook:   return std::unique_ptr<Figure>(new Rook(c));
        case PieceType::Bishop: return std::unique_ptr<Figure>(new Bishop(c));
        case PieceType::Knight: return std::unique_ptr<Figure>(new Knight(c));
        case PieceType::Pawn:   return std::unique_ptr<Figure>(new Pawn(c));
        default:                return std::unique_ptr<Figure>();
    }
}

// -
// King
// -
char King::getSymbol()        const { return color == Color::White ? 'K' : 'k'; }
int  King::getMaterialValue() const { return 20000; }

std::unique_ptr<Figure> King::clone() const {
    std::unique_ptr<Figure> c(new King(color));
    c->setHasMoved(hasMoved);
    return c;
}

int King::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_KING[r][col];
}

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
                moves.push_back(Position(nr, nc));
        }
    }

    if (!hasMoved && !t.isSquareAttacked(r, c, enemy)) {
        // Kingside
        Figure* kr = t.getFigureAt(r, c + 3);
        if (kr && kr->getType() == PieceType::Rook && kr->getColor() == color &&
            !kr->getHasMoved() &&
            !t.getFigureAt(r, c + 1) && !t.getFigureAt(r, c + 2) &&
            !t.isSquareAttacked(r, c + 1, enemy) && !t.isSquareAttacked(r, c + 2, enemy))
            moves.push_back(Position(r, c + 2));

        // Queenside
        Figure* qr = t.getFigureAt(r, c - 4);
        if (qr && qr->getType() == PieceType::Rook && qr->getColor() == color &&
            !qr->getHasMoved() &&
            !t.getFigureAt(r, c - 1) && !t.getFigureAt(r, c - 2) && !t.getFigureAt(r, c - 3) &&
            !t.isSquareAttacked(r, c - 1, enemy) && !t.isSquareAttacked(r, c - 2, enemy))
            moves.push_back(Position(r, c - 2));
    }

    return moves;
}

// -
// Queen
// -
char Queen::getSymbol()        const { return color == Color::White ? 'Q' : 'q'; }
int  Queen::getMaterialValue() const { return 900; }

std::unique_ptr<Figure> Queen::clone() const {
    std::unique_ptr<Figure> c(new Queen(color));
    c->setHasMoved(hasMoved);
    return c;
}

int Queen::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_QUEEN[r][col];
}

std::vector<Position> Queen::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[8][2] = {{1,0},{-1,0},{0,1},{0,-1},{1,1},{1,-1},{-1,1},{-1,-1}};
    for (int d = 0; d < 8; d++) {
        for (int i = 1; i < 8; i++) {
            int nr = r + dirs[d][0] * i, nc = c + dirs[d][1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back(Position(nr, nc));
            } else {
                if (fig->getColor() != color) moves.push_back(Position(nr, nc));
                break;
            }
        }
    }
    return moves;
}

// -
// Rook
// -
char Rook::getSymbol()        const { return color == Color::White ? 'R' : 'r'; }
int  Rook::getMaterialValue() const { return 500; }

std::unique_ptr<Figure> Rook::clone() const {
    std::unique_ptr<Figure> c(new Rook(color));
    c->setHasMoved(hasMoved);
    return c;
}

int Rook::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_ROOK[r][col];
}

std::vector<Position> Rook::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            int nr = r + dirs[d][0] * i, nc = c + dirs[d][1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back(Position(nr, nc));
            } else {
                if (fig->getColor() != color) moves.push_back(Position(nr, nc));
                break;
            }
        }
    }
    return moves;
}

// -
// Bishop
// -
char Bishop::getSymbol()        const { return color == Color::White ? 'B' : 'b'; }
int  Bishop::getMaterialValue() const { return 330; }

std::unique_ptr<Figure> Bishop::clone() const {
    std::unique_ptr<Figure> c(new Bishop(color));
    c->setHasMoved(hasMoved);
    return c;
}

int Bishop::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_BISHOP[r][col];
}

std::vector<Position> Bishop::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dirs[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            int nr = r + dirs[d][0] * i, nc = c + dirs[d][1] * i;
            if (!t.isInBounds(nr, nc)) break;
            Figure* fig = t.getFigureAt(nr, nc);
            if (!fig) {
                moves.push_back(Position(nr, nc));
            } else {
                if (fig->getColor() != color) moves.push_back(Position(nr, nc));
                break;
            }
        }
    }
    return moves;
}

// -
// Knight
// -
char Knight::getSymbol()        const { return color == Color::White ? 'N' : 'n'; }
int  Knight::getMaterialValue() const { return 320; }

std::unique_ptr<Figure> Knight::clone() const {
    std::unique_ptr<Figure> c(new Knight(color));
    c->setHasMoved(hasMoved);
    return c;
}

int Knight::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_KNIGHT[r][col];
}

std::vector<Position> Knight::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int jumps[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
    for (int j = 0; j < 8; j++) {
        int nr = r + jumps[j][0], nc = c + jumps[j][1];
        if (!t.isInBounds(nr, nc)) continue;
        Figure* fig = t.getFigureAt(nr, nc);
        if (!fig || fig->getColor() != color)
            moves.push_back(Position(nr, nc));
    }
    return moves;
}

// -
// Pawn
// -
char Pawn::getSymbol()        const { return color == Color::White ? 'P' : 'p'; }
int  Pawn::getMaterialValue() const { return 100; }

std::unique_ptr<Figure> Pawn::clone() const {
    std::unique_ptr<Figure> c(new Pawn(color));
    c->setHasMoved(hasMoved);
    return c;
}

int Pawn::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_PAWN[r][col];
}

std::vector<Position> Pawn::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    int dir      = (color == Color::White) ? 1 : -1;
    int startRow = (color == Color::White) ? 1 : 6;

    if (t.isInBounds(r + dir, c) && !t.getFigureAt(r + dir, c)) {
        moves.push_back(Position(r + dir, c));
        if (r == startRow && !t.getFigureAt(r + 2 * dir, c))
            moves.push_back(Position(r + 2 * dir, c));
    }

    int dcs[2] = {-1, 1};
    for (int i = 0; i < 2; i++) {
        int dc = dcs[i];
        int nr = r + dir, nc = c + dc;
        if (!t.isInBounds(nr, nc)) continue;
        Figure* fig = t.getFigureAt(nr, nc);
        if (fig && fig->getColor() != color)
            moves.push_back(Position(nr, nc));
        Position ep = t.getEnPassantTarget();
        if (ep.first == nr && ep.second == nc)
            moves.push_back(Position(nr, nc));
    }

    return moves;
}
