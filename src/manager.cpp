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
	argv(argv)
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(SDL_GetError());

	if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
		throw std::runtime_error(IMG_GetError());

	renderer = new Renderer();
	input_handler = new InputHandler();
	game_manager = new GameManager(this);
}

Manager::~Manager()
{
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

int Manager::run()
{
	/*
	std::vector<std::list<Texture>::iterator> textures;
	for (int i = 1; i < 10; ++i) {
	        std::ostringstream path;
	        path << "data/animation/mc_male/battle_stance/" << i
	             << ".png";
	        textures.push_back(renderer->getTextureManager()->loadTexture(path.str()));
	}

	auto draw_it = textures.begin();
	unsigned long last_draw_time = 0;
	const unsigned long FRAME_TIME = 100;
	*/

	uint64_t last_tick = 0;
	uint64_t current_tick = 0;

	while (!input_handler->isQuit()) {
		last_tick = current_tick;
		/* switch to SDL_GetTicks64 after update to SDL 2.0.18
		 * current_tick = SDL_GetTicks64();
		 */
		current_tick = SDL_GetTicks();

		input_handler->processEvents();

		/*
		if (draw_it == textures.end()) {
		        draw_it = textures.begin();
		}

		if (draw_it != textures.end()) {
		        renderer->addRenderItem(*draw_it, 10, 10);

		        unsigned long now = SDL_GetTicks();
		        if (last_draw_time + FRAME_TIME < now) {
		                draw_it++;
		                last_draw_time = now;
		        }

		}
		*/

		game_manager->runTick(current_tick - last_tick);

		renderer->render();
	}

	return 0;
}
