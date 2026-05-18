#include "table.h"
#include <iostream>
#include <cstdlib>

// ── Deep-copy helper ──────────────────────────────────────────────────────────
static std::unique_ptr<Figure> cloneFigure(const Figure* f) {
    std::unique_ptr<Figure> c;
    switch (f->getType()) {
        case PieceType::King:   c = std::make_unique<King>  (f->getColor()); break;
        case PieceType::Queen:  c = std::make_unique<Queen> (f->getColor()); break;
        case PieceType::Rook:   c = std::make_unique<Rook>  (f->getColor()); break;
        case PieceType::Bishop: c = std::make_unique<Bishop>(f->getColor()); break;
        case PieceType::Knight: c = std::make_unique<Knight>(f->getColor()); break;
        case PieceType::Pawn:   c = std::make_unique<Pawn>  (f->getColor()); break;
        default: break;
    }
    if (c) c->setHasMoved(f->getHasMoved());
    return c;
}

// Copy constructor: creates an independent snapshot of the board (needed by AI).
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

    // Clone every piece that sits on a cell (= all active pieces).
    // This naturally populates `pieces` in board order.
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* orig = other.cells[r][c].getFigure();
            if (orig) {
                auto clone = cloneFigure(orig);
                cells[r][c].setFigure(clone.get());
                pieces.push_back(std::move(clone));
            }
        }
    }

    // Clone captured pieces (not used by the search, but keeps the object consistent).
    for (auto& uptr : other.captured)
        captured.push_back(cloneFigure(uptr.get()));
}

Table::Table()
    : currentTurn(Color::White), enPassantTarget({-1, -1}),
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
    for (auto it = pieces.begin(); it != pieces.end(); ++it) {
        if (it->get() == fig) {
            captured.push_back(std::move(*it));
            pieces.erase(it);
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
    currentTurn = Color::White;
    enPassantTarget = {-1, -1};
    gameOver = false;
    winner = Color::None;
    isDraw = false;

    // White: back rank (row 0), pawns (row 1)
    addPiece(std::make_unique<Rook>(Color::White),   0, 0);
    addPiece(std::make_unique<Knight>(Color::White), 0, 1);
    addPiece(std::make_unique<Bishop>(Color::White), 0, 2);
    addPiece(std::make_unique<Queen>(Color::White),  0, 3);
    addPiece(std::make_unique<King>(Color::White),   0, 4);
    addPiece(std::make_unique<Bishop>(Color::White), 0, 5);
    addPiece(std::make_unique<Knight>(Color::White), 0, 6);
    addPiece(std::make_unique<Rook>(Color::White),   0, 7);
    for (int c = 0; c < 8; c++)
        addPiece(std::make_unique<Pawn>(Color::White), 1, c);

    // Black: back rank (row 7), pawns (row 6)
    addPiece(std::make_unique<Rook>(Color::Black),   7, 0);
    addPiece(std::make_unique<Knight>(Color::Black), 7, 1);
    addPiece(std::make_unique<Bishop>(Color::Black), 7, 2);
    addPiece(std::make_unique<Queen>(Color::Black),  7, 3);
    addPiece(std::make_unique<King>(Color::Black),   7, 4);
    addPiece(std::make_unique<Bishop>(Color::Black), 7, 5);
    addPiece(std::make_unique<Knight>(Color::Black), 7, 6);
    addPiece(std::make_unique<Rook>(Color::Black),   7, 7);
    for (int c = 0; c < 8; c++)
        addPiece(std::make_unique<Pawn>(Color::Black), 6, c);
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
    for (auto& j : knightJ) {
        int nr = r + j[0], nc = c + j[1];
        if (inBounds(nr, nc)) {
            Figure* fig = cells[nr][nc].getFigure();
            if (fig && fig->getColor() == byColor && fig->getType() == PieceType::Knight)
                return true;
        }
    }

    // Horizontal/vertical (rook, queen)
    int straightD[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    for (auto& d : straightD) {
        for (int i = 1; i < 8; i++) {
            int nr = r + d[0] * i, nc = c + d[1] * i;
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

    // Diagonal (bishop, queen)
    int diagD[4][2] = {{1,1},{1,-1},{-1,1},{-1,-1}};
    for (auto& d : diagD) {
        for (int i = 1; i < 8; i++) {
            int nr = r + d[0] * i, nc = c + d[1] * i;
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

    // Pawn: a white pawn at (r-1, c±1) attacks (r, c); black at (r+1, c±1)
    int pawnRow = (byColor == Color::White) ? r - 1 : r + 1;
    for (int dc : {-1, 1}) {
        int nc = c + dc;
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
    return true; // king not found (shouldn't happen)
}

bool Table::wouldLeaveInCheck(int fr, int fc, int tr, int tc) const {
    Figure* movingFig = cells[fr][fc].getFigure();
    Figure* toFig = cells[tr][tc].getFigure();

    // En passant: diagonal pawn move to empty square
    int epR = -1, epC = -1;
    Figure* epFig = nullptr;
    if (movingFig && movingFig->getType() == PieceType::Pawn && fc != tc && toFig == nullptr) {
        epR = fr; epC = tc;
        epFig = cells[epR][epC].getFigure();
        cells[epR][epC].setFigure(nullptr);
    }

    // Castling: also move the rook so simulation is accurate
    int rookFrom = -1, rookTo = -1;
    Figure* castleRook = nullptr;
    if (movingFig && movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) { // kingside
            rookFrom = 7; rookTo = fc + 1;
        } else {        // queenside
            rookFrom = 0; rookTo = fc - 1;
        }
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
    if (epR != -1)
        cells[epR][epC].setFigure(epFig);
    if (rookFrom != -1) {
        cells[fr][rookFrom].setFigure(castleRook);
        cells[fr][rookTo].setFigure(nullptr);
    }

    return result;
}

std::vector<Position> Table::getValidMoves(int r, int c) const {
    Figure* fig = cells[r][c].getFigure();
    if (!fig) return {};

    auto pseudo = fig->getPseudoMoves(r, c, *this);
    std::vector<Position> valid;
    for (auto& m : pseudo) {
        if (!wouldLeaveInCheck(r, c, m.first, m.second))
            valid.push_back(m);
    }
    return valid;
}

bool Table::isPawnPromotion(int fr, int fc, int tr, int tc) const {
    Figure* fig = cells[fr][fc].getFigure();
    if (!fig || fig->getType() != PieceType::Pawn) return false;
    int promRow = (fig->getColor() == Color::White) ? 7 : 0;
    return tr == promRow && fc == tc ? !cells[tr][tc].getFigure() || true : tr == promRow;
}

bool Table::makeMove(int fr, int fc, int tr, int tc, PieceType promotion) {
    Figure* movingFig = cells[fr][fc].getFigure();
    if (!movingFig || movingFig->getColor() != currentTurn)
        return false;

    auto valid = getValidMoves(fr, fc);
    bool found = false;
    for (auto& m : valid) {
        if (m.first == tr && m.second == tc) { found = true; break; }
    }
    if (!found) return false;

    Position newEP = {-1, -1};

    // En passant capture
    if (movingFig->getType() == PieceType::Pawn && fc != tc && !cells[tr][tc].getFigure())
        removePieceAt(fr, tc);

    // Normal capture
    if (cells[tr][tc].getFigure())
        removePieceAt(tr, tc);

    // Castling: move the rook
    if (movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) { // kingside
            Figure* rook = cells[fr][7].getFigure();
            cells[fr][5].setFigure(rook);
            cells[fr][7].setFigure(nullptr);
            if (rook) rook->setHasMoved(true);
        } else {        // queenside
            Figure* rook = cells[fr][0].getFigure();
            cells[fr][3].setFigure(rook);
            cells[fr][0].setFigure(nullptr);
            if (rook) rook->setHasMoved(true);
        }
    }

    // Pawn double push: set en passant target
    if (movingFig->getType() == PieceType::Pawn && std::abs(tr - fr) == 2)
        newEP = {(fr + tr) / 2, fc};

    // Move piece
    cells[tr][tc].setFigure(movingFig);
    cells[fr][fc].setFigure(nullptr);
    movingFig->setHasMoved(true);

    // Pawn promotion
    int promRow = (movingFig->getColor() == Color::White) ? 7 : 0;
    if (movingFig->getType() == PieceType::Pawn && tr == promRow) {
        Color pc = movingFig->getColor();
        // Remove old pawn
        for (auto it = pieces.begin(); it != pieces.end(); ++it) {
            if (it->get() == movingFig) {
                captured.push_back(std::move(*it));
                pieces.erase(it);
                break;
            }
        }
        std::unique_ptr<Figure> newPiece;
        switch (promotion) {
            case PieceType::Rook:   newPiece = std::make_unique<Rook>(pc);   break;
            case PieceType::Bishop: newPiece = std::make_unique<Bishop>(pc); break;
            case PieceType::Knight: newPiece = std::make_unique<Knight>(pc); break;
            default:                newPiece = std::make_unique<Queen>(pc);  break;
        }
        newPiece->setHasMoved(true);
        cells[tr][tc].setFigure(newPiece.get());
        pieces.push_back(std::move(newPiece));
    }

    enPassantTarget = newEP;
    currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;

    // Check for checkmate / stalemate
    bool hasAnyMove = false;
    for (int r = 0; r < 8 && !hasAnyMove; r++)
        for (int c = 0; c < 8 && !hasAnyMove; c++) {
            Figure* f = cells[r][c].getFigure();
            if (f && f->getColor() == currentTurn && !getValidMoves(r, c).empty())
                hasAnyMove = true;
        }

    if (!hasAnyMove) {
        gameOver = true;
        if (isInCheck(currentTurn)) {
            winner = (currentTurn == Color::White) ? Color::Black : Color::White;
        } else {
            isDraw = true;
        }
    }

    return true;
}
