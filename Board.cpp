#include "Board.h"
#include <iostream>
using namespace std;
// Constructor
Board::Board()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            grid[i][j] = NULL;
        }
    }
    initialize();
}
// Destructor
Board::~Board()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            delete grid[i][j];
        }
    }
}
pieces* Board::getPiece(int row, int col)
{
    return grid[row][col];
}
void Board::initialize()
{
    grid[0][0] = new rook('B');
    grid[0][1] = new knight('B');
    grid[0][2] = new bishop('B');
    grid[0][3] = new queen('B');
    grid[0][4] = new king('B');
    grid[0][5] = new bishop('B');
    grid[0][6] = new knight('B');
    grid[0][7] = new rook('B');
    for (int j = 0; j < 8; j++)
    {
        grid[1][j] = new pawn('B');
    }
    grid[7][0] = new rook('W');
    grid[7][1] = new knight('W');
    grid[7][2] = new bishop('W');
    grid[7][3] = new queen('W');
    grid[7][4] = new king('W');
    grid[7][5] = new bishop('W');
    grid[7][6] = new knight('W');
    grid[7][7] = new rook('W');
    for (int j = 0; j < 8; j++)
    {
        grid[6][j] = new pawn('W');
    }
}
void Board::display()
{
    cout <<endl<<" 0 1 2 3 4 5 6 7"<<endl;
    for (int i = 0; i < 8; i++)
    {
        cout << i << "  ";
        for (int j = 0; j < 8; j++)
        {
            if (grid[i][j] == NULL)
            {
                cout << ". ";
            }
            else
            {
                cout << grid[i][j]->getSymbol() << " ";
            }
        }
        cout << endl;
    }
}
bool Board::isInsideBoard(int row, int col)
{
    return (row >= 0 && row < 8 && col >= 0 && col < 8);
}
bool Board::movePiece(int startRow, int startCol, int endRow, int endCol)
{
    if (!isInsideBoard(startRow, startCol) || !isInsideBoard(endRow, endCol))
    {
        return false;
    }
    pieces* p = grid[startRow][startCol];
    if (p == NULL)
    {
        return false;
    }
    if (!p->isValidMove(startRow, startCol, endRow, endCol))
    {
        return false;
    }
    pieces* dest = grid[endRow][endCol];
    if (dest != NULL && dest->getColor() == p->getColor())
    {
        return false;
    }
    pawn* pPawn = dynamic_cast<pawn*>(p);
    if (pPawn != NULL)
    {
        if (startCol == endCol)
        {
            if (dest != NULL)
            {
                return false;
            }
        }
        else
        {
            if (dest == NULL)
            {
                return false;
            }
        }
    }
    delete dest;
    grid[endRow][endCol] = p;
    grid[startRow][startCol] = NULL;
    if (pPawn != NULL)
    {
        pPawn->setFirstMove(false);
    }
    return true;
}