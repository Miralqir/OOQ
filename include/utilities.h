#pragma once

/*
 * generic stuff that has many uses
 * but aren't part of a class
 *
 * note: consider using templates?
 */

#include <stdint.h>

enum DIR {UP = 0, LEFT, DOWN, RIGHT, DIR_SIZE};

int sgn(int nr);

int countDigit(int n);

void getTime(uint64_t time, uint64_t* hours, uint64_t* minutes, uint64_t* seconds);