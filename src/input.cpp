#include "input.h"

#include <SDL2/SDL.h>

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
				quit = true;
				break;

			case SDLK_w:
				up = true;
				break;

			case SDLK_UP:
				up2 = true;
				break;

			case SDLK_d:
				right = true;
				break;

			case SDLK_RIGHT:
				right2 = true;
				break;

			case SDLK_s:
				down = true;
				break;

			case SDLK_DOWN:
				down2 = true;
				break;

			case SDLK_a:
				left = true;
				break;

			case SDLK_LEFT:
				left2 = true;
				break;
			}

			break;
		}

		case SDL_KEYUP: {

			switch (event.key.keysym.sym) {
			case SDLK_w:
				up = false;
				break;

			case SDLK_UP:
				up2 = false;
				break;

			case SDLK_d:
				right = false;
				break;

			case SDLK_RIGHT:
				right2 = false;
				break;

			case SDLK_s:
				down = false;
				break;

			case SDLK_DOWN:
				down2 = false;
				break;

			case SDLK_a:
				left = false;
				break;

			case SDLK_LEFT:
				left2 = false;
				break;
			}

			break;
		}
		}
}

bool InputHandler::isQuit()
{
	return quit;
}

bool InputHandler::isUp()
{
	return up;
}

bool InputHandler::isRight()
{
	return right;
}

bool InputHandler::isDown()
{
	return down;
}

bool InputHandler::isLeft()
{
	return left;
}

bool InputHandler::isUp2()
{
	return up2;
}

bool InputHandler::isRight2()
{
	return right2;
}

bool InputHandler::isDown2()
{
	return down2;
}

bool InputHandler::isLeft2()
{
	return left2;
}
