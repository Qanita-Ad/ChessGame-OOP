#include "Game.h"
#include <iostream>
using namespace std;
// Constructor
Game::Game()
{
    currentTurn = 'W';  
}
// Destructor
Game::~Game(){}
void Game::displayBoard()
{
    board.display();
}
void Game::switchTurn()
{
    if (currentTurn == 'W')
    {
        currentTurn = 'B';
    }
    else
    {
        currentTurn = 'W';
    }
}
void Game::playMove(int sr, int sc, int er, int ec)
{
    pieces* p = board.getPiece(sr,sc);
    if (p == NULL)
    {
        cout << "No piece at selected position!"<<endl;
        return;
    }
    if (p->getColor() != currentTurn)
    {
        cout << "Not your turn!"<<endl;
        return;
    }
    bool moved = board.movePiece(sr, sc, er, ec);
    if (moved)
    {
        cout << "Move successful!"<<endl;
        switchTurn();
    }
    else
    {
        cout << "Invalid move!"<<endl;
    }
}
void Game::start()
{
    int sr, sc, er, ec;
    while (true)
    {
        displayBoard();
        cout << endl;
        cout << "Turn: " << currentTurn << endl;
        cout << "Enter start row and col: ";
        cin >> sr >> sc;
        cout << "Enter end row and col: ";
        cin >> er >> ec;
        playMove(sr, sc, er, ec);
    }
}
bool Game::isGameOver()
{
    return false;
}