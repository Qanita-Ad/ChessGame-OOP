#ifndef KNIGHT_H
#define KNIGHT_H
#include "pieces.h"
class knight :public pieces
{
public:
	knight(char color);
	~knight();
	bool isValidMove(int startRow, int startCol, int endRow, int endCol) override;
	char getSymbol() override;
};
#endif

