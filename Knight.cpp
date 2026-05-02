#include "Pieces.h"
#include "Knight.h"
knight::knight(char color) : pieces(color) {}
knight::~knight() {}
bool knight::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
	if (startRow == endRow && startCol == endCol)
	{
		return false;
	}
	return (abs(endRow - startRow) == 2 && abs(endCol - startCol) == 1) ||
		(abs(endRow - startRow) == 1 && abs(endCol - startCol) == 2);
}
char knight::getSymbol()
{
	return (color == 'W') ? 'N' : 'n';
}