#include "table.h"
#include <iostream>
#include <cstdlib>

// Copy constructor — deep-copy used by the AI search tree.
// Uses Figure::clone() so no switch-case is needed here; adding a new piece
// type is fully handled inside the piece class itself.
Table::Table(const Table& other)
    : currentTurn(other.currentTurn),
      enPassantTarget(other.enPassantTarget),
      gameOver(other.gameOver),
      winner(other.winner),
      isDraw(other.isDraw)
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            cells[r][c] = Cell(r, c);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* orig = other.cells[r][c].getFigure();
            if (orig) {
                std::unique_ptr<Figure> copy = orig->clone();
                cells[r][c].setFigure(copy.get());
                pieces.push_back(std::move(copy));
            }
        }
    }

    for (size_t i = 0; i < other.captured.size(); i++)
        captured.push_back(other.captured[i]->clone());
}

Table::Table()
    : currentTurn(Color::White), enPassantTarget(Position(-1, -1)),
      gameOver(false), winner(Color::None), isDraw(false)
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            cells[r][c] = Cell(r, c);
}

void Table::addPiece(std::unique_ptr<Figure> fig, int r, int c) {
    cells[r][c].setFigure(fig.get());
    pieces.push_back(std::move(fig));
}

void Table::removePieceAt(int r, int c) {
    Figure* fig = cells[r][c].getFigure();
    if (!fig) return;
    cells[r][c].setFigure(nullptr);
    for (size_t i = 0; i < pieces.size(); i++) {
        if (pieces[i].get() == fig) {
            captured.push_back(std::move(pieces[i]));
            pieces.erase(pieces.begin() + static_cast<int>(i));
            return;
        }
    }
}

void Table::initialize() {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            cells[r][c].setFigure(nullptr);
    pieces.clear();
    captured.clear();
    currentTurn      = Color::White;
    enPassantTarget  = Position(-1, -1);
    gameOver         = false;
    winner           = Color::None;
    isDraw           = false;

    // Uses Figure::create() — adding a custom piece only requires changing
    // the factory; board setup code stays the same.
    addPiece(Figure::create(PieceType::Rook,   Color::White), 0, 0);
    addPiece(Figure::create(PieceType::Knight, Color::White), 0, 1);
    addPiece(Figure::create(PieceType::Bishop, Color::White), 0, 2);
    addPiece(Figure::create(PieceType::Queen,  Color::White), 0, 3);
    addPiece(Figure::create(PieceType::King,   Color::White), 0, 4);
    addPiece(Figure::create(PieceType::Bishop, Color::White), 0, 5);
    addPiece(Figure::create(PieceType::Knight, Color::White), 0, 6);
    addPiece(Figure::create(PieceType::Rook,   Color::White), 0, 7);
    for (int c = 0; c < 8; c++)
        addPiece(Figure::create(PieceType::Pawn, Color::White), 1, c);

    addPiece(Figure::create(PieceType::Rook,   Color::Black), 7, 0);
    addPiece(Figure::create(PieceType::Knight, Color::Black), 7, 1);
    addPiece(Figure::create(PieceType::Bishop, Color::Black), 7, 2);
    addPiece(Figure::create(PieceType::Queen,  Color::Black), 7, 3);
    addPiece(Figure::create(PieceType::King,   Color::Black), 7, 4);
    addPiece(Figure::create(PieceType::Bishop, Color::Black), 7, 5);
    addPiece(Figure::create(PieceType::Knight, Color::Black), 7, 6);
    addPiece(Figure::create(PieceType::Rook,   Color::Black), 7, 7);
    for (int c = 0; c < 8; c++)
        addPiece(Figure::create(PieceType::Pawn, Color::Black), 6, c);
}

Figure* Table::getFigureAt(int r, int c) const {
    if (!inBounds(r, c)) return nullptr;
    return cells[r][c].getFigure();
}

void Table::display() const {
    std::cout << "\n  a b c d e f g h\n";
    for (int r = 7; r >= 0; r--) {
        std::cout << (r + 1) << " ";
        for (int c = 0; c < 8; c++) {
            Figure* fig = cells[r][c].getFigure();
            if (fig)
                std::cout << fig->getSymbol();
            else
                std::cout << ((r + c) % 2 == 0 ? '.' : ',');
            std::cout << ' ';
        }
        std::cout << (r + 1) << "\n";
    }
    std::cout << "  a b c d e f g h\n\n";
}

bool Table::isSquareAttacked(int r, int c, Color byColor) const {
    // Knight
    int knightJ[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
    for (int i = 0; i < 8; i++) {
        int nr = r + knightJ[i][0], nc = c + knightJ[i][1];
        if (inBounds(nr, nc)) {
            Figure* fig = cells[nr][nc].getFigure();
            if (fig && fig->getColor() == byColor && fig->getType() == PieceType::Knight)
                return true;
        }
    }

    // Horizontal/vertical — rook or queen
    int straightD[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            int nr = r + straightD[d][0] * i, nc = c + straightD[d][1] * i;
            if (!inBounds(nr, nc)) break;
            Figure* fig = cells[nr][nc].getFigure();
            if (fig) {
                if (fig->getColor() == byColor &&
                    (fig->getType() == PieceType::Rook || fig->getType() == PieceType::Queen))
                    return true;
                break;
            }
        }
    }

    // Diagonal — bishop or queen
    int diagD[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (int d = 0; d < 4; d++) {
        for (int i = 1; i < 8; i++) {
            int nr = r + diagD[d][0] * i, nc = c + diagD[d][1] * i;
            if (!inBounds(nr, nc)) break;
            Figure* fig = cells[nr][nc].getFigure();
            if (fig) {
                if (fig->getColor() == byColor &&
                    (fig->getType() == PieceType::Bishop || fig->getType() == PieceType::Queen))
                    return true;
                break;
            }
        }
    }

    // King
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (inBounds(nr, nc)) {
                Figure* fig = cells[nr][nc].getFigure();
                if (fig && fig->getColor() == byColor && fig->getType() == PieceType::King)
                    return true;
            }
        }
    }

    // Pawn
    int pawnRow = (byColor == Color::White) ? r - 1 : r + 1;
    int pawnDcs[2] = {-1, 1};
    for (int i = 0; i < 2; i++) {
        int nc = c + pawnDcs[i];
        if (inBounds(pawnRow, nc)) {
            Figure* fig = cells[pawnRow][nc].getFigure();
            if (fig && fig->getColor() == byColor && fig->getType() == PieceType::Pawn)
                return true;
        }
    }

    return false;
}

bool Table::isInCheck(Color color) const {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* fig = cells[r][c].getFigure();
            if (fig && fig->getColor() == color && fig->getType() == PieceType::King) {
                Color enemy = (color == Color::White) ? Color::Black : Color::White;
                return isSquareAttacked(r, c, enemy);
            }
        }
    }
    return true; // king missing — should never happen
}

bool Table::wouldLeaveInCheck(int fr, int fc, int tr, int tc) const {
    Figure* movingFig = cells[fr][fc].getFigure();
    Figure* toFig     = cells[tr][tc].getFigure();

    // En passant: diagonal pawn move to empty square
    int     epR   = -1, epC = -1;
    Figure* epFig = nullptr;
    if (movingFig && movingFig->getType() == PieceType::Pawn && fc != tc && toFig == nullptr) {
        epR = fr; epC = tc;
        epFig = cells[epR][epC].getFigure();
        cells[epR][epC].setFigure(nullptr);
    }

    // Castling: move rook for accurate check-detection
    int     rookFrom = -1, rookTo = -1;
    Figure* castleRook = nullptr;
    if (movingFig && movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) { rookFrom = 7; rookTo = fc + 1; }
        else         { rookFrom = 0; rookTo = fc - 1; }
        castleRook = cells[fr][rookFrom].getFigure();
        cells[fr][rookTo].setFigure(castleRook);
        cells[fr][rookFrom].setFigure(nullptr);
    }

    cells[tr][tc].setFigure(movingFig);
    cells[fr][fc].setFigure(nullptr);

    bool result = isInCheck(movingFig->getColor());

    // Restore
    cells[fr][fc].setFigure(movingFig);
    cells[tr][tc].setFigure(toFig);
    if (epR != -1)     cells[epR][epC].setFigure(epFig);
    if (rookFrom != -1) {
        cells[fr][rookFrom].setFigure(castleRook);
        cells[fr][rookTo].setFigure(nullptr);
    }

    return result;
}

std::vector<Position> Table::getValidMoves(int r, int c) const {
    Figure* fig = cells[r][c].getFigure();
    if (!fig) return std::vector<Position>();

    std::vector<Position> pseudo = fig->getPseudoMoves(r, c, *this);
    std::vector<Position> valid;
    for (size_t i = 0; i < pseudo.size(); i++) {
        if (!wouldLeaveInCheck(r, c, pseudo[i].first, pseudo[i].second))
            valid.push_back(pseudo[i]);
    }
    return valid;
}

bool Table::isPawnPromotion(int fr, int fc, int tr, int tc) const {
    (void)fc; (void)tc;
    Figure* fig = cells[fr][fc].getFigure();
    if (!fig || fig->getType() != PieceType::Pawn) return false;
    int promRow = (fig->getColor() == Color::White) ? 7 : 0;
    return tr == promRow;
}

bool Table::makeMove(int fr, int fc, int tr, int tc, PieceType promotion) {
    Figure* movingFig = cells[fr][fc].getFigure();
    if (!movingFig || movingFig->getColor() != currentTurn)
        return false;

    std::vector<Position> valid = getValidMoves(fr, fc);
    bool found = false;
    for (size_t i = 0; i < valid.size(); i++) {
        if (valid[i].first == tr && valid[i].second == tc) { found = true; break; }
    }
    if (!found) return false;

    Position newEP(-1, -1);

    // En passant capture
    if (movingFig->getType() == PieceType::Pawn && fc != tc && !cells[tr][tc].getFigure())
        removePieceAt(fr, tc);

    // Normal capture
    if (cells[tr][tc].getFigure())
        removePieceAt(tr, tc);

    // Castling: relocate the rook
    if (movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) {
            Figure* rook = cells[fr][7].getFigure();
            cells[fr][5].setFigure(rook);
            cells[fr][7].setFigure(nullptr);
            if (rook) rook->setHasMoved(true);
        } else {
            Figure* rook = cells[fr][0].getFigure();
            cells[fr][3].setFigure(rook);
            cells[fr][0].setFigure(nullptr);
            if (rook) rook->setHasMoved(true);
        }
    }

    // Double pawn push: record en-passant target
    if (movingFig->getType() == PieceType::Pawn && std::abs(tr - fr) == 2)
        newEP = Position((fr + tr) / 2, fc);

    cells[tr][tc].setFigure(movingFig);
    cells[fr][fc].setFigure(nullptr);
    movingFig->setHasMoved(true);

    // Pawn promotion — Figure::create() replaces the per-case switch
    int promRow = (movingFig->getColor() == Color::White) ? 7 : 0;
    if (movingFig->getType() == PieceType::Pawn && tr == promRow) {
        Color pc = movingFig->getColor();
        for (size_t i = 0; i < pieces.size(); i++) {
            if (pieces[i].get() == movingFig) {
                captured.push_back(std::move(pieces[i]));
                pieces.erase(pieces.begin() + static_cast<int>(i));
                break;
            }
        }
        // Queen is the default promotion piece if an unknown type is passed
        PieceType promoType = (promotion == PieceType::Rook   ||
                               promotion == PieceType::Bishop ||
                               promotion == PieceType::Knight)
                              ? promotion : PieceType::Queen;
        std::unique_ptr<Figure> newPiece = Figure::create(promoType, pc);
        newPiece->setHasMoved(true);
        cells[tr][tc].setFigure(newPiece.get());
        pieces.push_back(std::move(newPiece));
    }

    enPassantTarget = newEP;
    currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;

    // Checkmate / stalemate detection
    bool hasAnyMove = false;
    for (int r = 0; r < 8 && !hasAnyMove; r++)
        for (int c = 0; c < 8 && !hasAnyMove; c++) {
            Figure* f = cells[r][c].getFigure();
            if (f && f->getColor() == currentTurn && !getValidMoves(r, c).empty())
                hasAnyMove = true;
        }

    if (!hasAnyMove) {
        gameOver = true;
        if (isInCheck(currentTurn))
            winner = (currentTurn == Color::White) ? Color::Black : Color::White;
        else
            isDraw = true;
    }

    return true;
}
