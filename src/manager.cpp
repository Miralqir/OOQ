#include "manager.h"

#include <cstdint>
#include <stdexcept>
#include <new>
#include <vector>
#include <list>
#include <string>

#ifdef __unix__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#elif _WIN32
#include <ciso646>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#else
#error Unsupported platform
#endif

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

	if (TTF_Init() != 0)
		throw std::runtime_error(TTF_GetError());

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
	TTF_Quit();
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

int Manager::operator()()
{
	uint64_t delta = 0;

	while (not is_quit) {
		/* 
		 * switch to SDL_GetTicks64 after update to SDL 2.0.18
		 * current_tick = SDL_GetTicks64();
		 */
		// get time passed since last frame
		last_tick = current_tick;
		current_tick = SDL_GetTicks();
		delta = current_tick - last_tick;

		// handle events
		input_handler->processEvents();
		if (input_handler->isQuit())
			is_quit = true;

		game_manager->runTick(delta);

		//ui_manager->runTick(delta);
		(*ui_manager)(delta);

		(*renderer)();

		// limit fps
		static const uint64_t max_fps = 60;
		static const uint64_t min_ticks = 1000 / max_fps;

		// calculate time it took to run this frame
		delta = SDL_GetTicks() - current_tick;

		if (delta < min_ticks)
			SDL_Delay(min_ticks - delta);
	}

	return 0;
}
