#pragma once

/*
 * generic stuff that has many uses
 * but aren't part of a class
 *
 * note: consider using templates?
 */

enum DIR {UP = 0, LEFT, DOWN, RIGHT, DIR_SIZE};

int sgn(int nr);
