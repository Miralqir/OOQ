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

	std::list<Texture>::iterator background;
	std::list<Texture>::iterator background_quiz;
	std::list<Texture>::iterator exit_button;
	std::list<Texture>::iterator continue_button;
	std::list<Texture>::iterator selected_exit_button;
	std::list<Texture>::iterator selected_continue_button;
	std::list<Texture>::iterator time_edit1;
	std::list<Texture>::iterator time_edit2;
	std::list<Texture>::iterator text_box_short1;
	std::list<Texture>::iterator text_box_short2;
	std::list<Texture>::iterator text_box_long;
	std::list<Texture>::iterator text_box;
	std::list<Texture>::iterator sign;

	std::vector<std::list<Texture>::iterator> letters;
	std::vector<std::list<Texture>::iterator> digits_black;
	std::vector<std::list<Texture>::iterator> digits_white;
	std::vector<std::list<Texture>::iterator> digits_green;


public:
	UIManager(Manager *parent);

	void runTick(uint64_t delta);
	void renderText(std::string text, int pos_x, int pos_y, int max_x);
	void renderNumber(std::vector<std::list<Texture>::iterator> type_digits, int number, int pos_x, int pos_y);
	void renderTimePart(uint64_t time, int pos_x, int pos_y, int* final_pos);
	void renderTime(uint64_t time, int pos_x, int pos_y);
	void displayQuiz(std::string question, std::vector<std::string> answers);
	void endQuiz();
};
