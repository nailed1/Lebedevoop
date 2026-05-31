#include "mate_searcher.h"
#include <fstream>
#include <cctype>
#include <cstring>

std::string MateSearcher::squareStr(int r, int c) {
    std::string s;
    s += static_cast<char>('a' + c);
    s += static_cast<char>('1' + r);
    return s;
}

// Формирует строку хода в алгебраической нотации, например "Qd5-d8" или "Nc4xe5"
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
                bool isPromo = (f->getType() == PieceType::Pawn &&
                                (tr == 0 || tr == 7));
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

// Проверяет, есть ли у белых мат за один ход
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

// Проверяет, есть ли у белых форсированный мат за два хода:
// нужно найти ход белых, после которого ВСЕ ответы чёрных ведут к мату в 1
bool MateSearcher::mateIn2(const Table& t, Move& out) {
    if (mateIn1(t, out)) return true;

    std::vector<Move> white_moves = collectMoves(t, Color::White);
    for (size_t wi = 0; wi < white_moves.size(); wi++) {
        const Move& wm = white_moves[wi];
        Table after_white = t;
        after_white.makeMove(wm.fr, wm.fc, wm.tr, wm.tc, charToPromo(wm.promotion));

        if (after_white.isGameOver()) continue;

        std::vector<Move> black_moves = collectMoves(after_white, Color::Black);
        if (black_moves.empty()) continue; // пат — не мат

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

Move MateSearcher::findMatingMove(const Table& board, int maxDepth) {
    Move result;
    result.fr = -1; result.fc = -1; result.tr = -1; result.tc = -1;
    result.promotion = 'Q';

    if (maxDepth >= 1 && mateIn1(board, result)) return result;
    if (maxDepth >= 2 && mateIn2(board, result)) return result;

    return result;
}

// Парсит файл задачи формата:
//   White: N
//   QA4        <- буква фигуры + столбец (A-H) + ряд (1-8)
//   Black: M
//   KD8
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

        if (line.size() >= 6 && line.substr(0, 6) == "White:") {
            currentColor = Color::White;
            continue;
        }
        if (line.size() >= 6 && line.substr(0, 6) == "Black:") {
            currentColor = Color::Black;
            continue;
        }

        if (line.size() < 3) continue;

        char pieceChar = line[0];
        char fileChar  = static_cast<char>(
                            toupper(static_cast<unsigned char>(line[1])));
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

        // Показываем один пример ответа чёрных и ответный мат белых
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

    out << "Форсированный мат в 1-2 хода не найден.\n";
    return 0;
}
