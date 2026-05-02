#include "Pieces.h"
#include "Bishop.h"
bishop::bishop(char color) : pieces(color) {}
bishop::~bishop() {}
bool bishop::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
	if (startRow == endRow && startCol == endCol)
	{
		return false;
	}
	return abs(endRow - startRow) == abs(endCol - startCol);
}
char bishop::getSymbol()
{
	return (color == 'W') ? 'B' : 'b';
}