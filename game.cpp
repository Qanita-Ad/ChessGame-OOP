#include "Game.h"
#include <iostream>
using namespace std;
// Constructor
Game::Game()
{
    currentTurn = 'W';
}
// Destructor
Game::~Game() {}
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
bool Game::playMove(int sr, int sc, int er, int ec)
{
    pieces* p = board.getPiece(sr, sc);
    if (p == NULL)
    {
        cout << "No piece at selected position!" << endl;
        return false;
    }
    if (p->getColor() != currentTurn)
    {
        cout << "Not your turn!" << endl;
        return false;
    }
    bool moved = board.movePiece(sr, sc, er, ec);
    if (moved)
    {
        cout << "Move successful!" << endl;
        switchTurn();
    }
    else
    {
        cout << "Invalid move!" << endl;
    }
    return moved;
}
char Game::getCurrentTurn() const
{
    return currentTurn;
}
pieces* Game::getPieceAt(int row, int col)
{
    return board.getPiece(row, col);
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
    bool whiteKing = false, blackKing = false;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
        {
            pieces* p = board.getPiece(r, c);
            if (p)
            {
                char s = p->getSymbol();
                if (s == 'K') whiteKing = true;
                if (s == 'k') blackKing = true;
            }
        }
    return !whiteKing || !blackKing;
}
int Game::getValidMoves(int row, int col, std::pair<int, int> out[64])
{
    int count = 0;
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++)
            if (board.canMove(row, col, r, c))
                out[count++] = std::make_pair(r, c);
    return count;
}