#ifndef KING_H
#define KING_H
#include "pieces.h"
class king :public pieces
{
public:
	king(char color);
	~king();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif

