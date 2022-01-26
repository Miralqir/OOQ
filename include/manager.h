#pragma once

#include "render.h"
#include "input.h"
#include "game.h"
#include "ui.h"

class GameManager;
class UIManager;

class Manager
{
private:
	int argc;
	char **argv;
	Renderer *renderer;
	InputHandler *input_handler;
	GameManager *game_manager;
	UIManager *ui_manager;

	uint64_t last_tick = 0;
	uint64_t current_tick = 0;
	bool is_quit;

public:
	Manager(int argc, char **argv);
	~Manager();

	Renderer *getRenderer();
	InputHandler *getInputHandler();
	GameManager *getGameManager();
	UIManager *getUIManager();

	void quit();

	int run();
};
