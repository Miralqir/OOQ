#include "ui.h"

UIManager::UIManager(Manager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	texture_manager(renderer->getTextureManager()),
	input_handler(parent->getInputHandler()),
	game_manager(parent->getGameManager()),
	paused(false)
{
	logo = texture_manager->loadTexture("data/logo/WSS.png");
}

void UIManager::runTick(uint64_t delta)
{
	if (input_handler->isPause(true)) paused = not paused;

	game_manager->setPaused(paused);

	if (paused)
		renderer->addRenderItem(logo, 0, 0, false, false, 10, true);
}
