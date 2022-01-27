#pragma once

#include "utilities.h"

#include <vector>

class InputHandler
{
private:
	bool quit;
	std::vector<bool> player;
	std::vector<bool> player2;
	bool pause;
	bool enter;
	std::vector<bool> answer;

public:
	InputHandler();

	void processEvents();
	bool isQuit(bool clear = false);
	bool isPlayer(DIR dir, bool clear = false);
	bool isPlayer2(DIR dir, bool clear = false);
	bool isPause(bool clear = false);
	bool isEnter(bool clear = false);
	bool isAnswer(int ans, bool clear = false);
};
