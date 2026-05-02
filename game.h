#ifndef GAME_H
#define GAME_H
#include "Board.h"
class Game
{
private:
    //composition
    Board board;
    char currentTurn;  
public:
    Game();
	~Game();
    void start();
    void switchTurn();
    void playMove(int sr, int sc, int er, int ec);
    void displayBoard();
    bool isGameOver();
};
#endif