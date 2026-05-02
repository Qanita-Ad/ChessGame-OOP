#include "Pieces.h"
pieces::pieces(char color) : color(color) {}
pieces::~pieces(){}
char pieces::getColor() 
{
	return color;
}
