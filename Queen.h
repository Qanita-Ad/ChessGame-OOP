#ifndef QUEEN_H
#define QUEEN_H
#include "pieces.h"
class queen :public pieces
{
public:
	queen(char color);
	~queen();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif


