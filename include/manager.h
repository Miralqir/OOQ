#pragma once

#include "renderer.h"
#include "input.h"
#include "game.h"

class GameManager;

class Manager
{
private:
	int argc;
	char **argv;
	Renderer *renderer;
	InputHandler *input_handler;
	GameManager *game_manager;

public:
	Manager(int argc, char **argv);
	~Manager();

	Renderer *getRenderer();
	InputHandler *getInputHandler();
	GameManager *getGameManager();

	int run();
};
