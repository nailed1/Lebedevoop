#include "figures.h"
#include "table.h"

// Позиционные бонусы (piece-square tables): бонус за нахождение фигуры на клетке.
// Индексация: [row][col], row=0 — база белых. Для чёрных зеркалится: r = 7 - row.
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

Figure* Figure::create(PieceType::E t, Color::E c) {
    switch (t) {
        case PieceType::King:   return new King(c);
        case PieceType::Queen:  return new Queen(c);
        case PieceType::Rook:   return new Rook(c);
        case PieceType::Bishop: return new Bishop(c);
        case PieceType::Knight: return new Knight(c);
        case PieceType::Pawn:   return new Pawn(c);
        default:                return NULL;
    }
}

// ====== Король ======

char King::getSymbol()        const { return color == Color::White ? 'K' : 'k'; }
int  King::getMaterialValue() const { return 20000; }

Figure* King::clone() const {
    Figure* c = new King(color);
    c->setHasMoved(hasMoved);
    return c;
}

int King::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_KING[r][col];
}

std::vector<Position> King::getPseudoMoves(int r, int c, const Table& t) const {
    std::vector<Position> moves;
    Color::E enemy = (color == Color::White) ? Color::Black : Color::White;

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

    // Рокировка: король и ладья не двигались, путь свободен, клетки не под шахом
    if (!hasMoved && !t.isSquareAttacked(r, c, enemy)) {
        Figure* kr = t.getFigureAt(r, c + 3); // короткая рокировка (ладья h)
        if (kr && kr->getType() == PieceType::Rook && kr->getColor() == color &&
            !kr->getHasMoved() &&
            !t.getFigureAt(r, c + 1) && !t.getFigureAt(r, c + 2) &&
            !t.isSquareAttacked(r, c + 1, enemy) && !t.isSquareAttacked(r, c + 2, enemy))
            moves.push_back(Position(r, c + 2));

        Figure* qr = t.getFigureAt(r, c - 4); // длинная рокировка (ладья a)
        if (qr && qr->getType() == PieceType::Rook && qr->getColor() == color &&
            !qr->getHasMoved() &&
            !t.getFigureAt(r, c - 1) && !t.getFigureAt(r, c - 2) && !t.getFigureAt(r, c - 3) &&
            !t.isSquareAttacked(r, c - 1, enemy) && !t.isSquareAttacked(r, c - 2, enemy))
            moves.push_back(Position(r, c - 2));
    }

    return moves;
}

// ====== Ферзь ======

char Queen::getSymbol()        const { return color == Color::White ? 'Q' : 'q'; }
int  Queen::getMaterialValue() const { return 900; }

Figure* Queen::clone() const {
    Figure* c = new Queen(color);
    c->setHasMoved(hasMoved);
    return c;
}

int Queen::getPositionalValue(int row, int col) const {
    int r = (color == Color::White) ? row : (7 - row);
    return PST_QUEEN[r][col];
}

// Ходит по всем 8 направлениям до упора (ладья + слон)
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

// ====== Ладья ======

char Rook::getSymbol()        const { return color == Color::White ? 'R' : 'r'; }
int  Rook::getMaterialValue() const { return 500; }

Figure* Rook::clone() const {
    Figure* c = new Rook(color);
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

// ====== Слон ======

char Bishop::getSymbol()        const { return color == Color::White ? 'B' : 'b'; }
int  Bishop::getMaterialValue() const { return 330; }

Figure* Bishop::clone() const {
    Figure* c = new Bishop(color);
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

// ====== Конь ======

char Knight::getSymbol()        const { return color == Color::White ? 'N' : 'n'; }
int  Knight::getMaterialValue() const { return 320; }

Figure* Knight::clone() const {
    Figure* c = new Knight(color);
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

// ====== Пешка ======

char Pawn::getSymbol()        const { return color == Color::White ? 'P' : 'p'; }
int  Pawn::getMaterialValue() const { return 100; }

Figure* Pawn::clone() const {
    Figure* c = new Pawn(color);
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

    // Ход вперёд
    if (t.isInBounds(r + dir, c) && !t.getFigureAt(r + dir, c)) {
        moves.push_back(Position(r + dir, c));
        if (r == startRow && !t.getFigureAt(r + 2 * dir, c))
            moves.push_back(Position(r + 2 * dir, c));
    }

    // Взятие по диагонали (обычное и на проходе)
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
