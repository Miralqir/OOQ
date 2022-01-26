#include "input.h"

#include <SDL2/SDL.h>

InputHandler::InputHandler() :
	quit(false),
	player(DIR_SIZE),
	player2(DIR_SIZE)
{}

void InputHandler::processEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
		switch (event.type) {
		case SDL_QUIT:
			quit = true;
			break;

		/*
		 * for list of available keycodes:
		 * https://wiki.libsdl.org/SDL_Keycode
		 */
		case SDL_KEYDOWN: {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
			case SDLK_p:
				pause = true;
				break;

			case SDLK_w:
				player[UP] = true;
				break;

			case SDLK_UP:
				player2[UP] = true;
				break;

			case SDLK_d:
				player[RIGHT] = true;
				break;

			case SDLK_RIGHT:
				player2[RIGHT] = true;
				break;

			case SDLK_s:
				player[DOWN] = true;
				break;

			case SDLK_DOWN:
				player2[DOWN] = true;
				break;

			case SDLK_a:
				player[LEFT] = true;
				break;

			case SDLK_LEFT:
				player2[LEFT] = true;
				break;
			}

			break;
		}

		case SDL_KEYUP: {
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
			case SDLK_p:
				pause = false;
				break;

			case SDLK_w:
				player[UP] = false;
				break;

			case SDLK_UP:
				player2[UP] = false;
				break;

			case SDLK_d:
				player[RIGHT] = false;
				break;

			case SDLK_RIGHT:
				player2[RIGHT] = false;
				break;

			case SDLK_s:
				player[DOWN] = false;
				break;

			case SDLK_DOWN:
				player2[DOWN] = false;
				break;

			case SDLK_a:
				player[LEFT] = false;
				break;

			case SDLK_LEFT:
				player2[LEFT] = false;
				break;
			}

			break;
		}
		}
}

bool InputHandler::isQuit(bool clear)
{
	bool ret = quit;
	if (clear) quit = false;
	return ret;
}

bool InputHandler::isPlayer(DIR dir, bool clear)
{
	bool ret = player[dir];
	if (clear) player[dir] = false;
	return ret;
}

bool InputHandler::isPlayer2(DIR dir, bool clear)
{
	bool ret = player2[dir];
	if (clear) player2[dir] = false;
	return ret;
}

bool InputHandler::isPause(bool clear)
{
	bool ret = pause;
	if (clear) pause = false;
	return ret;
}
