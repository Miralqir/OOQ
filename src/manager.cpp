#include "manager.h"

#include <cstdint>
#include <stdexcept>
#include <new>
#include <vector>
#include <list>
#include <string>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

Manager::Manager(int argc, char **argv) :
	argc(argc),
	argv(argv),
	last_tick(0),
	current_tick(0),
	is_quit(false)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
		throw std::runtime_error(IMG_GetError());

	renderer = new Renderer();
	input_handler = new InputHandler();
	game_manager = new GameManager(this);
	ui_manager = new UIManager(this);
}

Manager::~Manager()
{
	delete ui_manager;
	delete game_manager;
	delete input_handler;
	delete renderer;
	IMG_Quit();
	SDL_Quit();
}

Renderer *Manager::getRenderer()
{
	return renderer;
}

InputHandler *Manager::getInputHandler()
{
	return input_handler;
}

GameManager *Manager::getGameManager()
{
	return game_manager;
}

UIManager *Manager::getUIManager()
{
	return ui_manager;
}

void Manager::quit()
{
	is_quit = true;
}

int Manager::run()
{
	while (not is_quit) {
		last_tick = current_tick;
		/* switch to SDL_GetTicks64 after update to SDL 2.0.18
		 * current_tick = SDL_GetTicks64();
		 */
		current_tick = SDL_GetTicks();
		uint64_t delta = current_tick - last_tick;

		input_handler->processEvents();
		if (input_handler->isQuit())
			is_quit = true;

		game_manager->runTick(delta);

		ui_manager->runTick(delta);

		renderer->render();

		// debugging purposes
		//SDL_Delay(500);
	}

	return 0;
}
