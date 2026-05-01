#include "Pieces.h"
#include "Pawn.h"
pawn::pawn(char color) : pieces(color) {}
pawn::~pawn() {}
bool pawn::isValidMove(int startRow, int startCol, int endRow, int endCol) 
{
	if (color == 'W')
	{
		if (startRow == 6 && endRow == 4 && startCol == endCol) 
		{
			return true; 
		}
		if (endRow == startRow - 1 && startCol == endCol)
		{
			return true; 
		}
	} 
	else
	{
		if (startRow == 1 && endRow == 3 && startCol == endCol)
		{
			return true; 
		}
		if (endRow == startRow + 1 && startCol == endCol)
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