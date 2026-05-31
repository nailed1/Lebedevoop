#include "table.h"
#include <iostream>
#include <cstdlib>

Table::Table()
    : currentTurn(Color::White), enPassantTarget(Position(-1, -1)),
      gameOver(false), winner(Color::None), isDraw(false)
{
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            cells[r][c] = Cell(r, c); // все клетки пустые (figure = NULL)
}

// [?] Конструктор копирования — вызывается каждый раз когда пишем: Table copy = t;
//     В минимаксе это происходит в каждом узле дерева поиска.
//     Глубокое копирование: clone() для каждой занятой клетки — создаём независимую копию фигур.
//     Без этого изменения на копии-доске влияли бы на оригинал (общие указатели).
Table::Table(const Table& other)
    : currentTurn(other.currentTurn),
      enPassantTarget(other.enPassantTarget),
      gameOver(other.gameOver),
      winner(other.winner),
      isDraw(other.isDraw)
{
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            cells[r][c] = Cell(r, c);
            Figure* orig = other.cells[r][c].getFigure();
            // clone() для занятых клеток, NULL для пустых
            cells[r][c].setFigure(orig ? orig->clone() : NULL);
        }
    }
    // captured больше нет — снятые фигуры удаляются сразу при взятии
}

// [?] delete NULL — в стандарте C++ это явно определено как no-op (ничего не делает, не падает).
//     Поэтому проверять if (fig != NULL) перед delete не нужно — освобождаем все 64 клетки без условий.
Table::~Table() {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            delete cells[r][c].getFigure();
}

// Очищает доску: удаляет все фигуры и сбрасывает состояние игры
void Table::clearBoard() {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            delete cells[r][c].getFigure(); // delete NULL безопасен
            cells[r][c] = Cell(r, c);
        }
    }
    currentTurn     = Color::White;
    enPassantTarget = Position(-1, -1);
    gameOver        = false;
    winner          = Color::None;
    isDraw          = false;
}

void Table::placePiece(PieceType::E t, Color::E c, int row, int col) {
    if (!inBounds(row, col)) return;
    addPiece(Figure::create(t, c), row, col);
}

// Помещает фигуру на клетку (предполагается, что клетка пуста)
void Table::addPiece(Figure* fig, int r, int c) {
    cells[r][c].setFigure(fig);
}

// Удаляет фигуру с клетки — O(1), без поиска в векторе
void Table::removePieceAt(int r, int c) {
    delete cells[r][c].getFigure(); // освобождаем память сразу
    cells[r][c].setFigure(NULL);
}

// Расставляет начальную шахматную позицию
void Table::initialize() {
    // Удаляем всё что есть на доске
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            delete cells[r][c].getFigure();
            cells[r][c] = Cell(r, c);
        }
    }

    currentTurn     = Color::White;
    enPassantTarget = Position(-1, -1);
    gameOver        = false;
    winner          = Color::None;
    isDraw          = false;

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
    if (!inBounds(r, c)) return NULL;
    return cells[r][c].getFigure();
}

// Вывод доски в консоль (для консольного режима и отладки)
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

bool Table::isSquareAttacked(int r, int c, Color::E byColor) const {
    // Конь
    int knightJ[8][2] = {{2,1},{2,-1},{-2,1},{-2,-1},{1,2},{1,-2},{-1,2},{-1,-2}};
    for (int i = 0; i < 8; i++) {
        int nr = r + knightJ[i][0], nc = c + knightJ[i][1];
        if (inBounds(nr, nc)) {
            Figure* fig = cells[nr][nc].getFigure();
            if (fig && fig->getColor() == byColor && fig->getType() == PieceType::Knight)
                return true;
        }
    }

    // Ладья или ферзь по горизонтали/вертикали
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

    // Слон или ферзь по диагонали
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

    // Король
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

    // Пешка
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

bool Table::isInCheck(Color::E color) const {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* fig = cells[r][c].getFigure();
            if (fig && fig->getColor() == color && fig->getType() == PieceType::King) {
                Color::E enemy = (color == Color::White) ? Color::Black : Color::White;
                return isSquareAttacked(r, c, enemy);
            }
        }
    }
    return true;
}

// [?] const-метод, но изменяет cells — это законно из-за mutable.
//     "Логически const": со стороны вызывающего кода позиция не изменилась (всё восстанавливается).
//     "Физически" — переставляет указатели для симуляции хода, затем откатывает всё обратно.
//     Ничего не удаляет (нет delete) — только setFigure(). Поэтому safe для mutable-доступа.
bool Table::wouldLeaveInCheck(int fr, int fc, int tr, int tc) const {
    Figure* movingFig = cells[fr][fc].getFigure();
    Figure* toFig     = cells[tr][tc].getFigure(); // может быть NULL

    // Взятие на проходе: пешка идёт по диагонали на пустую клетку
    int     epR   = -1, epC = -1;
    Figure* epFig = NULL;
    if (movingFig && movingFig->getType() == PieceType::Pawn && fc != tc && toFig == NULL) {
        epR = fr; epC = tc;
        epFig = cells[epR][epC].getFigure();
        cells[epR][epC].setFigure(NULL); // временно убираем
    }

    // Рокировка: перемещаем ладью для корректной проверки шаха
    int     rookFrom = -1, rookTo = -1;
    Figure* castleRook = NULL;
    if (movingFig && movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) { rookFrom = 7; rookTo = fc + 1; }
        else         { rookFrom = 0; rookTo = fc - 1; }
        castleRook = cells[fr][rookFrom].getFigure();
        cells[fr][rookTo].setFigure(castleRook);
        cells[fr][rookFrom].setFigure(NULL);
    }

    cells[tr][tc].setFigure(movingFig);
    cells[fr][fc].setFigure(NULL);

    bool result = isInCheck(movingFig->getColor());

    // Восстанавливаем позицию — ничего не было удалено
    cells[fr][fc].setFigure(movingFig);
    cells[tr][tc].setFigure(toFig);
    if (epR != -1)      cells[epR][epC].setFigure(epFig);
    if (rookFrom != -1) {
        cells[fr][rookFrom].setFigure(castleRook);
        cells[fr][rookTo].setFigure(NULL);
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

bool Table::makeMove(int fr, int fc, int tr, int tc, PieceType::E promotion) {
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

    // Взятие на проходе: убираем пешку с её реальной клетки (fr, tc)
    if (movingFig->getType() == PieceType::Pawn && fc != tc && !cells[tr][tc].getFigure())
        removePieceAt(fr, tc);

    // Обычное взятие: удаляем вражескую фигуру — O(1), без поиска в векторе
    if (cells[tr][tc].getFigure())
        removePieceAt(tr, tc);

    // Рокировка: перемещаем ладью
    if (movingFig->getType() == PieceType::King && std::abs(tc - fc) == 2) {
        if (tc > fc) {
            Figure* rook = cells[fr][7].getFigure();
            cells[fr][5].setFigure(rook);
            cells[fr][7].setFigure(NULL);
            if (rook) rook->setHasMoved(true);
        } else {
            Figure* rook = cells[fr][0].getFigure();
            cells[fr][3].setFigure(rook);
            cells[fr][0].setFigure(NULL);
            if (rook) rook->setHasMoved(true);
        }
    }

    // Двойной ход пешки: записываем клетку взятия на проходе
    if (movingFig->getType() == PieceType::Pawn && std::abs(tr - fr) == 2)
        newEP = Position((fr + tr) / 2, fc);

    cells[tr][tc].setFigure(movingFig);
    cells[fr][fc].setFigure(NULL);
    movingFig->setHasMoved(true);

    // Превращение пешки
    int promRow = (movingFig->getColor() == Color::White) ? 7 : 0;
    if (movingFig->getType() == PieceType::Pawn && tr == promRow) {
        Color::E pc = movingFig->getColor();
        PieceType::E promoType = (promotion == PieceType::Rook   ||
                                   promotion == PieceType::Bishop ||
                                   promotion == PieceType::Knight)
                                  ? promotion : PieceType::Queen;
        // Удаляем пешку, создаём новую фигуру — без поиска в pieces
        Figure* newPiece = Figure::create(promoType, pc);
        newPiece->setHasMoved(true);
        delete movingFig;                    // пешка больше не нужна
        cells[tr][tc].setFigure(newPiece);   // заменяем в клетке
    }

    enPassantTarget = newEP;
    currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;

    // Проверка мата и пата
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
