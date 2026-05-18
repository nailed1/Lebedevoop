#include "table.h"
#include <iostream>
#include <string>
#include <cctype>

static Position parseSquare(const std::string& s) {
    if (s.size() < 2) return {-1, -1};
    int col = s[0] - 'a';
    int row = s[1] - '1';
    if (col < 0 || col > 7 || row < 0 || row > 7) return {-1, -1};
    return {row, col};
}

int main() {
    Table board;
    board.initialize();

    std::cout << "=== Chess ===\n";
    std::cout << "Moves: e2e4  |  Promotion: e7e8q / e7e8r / e7e8b / e7e8n\n";
    std::cout << "Uppercase = White, lowercase = Black\n";
    std::cout << "Board: . = light square, , = dark square\n\n";

    while (!board.isGameOver()) {
        board.display();

        Color turn = board.getCurrentTurn();
        std::string turnName = (turn == Color::White) ? "White" : "Black";
        std::cout << turnName;
        if (board.isInCheck(turn))
            std::cout << " (CHECK!)";
        std::cout << " > ";

        std::string line;
        if (!std::getline(std::cin, line)) break;

        // Normalize: lowercase, strip spaces
        std::string input;
        for (char ch : line)
            if (ch != ' ' && ch != '\t') input += static_cast<char>(std::tolower(ch));

        if (input == "quit" || input == "exit") {
            std::cout << "Goodbye!\n";
            return 0;
        }
        if (input == "help") {
            std::cout << "Enter moves like 'e2e4'. For promotion: 'e7e8q' (q/r/b/n). 'quit' to exit.\n";
            continue;
        }
        if (input.size() < 4) {
            std::cout << "Invalid input. Example: e2e4\n";
            continue;
        }

        Position from = parseSquare(input.substr(0, 2));
        Position to   = parseSquare(input.substr(2, 2));
        if (from.first < 0 || to.first < 0) {
            std::cout << "Invalid square. Use a-h and 1-8.\n";
            continue;
        }

        PieceType promo = PieceType::Queen;
        if (input.size() >= 5) {
            switch (input[4]) {
                case 'r': promo = PieceType::Rook;   break;
                case 'b': promo = PieceType::Bishop; break;
                case 'n': promo = PieceType::Knight; break;
                default:  promo = PieceType::Queen;  break;
            }
        }

        if (!board.makeMove(from.first, from.second, to.first, to.second, promo))
            std::cout << "Illegal move!\n";
    }

    if (board.isGameOver()) {
        board.display();
        if (board.getIsDraw()) {
            std::cout << "Stalemate — draw!\n";
        } else {
            std::string w = (board.getWinner() == Color::White) ? "White" : "Black";
            std::cout << w << " wins by checkmate!\n";
        }
    }

    return 0;
}
