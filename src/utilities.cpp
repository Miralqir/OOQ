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
	if (n < 0)
		n *= -1;

	int count = 0;

	while (n > 0) {
		++count;
		n /= 10;
	}

	return count;
};

void getTime(uint64_t time, uint64_t *hours, uint64_t *minutes, uint64_t *seconds)
{
	static const uint64_t DAYS_IN_MILLISECONDS = 86400000;
	static const uint64_t HOURS_IN_MILLISECONDS = 3600000;
	static const uint64_t MINUTES_IN_MILLISECONDS = 60000;
	static const uint64_t SECONDS_IN_MILLISECONDS = 1000;

	uint64_t d = time / DAYS_IN_MILLISECONDS;
	uint64_t h = (time - d * DAYS_IN_MILLISECONDS) / HOURS_IN_MILLISECONDS;
	uint64_t m = (time - d * DAYS_IN_MILLISECONDS - h * HOURS_IN_MILLISECONDS) / MINUTES_IN_MILLISECONDS;
	uint64_t s = (time - d * DAYS_IN_MILLISECONDS - h * HOURS_IN_MILLISECONDS - m * MINUTES_IN_MILLISECONDS) / SECONDS_IN_MILLISECONDS;

	if (hours)
		*hours = h;

	if (minutes)
		*minutes = m;

	if (seconds)
		*seconds = s;
};
