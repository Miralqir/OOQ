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

int countDigit(int n)
{
    if (n/10 == 0)
        return 1;

    return 1 + countDigit(n / 10);
};

void getTime(uint64_t time, uint64_t* hours, uint64_t* minutes, uint64_t* seconds)
{
	uint64_t DAYS_IN_MILLISECONDS = 86400000;
	uint64_t HOURS_IN_MILLISECONDS = 3600000;
	uint64_t MINUTES_IN_MILLISECONDS = 60000;
	uint64_t SECONDS_IN_MILLISECONDS = 1000;
	uint64_t days = time / DAYS_IN_MILLISECONDS;
    *hours = (time - days*DAYS_IN_MILLISECONDS) / HOURS_IN_MILLISECONDS;
    *minutes = (time - days*DAYS_IN_MILLISECONDS - (*hours)*HOURS_IN_MILLISECONDS) / MINUTES_IN_MILLISECONDS;
    *seconds = (time - days*DAYS_IN_MILLISECONDS - (*hours)*HOURS_IN_MILLISECONDS - (*minutes)*MINUTES_IN_MILLISECONDS) / SECONDS_IN_MILLISECONDS;
};