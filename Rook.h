#ifndef ROOK_H
#define ROOK_H
#include "pieces.h"
class rook :public pieces
{
public:
	rook(char color);
	~rook();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif
