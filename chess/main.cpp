// Консольный шахматный интерфейс. Ввод ходов: e2e4, превращение: e7e8q
#include "table.h"
#include <iostream>
#include <string>
#include <cctype>

static Position parseSquare(const std::string& s) {
    if (s.size() < 2) return Position(-1, -1);
    int col = s[0] - 'a';
    int row = s[1] - '1';
    if (col < 0 || col > 7 || row < 0 || row > 7) return Position(-1, -1);
    return Position(row, col);
}

int main() {
    Table board;
    board.initialize();

    std::cout << "=== Шахматы (консоль) ===\n";
    std::cout << "Ходы: e2e4  |  Превращение: e7e8q / e7e8r / e7e8b / e7e8n\n";
    std::cout << "Заглавные = белые, строчные = чёрные\n\n";

    while (!board.isGameOver()) {
        board.display();

        Color::E    turn     = board.getCurrentTurn();
        std::string turnName = (turn == Color::White) ? "Белые" : "Чёрные";
        std::cout << turnName;
        if (board.isInCheck(turn))
            std::cout << " (ШАХ!)";
        std::cout << " > ";

        std::string line;
        if (!std::getline(std::cin, line)) break;

        // Приводим к нижнему регистру, убираем пробелы
        std::string input;
        for (size_t i = 0; i < line.size(); i++) {
            char ch = line[i];
            if (ch != ' ' && ch != '\t')
                input += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        }

        if (input == "quit" || input == "exit") {
            std::cout << "Выход.\n";
            return 0;
        }
        if (input == "help") {
            std::cout << "Введите ход: e2e4. Для превращения: e7e8q (q/r/b/n).\n";
            continue;
        }
        if (input.size() < 4) {
            std::cout << "Неверный ввод. Пример: e2e4\n";
            continue;
        }

        Position from = parseSquare(input.substr(0, 2));
        Position to   = parseSquare(input.substr(2, 2));
        if (from.first < 0 || to.first < 0) {
            std::cout << "Неверная клетка. Используйте a-h и 1-8.\n";
            continue;
        }

        PieceType::E promo = PieceType::Queen;
        if (input.size() >= 5) {
            switch (input[4]) {
                case 'r': promo = PieceType::Rook;   break;
                case 'b': promo = PieceType::Bishop; break;
                case 'n': promo = PieceType::Knight; break;
                default:  promo = PieceType::Queen;  break;
            }
        }

        if (!board.makeMove(from.first, from.second, to.first, to.second, promo))
            std::cout << "Недопустимый ход!\n";
    }

    if (board.isGameOver()) {
        board.display();
        if (board.getIsDraw()) {
            std::cout << "Пат — ничья!\n";
        } else {
            std::string w = (board.getWinner() == Color::White) ? "Белые" : "Чёрные";
            std::cout << w << " победили матом!\n";
        }
    }

    return 0;
}
