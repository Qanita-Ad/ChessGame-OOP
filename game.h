#ifndef GAME_H
#define GAME_H
#include "Board.h"
#include <utility>
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
    bool playMove(int sr, int sc, int er, int ec);
    void displayBoard();
    bool isGameOver();
    char getCurrentTurn() const;
    pieces* getPieceAt(int row, int col);
    int getValidMoves(int row, int col, std::pair<int, int> out[64]);
};
#endif