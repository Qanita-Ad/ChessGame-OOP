#ifndef BOARD_H
#define BOARD_H
#include "Pieces.h"
#include "Pawn.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
class Board
{
private:
    //composition
    pieces* grid[8][8];
public:
    Board();
    ~Board();
    pieces* getPiece(int row, int col);
    void initialize();
    void display();                         
    bool movePiece(int startRow, int startCol, int endRow, int endCol);
    bool isInsideBoard(int row, int col);   
};
#endif

