#include "utilities.h"

int sgn(int nr)
{
	if (nr == 0)
		return 0;
	if (nr > 0)
		return 1;
	if (nr < 0)
		return -1;

	return -2;
};
