#include "pole.h"
#include <locale.h>
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

void testHorizontalWin() {
    pole pole(true);
    int moves[] = {1, 1, 2, 2, 3, 3, 4};
    
    for (int col : moves)
        pole.makeTurn(col);

    if (pole.isWon(true)) 
        cout << "Test Horizontal Win: pass" << endl;
    else 
        cout << "Test Horizontal Win: fail" << endl;
}

void testDiagonallWin() {
    pole pole(true);
    int moves[] = {1, 2, 2, 3, 3, 4, 3, 4, 4, 7, 4};
    
    for (int col : moves)
        pole.makeTurn(col);

    if (pole.isWon(true)) 
        cout << "Test Diagonal Win: pass" << endl;
    else 
        cout << "Test Diagonal Win: fail" << endl;
}

void runRandomTest(int iterations) {
    srand(time(NULL));
    for (int t = 0; t < iterations; t++) {
        pole pole(rand() % 2);
        
        while (!pole.isOver()) {
            int randomCol = (rand() % pole_WIDTH) + 1;
            pole.makeTurn(randomCol);
        }
    }
    cout << "Successfully completed: " << iterations << " iterations" << endl;
}

int main(void){
    setlocale(LC_ALL, "Russian");

    testHorizontalWin();
    testDiagonallWin();
    runRandomTest(1000);

    pole pole(true);
    while (!pole.isOver()){
        pole.print();
        cout<<"Ходит "<<
        (pole.isRedTurnNow() ? "красный" : "желтый")<<
        " игрок, введите ход (1-7)"<<endl;
        int column; cin>>column;   
        pole.makeTurn(column);
    }
    pole.printResult();
    
    return 0;
}