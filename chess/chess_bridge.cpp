// C-совместимый API над C++ движком.
//
// extern "C" отключает name mangling — Python (ctypes) может вызывать функции по имени.
// Opaque handle: Python хранит Table* как void* (адрес), не зная тип.
// Схема: chess_gui.py → ctypes.CDLL("libchess.dylib") → chess_bridge.cpp → Table / AI / MateSearcher

#include "table.h"
#include "ai.h"
#include "mate_searcher.h"
#include <cstring>

extern "C" {

// Создаёт доску с начальной позицией. Память освобождается через chess_destroy().
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

// Записывает доску в строку из 64 символов (индекс = row*8+col).
// Белые: K Q R B N P; чёрные: k q r b n p; пусто: '.'.
void chess_get_board(void* handle, char* out) {
    Table* t = static_cast<Table*>(handle);
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            Figure* f = t->getFigureAt(r, c);
            out[r * 8 + c] = f ? f->getSymbol() : '.';
        }
    out[64] = '\0';
}

// Возвращает 'W' или 'B'.
char chess_get_turn(void* handle) {
    return static_cast<Table*>(handle)->getCurrentTurn() == Color::White ? 'W' : 'B';
}

// Возвращает 1 если мат или пат, 0 иначе.
int chess_is_game_over(void* handle) {
    return static_cast<Table*>(handle)->isGameOver() ? 1 : 0;
}

int chess_is_draw(void* handle) {
    return static_cast<Table*>(handle)->getIsDraw() ? 1 : 0;
}

// Возвращает 'W', 'B' или 'N' (нет победителя).
char chess_get_winner(void* handle) {
    Color::E w = static_cast<Table*>(handle)->getWinner();
    if (w == Color::White) return 'W';
    if (w == Color::Black) return 'B';
    return 'N';
}

int chess_is_in_check(void* handle, char color) {
    Color::E c = (color == 'W') ? Color::White : Color::Black;
    return static_cast<Table*>(handle)->isInCheck(c) ? 1 : 0;
}

// Заполняет positions парами [row, col] допустимых ходов. Возвращает количество ходов.
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

// Выполняет ход. promotion: 'Q'/'R'/'B'/'N'. Возвращает 1 при успехе.
int chess_make_move(void* handle, int fr, int fc, int tr, int tc, char promotion) {
    PieceType::E promo;
    switch (promotion) {
        case 'R': promo = PieceType::Rook;   break;
        case 'B': promo = PieceType::Bishop; break;
        case 'N': promo = PieceType::Knight; break;
        default:  promo = PieceType::Queen;  break;
    }
    return static_cast<Table*>(handle)->makeMove(fr, fc, tr, tc, promo) ? 1 : 0;
}

// Находит лучший ход (минимакс, глубина depth).
// out[5]: fr, fc, tr, tc, promotion (как int/ASCII).
// Возвращает 1 если ход найден.
int chess_get_best_move(void* handle, char color, int depth, int* out) {
    Color::E c = (color == 'W') ? Color::White : Color::Black;
    Move m = AI(depth).getBestMove(*static_cast<Table*>(handle), c);
    if (m.fr < 0) return 0;
    out[0] = m.fr; out[1] = m.fc; out[2] = m.tr; out[3] = m.tc;
    out[4] = static_cast<int>(m.promotion);
    return 1;
}

// Загружает позицию из файла. Возвращает 1 при успехе.
int chess_load_position(void* handle, const char* filename) {
    Table* t = static_cast<Table*>(handle);
    return MateSearcher::loadPosition(*t, std::string(filename)) ? 1 : 0;
}

// Проверяет корректность позиции на доске.
// Записывает описание ошибок в out (разделитель '\n'), максимум out_size байт.
// Возвращает 1 если позиция валидна, 0 если нет.
int chess_validate_position(void* handle, char* out, int out_size) {
    Table* t = static_cast<Table*>(handle);
    std::vector<std::string> errors;
    bool ok = MateSearcher::validatePosition(*t, errors);
    if (out && out_size > 0) {
        std::string msg;
        for (size_t i = 0; i < errors.size(); i++) {
            if (i > 0) msg += "\n";
            msg += errors[i];
        }
        std::strncpy(out, msg.c_str(), static_cast<size_t>(out_size) - 1);
        out[out_size - 1] = '\0';
    }
    return ok ? 1 : 0;
}

// Ищет форсированный мат за ≤ max_depth ходов.
// out[5]: fr, fc, tr, tc, promotion. Возвращает 1 если мат найден.
int chess_find_mate_move(void* handle, int max_depth, int* out) {
    Table* t = static_cast<Table*>(handle);
    Move m = MateSearcher::findMatingMove(*t, max_depth);
    if (m.fr < 0) return 0;
    out[0] = m.fr; out[1] = m.fc; out[2] = m.tr; out[3] = m.tc;
    out[4] = static_cast<int>(m.promotion);
    return 1;
}

// Решает задачу полностью: загрузка + поиск мата + запись решения.
// Возвращает 1 (мат в 1), 2 (мат в 2) или 0.
int chess_solve_puzzle(const char* input_file, const char* output_file) {
    return MateSearcher::solvePuzzle(std::string(input_file),
                                      std::string(output_file));
}

} // extern "C"