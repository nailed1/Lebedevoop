#ifndef MATE_SEARCHER_H
#define MATE_SEARCHER_H

#include "table.h"
#include "ai.h"
#include <string>
#include <vector>

// Поиск форсированного мата (задачи "мат в N ходов").
// Все методы статические — экземпляр класса не нужен.
// Отличие от AI: гарантирует мат при ВСЕХ ответах соперника, а не просто лучший ход.
class MateSearcher {
public:
    // Загружает позицию из файла (формат: "White:\nKE1\nQA4\nBlack:\nKD8").
    // Возвращает false если файл не открылся.
    static bool loadPosition(Table& table, const std::string& filename);

    // Ищет ход белых, ведущий к форсированному мату за ≤ maxDepth ходов.
    // Возвращает Move с fr=-1 если мат не найден.
    static Move findMatingMove(const Table& board, int maxDepth);

    // Загружает позицию, ищет мат, записывает решение в outputFile.
    // Возвращает 1 (мат в 1), 2 (мат в 2) или 0 (не найден).
    static int solvePuzzle(const std::string& inputFile,
                            const std::string& outputFile);

private:
    static bool mateIn1(const Table& t, Move& out);
    static bool mateIn2(const Table& t, Move& out);
    static std::vector<Move> collectMoves(const Table& t, Color::E color);
    static PieceType::E charToPromo(char p);
    static std::string squareStr(int r, int c);
    static std::string moveStr(const Table& t, const Move& m);
};

#endif // MATE_SEARCHER_H
