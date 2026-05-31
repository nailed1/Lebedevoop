#ifndef MATE_SEARCHER_H
#define MATE_SEARCHER_H

#include "table.h"
#include "ai.h"
#include <string>
#include <vector>

// Поиск принудительного мата в N ходов.
// Загружает позицию из текстового файла формата задачи и ищет выигрывающий ход белых.
class MateSearcher {
public:
    // Читает файл задачи и расставляет фигуры на доске
    static bool loadPosition(Table& table, const std::string& filename);

    // Ищет ход, ведущий к мату не более чем за maxDepth полуходов.
    // Возвращает Move с fr=-1, если мат не найден.
    static Move findMatingMove(const Table& board, int maxDepth);

    // Полное решение задачи: загружает позицию, ищет мат, пишет результат в файл.
    // Возвращает количество ходов белых в решении (0 = не найдено).
    static int solvePuzzle(const std::string& inputFile,
                            const std::string& outputFile);

private:
    static bool mateIn1(const Table& t, Move& out);
    static bool mateIn2(const Table& t, Move& out);

    static std::vector<Move> collectMoves(const Table& t, Color::E color);
    static PieceType::E      charToPromo(char p);
    static std::string       squareStr(int r, int c);
    static std::string       moveStr(const Table& t, const Move& m);
};

#endif // MATE_SEARCHER_H
