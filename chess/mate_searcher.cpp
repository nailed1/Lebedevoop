#include "mate_searcher.h"
#include <fstream>
#include <cctype>
#include <cstring>
#include <cstdio>

std::string MateSearcher::squareStr(int r, int c) {
    std::string s;
    s += static_cast<char>('a' + c);
    s += static_cast<char>('1' + r);
    return s;
}

std::string MateSearcher::moveStr(const Table& t, const Move& m) {
    Figure* f = t.getFigureAt(m.fr, m.fc);
    std::string piece;
    if (f) {
        switch (f->getType()) {
            case PieceType::Queen:  piece = "Q"; break;
            case PieceType::Rook:   piece = "R"; break;
            case PieceType::Bishop: piece = "B"; break;
            case PieceType::Knight: piece = "N"; break;
            case PieceType::King:   piece = "K"; break;
            default:                piece = "";  break;
        }
    }
    Figure* cap = t.getFigureAt(m.tr, m.tc);
    std::string sep = cap ? "x" : "-";
    std::string s = piece + squareStr(m.fr, m.fc) + sep + squareStr(m.tr, m.tc);
    if (f && f->getType() == PieceType::Pawn && (m.tr == 0 || m.tr == 7))
        s += std::string("=") + m.promotion;
    return s;
}

PieceType::E MateSearcher::charToPromo(char p) {
    switch (p) {
        case 'R': return PieceType::Rook;
        case 'B': return PieceType::Bishop;
        case 'N': return PieceType::Knight;
        default:  return PieceType::Queen;
    }
}

std::vector<Move> MateSearcher::collectMoves(const Table& t, Color::E color) {
    std::vector<Move> moves;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = t.getFigureAt(r, c);
            if (!f || f->getColor() != color) continue;
            std::vector<Position> dests = t.getValidMoves(r, c);
            for (size_t i = 0; i < dests.size(); i++) {
                int tr = dests[i].first, tc = dests[i].second;
                bool isPromo = (f->getType() == PieceType::Pawn && (tr == 0 || tr == 7));
                if (isPromo) {
                    const char ps[] = {'Q','R','B','N'};
                    for (int p = 0; p < 4; p++) {
                        Move m; m.fr = r; m.fc = c;
                        m.tr = tr; m.tc = tc; m.promotion = ps[p];
                        moves.push_back(m);
                    }
                } else {
                    Move m; m.fr = r; m.fc = c;
                    m.tr = tr; m.tc = tc; m.promotion = 'Q';
                    moves.push_back(m);
                }
            }
        }
    }
    return moves;
}

bool MateSearcher::validatePosition(const Table& board,
                                     std::vector<std::string>& errors) {
    errors.clear();

    int kings[2]   = {0, 0};
    int pawns[2]   = {0, 0};
    int queens[2]  = {0, 0};
    int rooks[2]   = {0, 0};
    int bishops[2] = {0, 0};
    int knights[2] = {0, 0};
    int total[2]   = {0, 0};

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = board.getFigureAt(r, c);
            if (!f) continue;
            int idx = (f->getColor() == Color::White) ? 0 : 1;
            total[idx]++;
            switch (f->getType()) {
                case PieceType::King:   kings[idx]++;   break;
                case PieceType::Queen:  queens[idx]++;  break;
                case PieceType::Rook:   rooks[idx]++;   break;
                case PieceType::Bishop: bishops[idx]++; break;
                case PieceType::Knight: knights[idx]++; break;
                case PieceType::Pawn:   pawns[idx]++;   break;
                default: break;
            }
        }
    }

    const char* colorName[2] = {"Белые", "Чёрные"};

    for (int i = 0; i < 2; i++) {
        // 1. Ровно один король
        if (kings[i] != 1) {
            char buf[128];
            std::sprintf(buf, "%s: королей на доске %d, должен быть ровно 1.",
                         colorName[i], kings[i]);
            errors.push_back(std::string(buf));
        }

        // 2. Не более 16 фигур на сторону
        if (total[i] > 16) {
            char buf[64];
            std::sprintf(buf, "%s: фигур на доске %d, максимум 16.",
                         colorName[i], total[i]);
            errors.push_back(std::string(buf));
        }

        // 3. Пешки: 0–8
        if (pawns[i] > 8) {
            char buf[64];
            std::sprintf(buf, "%s: пешек %d, допустимо от 0 до 8.",
                         colorName[i], pawns[i]);
            errors.push_back(std::string(buf));
        }

        // 4. Превращённые фигуры
        // Лишние = те, что сверх стартового набора без учёта пешек: 1Q, 2R, 2B, 2N.
        // Суммарное число лишних не должно превышать (8 - pawns[i]).
        int promotionsAvailable = 8 - pawns[i];
        int extraQueens  = (queens[i]  > 1) ? queens[i]  - 1 : 0;
        int extraRooks   = (rooks[i]   > 2) ? rooks[i]   - 2 : 0;
        int extraBishops = (bishops[i] > 2) ? bishops[i] - 2 : 0;
        int extraKnights = (knights[i] > 2) ? knights[i] - 2 : 0;
        int totalExtra   = extraQueens + extraRooks + extraBishops + extraKnights;

        if (totalExtra > promotionsAvailable) {
            char buf[256];
            std::sprintf(buf,
                "%s: лишних фигур %d (Q+%d R+%d B+%d N+%d), "
                "но возможных превращений только %d (пешек на доске: %d).",
                colorName[i],
                totalExtra, extraQueens, extraRooks, extraBishops, extraKnights,
                promotionsAvailable, pawns[i]);
            errors.push_back(std::string(buf));
        }
    }

    // 5. Суммарный лимит (redundant при корректном подсчёте выше, но явная защита)
    if (total[0] + total[1] > 32) {
        char buf[64];
        std::sprintf(buf, "Всего фигур на доске %d, максимум 32.",
                     total[0] + total[1]);
        errors.push_back(std::string(buf));
    }

    return errors.empty();
}

bool MateSearcher::mateIn1(const Table& t, Move& out) {
    std::vector<Move> moves = collectMoves(t, Color::White);
    for (size_t i = 0; i < moves.size(); i++) {
        const Move& m = moves[i];
        Table copy = t;
        copy.makeMove(m.fr, m.fc, m.tr, m.tc, charToPromo(m.promotion));
        if (copy.isGameOver() && copy.getWinner() == Color::White) {
            out = m;
            return true;
        }
    }
    return false;
}

// mateIn2: ход белых ведёт к мату только если ВСЕ ответы чёрных оставляют mateIn1.
bool MateSearcher::mateIn2(const Table& t, Move& out) {
    if (mateIn1(t, out)) return true;

    std::vector<Move> white_moves = collectMoves(t, Color::White);
    for (size_t wi = 0; wi < white_moves.size(); wi++) {
        const Move& wm = white_moves[wi];
        Table after_white = t;
        after_white.makeMove(wm.fr, wm.fc, wm.tr, wm.tc, charToPromo(wm.promotion));
        if (after_white.isGameOver()) continue;

        std::vector<Move> black_moves = collectMoves(after_white, Color::Black);
        if (black_moves.empty()) continue;

        bool all_lead_to_mate = true;
        for (size_t bi = 0; bi < black_moves.size(); bi++) {
            const Move& bm = black_moves[bi];
            Table after_black = after_white;
            after_black.makeMove(bm.fr, bm.fc, bm.tr, bm.tc, charToPromo(bm.promotion));
            Move dummy;
            if (!mateIn1(after_black, dummy)) {
                all_lead_to_mate = false;
                break;
            }
        }
        if (all_lead_to_mate) {
            out = wm;
            return true;
        }
    }
    return false;
}

// mateIn3: ход белых ведёт к мату только если ВСЕ ответы чёрных оставляют mateIn2.
bool MateSearcher::mateIn3(const Table& t, Move& out) {
    if (mateIn2(t, out)) return true;

    std::vector<Move> white_moves = collectMoves(t, Color::White);
    for (size_t wi = 0; wi < white_moves.size(); wi++) {
        const Move& wm = white_moves[wi];
        Table after_white = t;
        after_white.makeMove(wm.fr, wm.fc, wm.tr, wm.tc, charToPromo(wm.promotion));
        if (after_white.isGameOver()) continue;

        std::vector<Move> black_moves = collectMoves(after_white, Color::Black);
        if (black_moves.empty()) continue;

        bool all_lead_to_mate = true;
        for (size_t bi = 0; bi < black_moves.size(); bi++) {
            const Move& bm = black_moves[bi];
            Table after_black = after_white;
            after_black.makeMove(bm.fr, bm.fc, bm.tr, bm.tc, charToPromo(bm.promotion));
            Move dummy;
            if (!mateIn2(after_black, dummy)) {
                all_lead_to_mate = false;
                break;
            }
        }
        if (all_lead_to_mate) {
            out = wm;
            return true;
        }
    }
    return false;
}

Move MateSearcher::findMatingMove(const Table& board, int maxDepth) {
    Move result;
    result.fr = -1; result.fc = -1; result.tr = -1; result.tc = -1;
    result.promotion = 'Q';
    if (maxDepth >= 1 && mateIn1(board, result)) return result;
    if (maxDepth >= 2 && mateIn2(board, result)) return result;
    if (maxDepth >= 3 && mateIn3(board, result)) return result;
    return result;
}

bool MateSearcher::loadPosition(Table& table, const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) return false;

    table.clearBoard();
    Color::E currentColor = Color::White;
    std::string line;

    while (std::getline(file, line)) {
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        if (line.empty()) continue;

        if (line.size() >= 6 && line.substr(0, 6) == "White:") { currentColor = Color::White; continue; }
        if (line.size() >= 6 && line.substr(0, 6) == "Black:") { currentColor = Color::Black; continue; }
        if (line.size() < 3) continue;

        char pieceChar = line[0];
        char fileChar  = static_cast<char>(toupper(static_cast<unsigned char>(line[1])));
        char rankChar  = line[2];

        if (fileChar < 'A' || fileChar > 'H') continue;
        if (rankChar < '1' || rankChar > '8') continue;

        int col = fileChar - 'A';
        int row = rankChar - '1';

        PieceType::E type;
        switch (toupper(static_cast<unsigned char>(pieceChar))) {
            case 'K': type = PieceType::King;   break;
            case 'Q': type = PieceType::Queen;  break;
            case 'R': type = PieceType::Rook;   break;
            case 'B': type = PieceType::Bishop; break;
            case 'N': type = PieceType::Knight; break;
            case 'P': type = PieceType::Pawn;   break;
            default: continue;
        }
        table.placePiece(type, currentColor, row, col);
    }
    return true;
}

int MateSearcher::solvePuzzle(const std::string& inputFile,
                               const std::string& outputFile) {
    Table board;
    if (!loadPosition(board, inputFile)) return 0;

    std::ofstream out(outputFile.c_str());
    if (!out.is_open()) return 0;

    // Проверяем корректность позиции перед поиском мата
    std::vector<std::string> validationErrors;
    if (!validatePosition(board, validationErrors)) {
        out << "Позиция некорректна:\n";
        for (size_t i = 0; i < validationErrors.size(); i++)
            out << "  - " << validationErrors[i] << "\n";
        return 0;
    }

    Move wm1;
    if (mateIn1(board, wm1)) {
        out << "Решение: мат в 1 ход.\n";
        out << "1. " << moveStr(board, wm1) << "#\n";
        return 1;
    }

    Move wm2;
    if (mateIn2(board, wm2)) {
        out << "Решение: мат в 2 хода.\n";
        out << "1. " << moveStr(board, wm2) << "\n";

        Table after_w = board;
        after_w.makeMove(wm2.fr, wm2.fc, wm2.tr, wm2.tc, charToPromo(wm2.promotion));

        std::vector<Move> black_moves = collectMoves(after_w, Color::Black);
        if (!black_moves.empty()) {
            const Move& bm = black_moves[0];
            out << "   (например 1... " << moveStr(after_w, bm) << ")\n";

            Table after_b = after_w;
            after_b.makeMove(bm.fr, bm.fc, bm.tr, bm.tc, charToPromo(bm.promotion));

            Move wm_reply;
            if (mateIn1(after_b, wm_reply))
                out << "2. " << moveStr(after_b, wm_reply) << "#\n";
        }
        return 2;
    }

    Move wm3;
    if (mateIn3(board, wm3)) {
        out << "Решение: мат в 3 хода.\n";
        out << "1. " << moveStr(board, wm3) << "\n";
        return 3;
    }

    out << "Форсированный мат в 1-3 хода не найден.\n";
    return 0;
}