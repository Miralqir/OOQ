#pragma once

#include "manager.h"
#include "render.h"
#include "game.h"

#include <list>
#include <vector>
#include <string>

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

	const uint64_t FRAMETIME = 100;
	uint64_t tick;
	uint64_t splash_deadline;

	bool in_menu;
	uint64_t menu_deadline;
	unsigned int menu_counter;

	bool in_quiz;
	uint64_t quiz_deadline;
	unsigned int quiz_counter;

	std::string question;
	std::vector<std::string> answers;

	TextureAccess splash;
	std::vector<TextureAccess> menu;
	std::vector<TextureAccess> quiz;

	TextureAccess continue_btn;
	TextureAccess documentation_btn;
	TextureAccess exit_btn;
	TextureAccess submit_btn;

	TextureAccess minimap;
	TextureAccess point;

public:
	UIManager(Manager *parent);

	void displayQuiz(std::string question, std::vector<std::string> answers);
	void endQuiz();

	void operator()(uint64_t delta);

private:
	int printText(std::string text, int x, int y, int max_char, int layer, COLOR color = BLACK);
	void openDocumentation();
};
