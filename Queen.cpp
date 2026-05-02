#include "Pieces.h"
#include "Queen.h"
// Constructor
queen::queen(char color) : pieces(color) {}
// Destructor
queen::~queen() {}
bool queen::isValidMove(int startRow, int startCol, int endRow, int endCol)
{
	if (startRow == endRow && startCol == endCol)
	{
		return false;
	}
	return (startRow == endRow || startCol == endCol || abs(endRow - startRow) == abs(endCol - startCol));
}
char queen::getSymbol()
{
	return (color == 'W') ? 'Q' : 'q';
}