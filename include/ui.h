#pragma once

#include "manager.h"
#include "render.h"
#include "game.h"

#include <list>

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

	std::list<Texture>::iterator logo;

public:
	UIManager(Manager *parent);

	void runTick(uint64_t delta);
};
