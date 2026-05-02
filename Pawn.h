#ifndef PAWN_H
#define PAWN_H
#include "pieces.h"
class pawn :public pieces
{
private:
	bool firstMove;
public:
	pawn(char color);
	~pawn();
	void setFirstMove(bool val);
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif
