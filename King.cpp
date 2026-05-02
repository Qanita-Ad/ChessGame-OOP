#include "Pieces.h"
#include "King.h"
// Constructor
king::king(char color) : pieces(color) {}
// Destructor
king::~king() {}
bool king::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
	if (startRow == endRow && startCol == endCol)
	{
		return false;
	}
	return (abs(endRow - startRow) <= 1 && abs(endCol - startCol) <= 1);
}
char king::getSymbol()
{
	return (color == 'W') ? 'K' : 'k';
}
