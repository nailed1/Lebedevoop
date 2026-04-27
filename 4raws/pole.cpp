#include "pole.h"
#include <iostream>
#include <iomanip>

pole::pole(bool isRedFirst){
    clear(isRedFirst);
}

void pole::clear(bool isRedFirst){
    isRedTurn = isRedFirst;
    winner = EMPTY;
    for (int i=0; i<pole_WIDTH; i++)
    for (int j=0; j<pole_HEIGHT; j++)
    cells[i][j] = EMPTY;
}

bool pole::makeTurn(int column){
    if (winner != EMPTY || column < 1 || column > pole_WIDTH)
        return false;
    int i=column-1;
    for (int j=0; j<pole_HEIGHT; j++)
        if (cells[i][j]==EMPTY)
        {
            cells[i][j] = isRedTurn ? RED : YELLOW;
            checkWinner(); // Победа?
            isRedTurn = !isRedTurn;
            return true;
        }
    return false;
}

const int DIR_NUMBER = 4;
const int di[] = { 1, 0, 1, 1 };
const int dj[] = { 0, 1, -1, 1 };
const int WIN_LENGTH = 4;
void pole::checkWinner(){
    for (int i=0; i<pole_WIDTH; i++)
        for (int j=0; j<pole_HEIGHT; j++){
            Cell start = cells[i][j];
            if (start==EMPTY) continue;
            for (int dir=0; dir<DIR_NUMBER; dir++){ 
                int length=0, iline = i, jline = j;
                while (++length < WIN_LENGTH){
                    iline += di[dir];
                    jline += dj[dir];
                    if (iline < 0 || iline >= pole_WIDTH ||
                        jline < 0 || jline >= pole_HEIGHT)
                        break;
                    if (cells[iline][jline]!=start) break;
                }
                if (length==WIN_LENGTH){
                    winner = start;
                    return;
                }
            }
        }
}

bool pole::isOver() const{
    if (winner != EMPTY)
        return true;
    for (int i=0; i<pole_WIDTH; i++)
        for (int j=0; j<pole_HEIGHT; j++)
    // Если хоть одна ячейка свободна,
    // игра не окончена
            if (cells[i][j]==EMPTY)
                return false;
    // Все ячейки заняты
    return true;
}

bool pole::isWon(bool red) const {
    return winner == (red ? RED : YELLOW);
}

bool pole::isRedTurnNow() const {
    return isRedTurn;
}

Cell pole::getCell(int i, int j) const {
    if (i < 0 || i >= pole_WIDTH || j < 0 || j >= pole_HEIGHT) {
        return EMPTY;
    }
    return cells[i][j];
}

void pole::printResult() const {
    if (winner == RED) {
        std::cout << "Победили Красные (RED)!" << std::endl;
    } else if (winner == YELLOW) {
        std::cout << "Победили Желтые (YELLOW)!" << std::endl;
    } else if (isOver()) {
        std::cout << "Ничья, поле заполнено." << std::endl;
    } else {
        std::cout << "Игра еще продолжается. Сейчас ход " 
                  << (isRedTurn ? "Красных" : "Желтых") << std::endl;
    }
}

void pole::print() const {
    for (int j = pole_HEIGHT - 1; j >= 0; j--) {
        std::cout << std::setw(2) << j + 1 << " |";
        for (int i = 0; i < pole_WIDTH; i++) {
            if (cells[i][j] == RED) std::cout << " R";
            else if (cells[i][j] == YELLOW) std::cout << " Y";
            else std::cout << " .";
        }
        std::cout << std::endl;
    }

    std::cout << "    ";
    for (int i = 0; i < pole_WIDTH; i++) std::cout << "--";
    std::cout << std::endl << "     ";
    for (int i = 0; i < pole_WIDTH; i++) std::cout << (i + 1) % 10 << " ";
    std::cout << std::endl;
}