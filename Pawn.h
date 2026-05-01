#ifndef PAWN_H
#define PAWN_H
#include "pieces.h"
class pawn :public pieces
{
public:
	pawn(char color);
	~pawn();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif
