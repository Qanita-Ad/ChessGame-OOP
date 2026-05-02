#include "Pieces.h"
#include "Pawn.h"
pawn::pawn(char color) : pieces(color)
{
	firstMove = true;
}
pawn::~pawn() {}
void pawn::setFirstMove(bool val)
{
    firstMove = val;
}
bool pawn::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
    if (startRow == endRow && startCol == endCol)
    {
        return false;
    }
    if (color == 'W')
    {
        if (startCol == endCol && endRow == startRow - 1)
        {
            return true;
        }
        if (firstMove && startCol == endCol && endRow == startRow - 2)
        {
            return true;
        }
        if (endRow == startRow - 1 && (endCol == startCol - 1 || endCol == startCol + 1))
        {
            return true;
        }
    }
    else
    {
        if (startCol == endCol && endRow == startRow + 1)
        {
            return true;
        }
        if (firstMove && startCol == endCol && endRow == startRow + 2)
        {
            return true;
        }
        if (endRow == startRow + 1 && (endCol == startCol - 1 || endCol == startCol + 1))
        {
            return true;
        }
    }
    return false;
}
char pawn::getSymbol()
{
    return (color == 'W') ? 'P' : 'p';
}