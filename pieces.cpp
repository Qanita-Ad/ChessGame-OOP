#include "Pieces.h"
// Base Constructor
pieces::pieces(char color) : color(color) {}
// Base Destructor
pieces::~pieces(){}
char pieces::getColor() 
{
	return color;
}
