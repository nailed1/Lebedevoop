// C-совместимый API над C++ движком. Компилируется в libchess.dylib,
// загружается из Python через ctypes.
#include "table.h"
#include "ai.h"
#include "mate_searcher.h"
#include <cstring>

extern "C" {

// Создаёт новую доску с начальной позицией, возвращает непрозрачный указатель
void* chess_create() {
    Table* t = new Table();
    t->initialize();
    return t;
}

// Удаляет объект доски
void chess_destroy(void* handle) {
    delete static_cast<Table*>(handle);
}

// Сбрасывает позицию к начальной
void chess_reset(void* handle) {
    static_cast<Table*>(handle)->initialize();
}

// Записывает состояние доски в строку из 64 символов (строка 0 = ряд 1)
// Заглавные буквы — белые, строчные — чёрные, '.' — пустая клетка
void chess_get_board(void* handle, char* out) {
    Table* t = static_cast<Table*>(handle);
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            Figure* f = t->getFigureAt(r, c);
            out[r * 8 + c] = f ? f->getSymbol() : '.';
        }
    out[64] = '\0';
}

// Возвращает текущий ход: 'W' или 'B'
char chess_get_turn(void* handle) {
    return static_cast<Table*>(handle)->getCurrentTurn() == Color::White ? 'W' : 'B';
}

// Возвращает 1, если игра закончена
int chess_is_game_over(void* handle) {
    return static_cast<Table*>(handle)->isGameOver() ? 1 : 0;
}

// Возвращает 1, если ничья (пат)
int chess_is_draw(void* handle) {
    return static_cast<Table*>(handle)->getIsDraw() ? 1 : 0;
}

// Возвращает победителя: 'W', 'B' или 'N' (нет)
char chess_get_winner(void* handle) {
    Color::E w = static_cast<Table*>(handle)->getWinner();
    if (w == Color::White) return 'W';
    if (w == Color::Black) return 'B';
    return 'N';
}

// Возвращает 1, если указанный цвет сейчас под шахом
int chess_is_in_check(void* handle, char color) {
    Color::E c = (color == 'W') ? Color::White : Color::Black;
    return static_cast<Table*>(handle)->isInCheck(c) ? 1 : 0;
}

// Записывает в positions все допустимые ходы для фигуры на (r,c).
// Формат: [row0, col0, row1, col1, ...]. Возвращает количество ходов.
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

// Делает ход (fr,fc)->(tr,tc). promotion: 'Q'/'R'/'B'/'N'. Возвращает 1 при успехе.
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

// Ищет лучший ход для color через минимакс глубиной depth.
// Результат в out[0..4]: fr, fc, tr, tc, символ превращения (int).
// Возвращает 1, если ход найден.
int chess_get_best_move(void* handle, char color, int depth, int* out) {
    Color::E c = (color == 'W') ? Color::White : Color::Black;
    Move     m = AI(depth).getBestMove(*static_cast<Table*>(handle), c);
    if (m.fr < 0) return 0;
    out[0] = m.fr;
    out[1] = m.fc;
    out[2] = m.tr;
    out[3] = m.tc;
    out[4] = static_cast<int>(m.promotion);
    return 1;
}

// Загружает позицию из файла задачи. Возвращает 1 при успехе.
int chess_load_position(void* handle, const char* filename) {
    Table* t = static_cast<Table*>(handle);
    return MateSearcher::loadPosition(*t, std::string(filename)) ? 1 : 0;
}

// Ищет ход белых, ведущий к форсированному мату не более чем за max_depth ходов.
// Результат в out[0..4]: fr, fc, tr, tc, символ превращения (int).
// Возвращает 1, если мат найден.
int chess_find_mate_move(void* handle, int max_depth, int* out) {
    Table* t = static_cast<Table*>(handle);
    Move m = MateSearcher::findMatingMove(*t, max_depth);
    if (m.fr < 0) return 0;
    out[0] = m.fr;
    out[1] = m.fc;
    out[2] = m.tr;
    out[3] = m.tc;
    out[4] = static_cast<int>(m.promotion);
    return 1;
}

// Полное решение задачи: загрузить из input_file, найти мат, записать в output_file.
// Возвращает количество ходов белых в решении (0 = не найдено).
int chess_solve_puzzle(const char* input_file, const char* output_file) {
    return MateSearcher::solvePuzzle(std::string(input_file),
                                      std::string(output_file));
}

} // extern "C"
