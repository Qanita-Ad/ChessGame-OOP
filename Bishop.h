#ifndef BISHOP_H
#define BISHOP_H
#include "pieces.h"
class bishop :public pieces
{
public:
	bishop(char color);
	~bishop();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif
