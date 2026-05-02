#include "Pieces.h"
#include "Rook.h"
rook::rook(char color) : pieces(color) {}
rook::~rook() {}
bool rook::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
	if (startRow == endRow && startCol == endCol)
	{
		return false;
	}
	return (startRow == endRow || startCol == endCol);
}
char rook::getSymbol()
{
	return (color == 'W') ? 'R' : 'r';
}
