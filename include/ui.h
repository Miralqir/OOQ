#pragma once

#include "manager.h"
#include "render.h"
#include "game.h"

#include <list>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <iterator>
#include <cmath>

class Manager;
class GameManager;

class UIManager
{
private:
	Manager *parent;
	Renderer *renderer;
	TextureManager *texture_manager;
	InputHandler *input_handler;
	GameManager *game_manager;

	bool paused;
	bool in_quiz;

	std::string question;
	std::vector<std::string> answers;

	TextureAccess background;
	TextureAccess background_quiz;
	TextureAccess exit_button;
	TextureAccess continue_button;
	TextureAccess selected_exit_button;
	TextureAccess selected_continue_button;
	TextureAccess time_edit1;
	TextureAccess time_edit2;
	TextureAccess text_box_short1;
	TextureAccess text_box_short2;
	TextureAccess text_box_long;
	TextureAccess text_box;
	TextureAccess sign;

	std::vector<TextureAccess> letters;
	std::vector<TextureAccess> digits_black;
	std::vector<TextureAccess> digits_white;
	std::vector<TextureAccess> digits_green;


public:
	UIManager(Manager *parent);

	void runTick(uint64_t delta);
	void renderText(std::string text, int pos_x, int pos_y, int max_x);
	void renderNumber(std::vector<TextureAccess> type_digits, int number, int pos_x, int pos_y);
	void renderTimePart(uint64_t time, int pos_x, int pos_y, int* final_pos);
	void renderTime(uint64_t time, int pos_x, int pos_y);
	void displayQuiz(std::string question, std::vector<std::string> answers);
	void endQuiz();
};
