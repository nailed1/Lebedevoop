#ifndef POLE_H
#define POLE_H

enum Cell{
    EMPTY,
    RED,
    YELLOW
};

const int pole_WIDTH = 7;
const int pole_HEIGHT = 6;

class pole{
    Cell cells[pole_WIDTH][pole_HEIGHT];
    // Очередь хода
    bool isRedTurn;
    // Кто на данный момент выиграл
    Cell winner;
    void checkWinner();
public:
    pole(bool isRedFirst);
    void clear(bool isRedFirst);
    bool makeTurn(int column);
    bool isWon(bool red) const;
    bool isOver() const;
    Cell getCell(int i, int j) const;
    bool isRedTurnNow() const;
    void print() const;
    void printResult() const;
};

#endif