#ifndef PIECES_H
#define PIECES_H
#include<iostream>
#include <cmath>
using namespace std;
class pieces
{
protected:
	char color;
public:
	pieces(char color);
	virtual ~pieces();
	virtual bool isValidMove(int startRow, int startCol, int endRow, int endCol)=0;
	char getColor();
	virtual char getSymbol() = 0;
};
#endif
