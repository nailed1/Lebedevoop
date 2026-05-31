#include "ai.h"
#include <algorithm>
#include <climits>

AI::AI(int depth) : searchDepth(depth) {}

// Статическая оценка позиции: сумма (материал + позиционный бонус) для белых минус для чёрных
int AI::evaluate(const Table& t) {
    int score = 0;
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = t.getFigureAt(r, c);
            if (!f) continue;
            // [?] Полиморфизм в действии: f — указатель на базовый класс Figure*.
            //     Вызов f->getMaterialValue() через vtable идёт в реальную реализацию:
            //     King::getMaterialValue() вернёт 20000, Pawn::getMaterialValue() вернёт 100.
            //     Без virtual это всегда вызывало бы Figure::getMaterialValue() — которого нет (= 0).
            int v = f->getMaterialValue() + f->getPositionalValue(r, c);
            score += (f->getColor() == Color::White) ? v : -v;
        }
    }
    return score;
}

// [?] Функтор (functor) — объект с перегруженным operator().
//     Используется вместо указателя на функцию, потому что нужно захватить контекст (const Table& t).
//     В C++98 нет лямбда-выражений — функтор это стандартный способ передать "функцию с состоянием".
//     std::sort/std::stable_sort принимают любой объект, у которого есть operator()(A, B) → bool.
struct CaptureSorter {
    const Table& t;
    explicit CaptureSorter(const Table& t_) : t(t_) {}
    bool operator()(const Move& a, const Move& b) const {
        Figure* va = t.getFigureAt(a.tr, a.tc);
        Figure* vb = t.getFigureAt(b.tr, b.tc);
        return (va ? va->getMaterialValue() : 0) >
               (vb ? vb->getMaterialValue() : 0);
    }
};

std::vector<Move> AI::collectMoves(const Table& t, Color::E color) {
    std::vector<Move> moves;
    moves.reserve(40);

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Figure* f = t.getFigureAt(r, c);
            if (!f || f->getColor() != color) continue;

            std::vector<Position> dests = t.getValidMoves(r, c);
            for (size_t i = 0; i < dests.size(); i++) {
                int tr = dests[i].first;
                int tc = dests[i].second;
                bool isPromo = (f->getType() == PieceType::Pawn && (tr == 0 || tr == 7));
                if (isPromo) {
                    const char promos[] = {'Q', 'R', 'B', 'N'};
                    for (int p = 0; p < 4; p++) {
                        Move m; m.fr = r; m.fc = c; m.tr = tr; m.tc = tc; m.promotion = promos[p];
                        moves.push_back(m);
                    }
                } else {
                    Move m; m.fr = r; m.fc = c; m.tr = tr; m.tc = tc; m.promotion = 'Q';
                    moves.push_back(m);
                }
            }
        }
    }

    std::stable_sort(moves.begin(), moves.end(), CaptureSorter(t));
    return moves;
}

PieceType::E AI::charToPromo(char p) {
    switch (p) {
        case 'R': return PieceType::Rook;
        case 'B': return PieceType::Bishop;
        case 'N': return PieceType::Knight;
        default:  return PieceType::Queen;
    }
}

// Минимакс с альфа-бета отсечением.
// Белые максимизируют оценку, чёрные минимизируют.
int AI::minimax(const Table& t, int depth, int alpha, int beta) const {
    if (t.isGameOver()) {
        if (t.getIsDraw()) return 0;
        return (t.getWinner() == Color::White) ? 100000 : -100000;
    }

    if (depth == 0) return evaluate(t);

    Color::E color  = t.getCurrentTurn();
    bool     maxing = (color == Color::White);
    std::vector<Move> moves = collectMoves(t, color);
    if (moves.empty()) return evaluate(t);

    int best = maxing ? INT_MIN : INT_MAX;

    for (size_t i = 0; i < moves.size(); i++) {
        const Move& m = moves[i];
        // [?] Table copy = t — вызов конструктора копирования Table(const Table&).
        //     Каждый узел дерева поиска делает полную независимую копию доски (64 clone()).
        //     Без копирования makeMove менял бы оригинальную доску и откат был бы невозможен.
        Table copy = t;
        copy.makeMove(m.fr, m.fc, m.tr, m.tc, charToPromo(m.promotion));

        int val = minimax(copy, depth - 1, alpha, beta);

        if (maxing) {
            if (val > best) best = val;
            if (best > alpha) alpha = best;
        } else {
            if (val < best) best = val;
            if (best < beta) beta = best;
        }

        // [?] Альфа-бета отсечение: если beta <= alpha — максимизирующий игрок уже
        //     нашёл лучший ход на верхнем уровне и никогда не выберет эту ветку.
        //     Позволяет не перебирать часть дерева → резкое ускорение (в идеале O(b^(d/2)) вместо O(b^d)).
        if (beta <= alpha) break;
    }

    return best;
}

Move AI::getBestMove(const Table& board, Color::E color) const {
    std::vector<Move> moves = collectMoves(board, color);
    if (moves.empty()) {
        Move none; none.fr = -1; none.fc = -1; none.tr = -1; none.tc = -1; none.promotion = 'Q';
        return none;
    }

    bool maxing = (color == Color::White);
    int  best   = maxing ? INT_MIN : INT_MAX;
    Move bestMove = moves[0];
    int  alpha  = INT_MIN;
    int  beta   = INT_MAX;

    for (size_t i = 0; i < moves.size(); i++) {
        const Move& m = moves[i];
        Table copy = board;
        copy.makeMove(m.fr, m.fc, m.tr, m.tc, charToPromo(m.promotion));

        int val = minimax(copy, searchDepth - 1, alpha, beta);

        bool better = maxing ? (val > best) : (val < best);
        if (better) {
            best     = val;
            bestMove = m;
        }

        if (maxing) alpha = std::max(alpha, best);
        else        beta  = std::min(beta,  best);
    }

    return bestMove;
}
