#include "game.h"

#include "config.h"

#include <algorithm>
#include <limits>
#include <cmath>
#include <utility>
#include <fstream>

MapManager::MapManager(GameManager *parent) :
	parent(parent),
	renderer(parent->getRenderer())
	//texture_manager(renderer->getTextureManager())
{
	std::ifstream maps("data/maps.txt");
	std::filesystem::path def;

	maps >> def;

	loadMap(def);
}

void MapManager::loadMap(std::filesystem::path map)
{
	std::ifstream data(map);

	int size_x, size_y;
	std::filesystem::path tex;
	int coll;

	data >> size_x >> size_y
	     >> spawn_x >> spawn_y;

	// set storage vectors to right size
	tile.resize(size_y);
	collision.resize(size_y);

	for (int i = 0; i < size_y; ++i) {
		tile[i].resize(size_x);
		collision[i].resize(size_x);
	}

	// set player position
	// bad idea, player not loaded
	//parent->getPlayer()->setPos(spawn_x, spawn_y);

	// load textures
	TextureManager *texture_manager =
	        parent->getManager()->getRenderer()->getTextureManager();

	for (int i = 0; i < size_y; ++i)
		for (int j = 0; j < size_x; ++j) {
			data >> tex >> coll;

			tile[i][j] = texture_manager->loadTexture(tex);
			collision[i][j] = coll;
		}
}

void MapManager::getSpawn(int *x, int *y)
{
	*x = spawn_x;
	*y = spawn_y;
}

int MapManager::getCollision(int pos_x, int pos_y)
{
	if (pos_y < 0 or pos_x < 0 or pos_y >= collision.size() or pos_x >=
	    collision[pos_y].size())
		return 1; // by default collide OOB
	return collision[pos_y][pos_x];
}

void MapManager::render()
{
	for (int i = 0; i < tile.size(); ++i)
		for (int j = 0; j < tile[i].size(); ++j)
			/*
			 * matrix has tile[line][column]
			 * Renderer uses column and then line
			 * hence this weird flip
			 */
			renderer->addRenderItem(tile[i][j], j * TILE_SIZE, i * TILE_SIZE, false, false, 0);
}

Player::Player(GameManager *parent, int type) :
	parent(parent),
	renderer(parent->getRenderer()),
	//texture_manager(renderer->getTextureManager()),
	input_handler(parent->getManager()->getInputHandler()),
	map_manager(parent->getMapManager()),
	type(type),
	input_time(0),
	input_deadline(0),
	last_dir(0),
	pos_x(0),
	pos_y(0)
{
	// load spawn location
	map_manager->getSpawn(&pos_x, &pos_y);
	centerCamera();

	// TODO: load male/female based on choice

	TextureManager *texture_manager = renderer->getTextureManager();

	/*
	 * keep all frames in order
	 * even if duplicated
	 * pointers don't take much memory
	 * and it simplifies code massively
	 */
	switch (type) {
	case 0:
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u1.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u2.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_male/u.png"));

		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d1.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d2.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_male/d.png"));

		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s1.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s2.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_male/s3.png"));

		break;

	case 1:
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u1.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u2.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u.png"));
		up.push_back(texture_manager->loadTexture("data/sprite/mc_female/u.png"));

		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d1.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d2.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d.png"));
		down.push_back(texture_manager->loadTexture("data/sprite/mc_female/d.png"));

		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s1.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s2.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s.png"));
		side.push_back(texture_manager->loadTexture("data/sprite/mc_female/s3.png"));

		break;
	}
}

bool Player::isCameraCenter()
{
	return true;
}

void Player::setPos(int pos_x, int pos_y)
{
	this->pos_x = pos_x;
	this->pos_y = pos_y;

	//centerCamera();
}

void Player::getPos(int *pos_x, int *pos_y)
{
	*pos_x = this->pos_x;
	*pos_y = this->pos_y;
}

void Player::getSize(int *size_x, int *size_y)
{
	*size_x = 2;
	*size_y = 2;
}

void Player::getCenter(int *pos_x, int *pos_y)
{
	*pos_x = this->pos_x + 1;
	*pos_y = this->pos_y + 1;
}

void Player::centerCamera()
{
	// deprecated, moved to GameManager
	// TODO: remove
	renderer->setCenter((pos_x + 1) * TILE_SIZE, (pos_y + 1) * TILE_SIZE);
}

int Player::checkCollision(int offset_x, int offset_y)
{
	/*
	 * player is 2x2 normal tiles
	 * account for it during collision checks
	 */

	int tmp_x = pos_x + offset_x;
	int tmp_y = pos_y + offset_y;

	return std::max(std::max(map_manager->getCollision(tmp_x, tmp_y), map_manager->getCollision(tmp_x + 1, tmp_y)), std::max(map_manager->getCollision(tmp_x, tmp_y + 1), map_manager->getCollision(tmp_x + 1, tmp_y + 1)));
}

void Player::runTick(uint64_t delta)
{
	// get input
	static const uint64_t INPUT_DELAY = 250;
	input_time += delta;
	if (input_time >= input_deadline) {
		bool has_input = false;
		if ((input_handler->isUp() and type == 0) or 
		    (input_handler->isUp2() and type == 1)) {
			if (checkCollision(0, -1) < 1) {
				--pos_y;
				has_input = true;
			}

			last_dir = 0;

			goto skip;
		}

		if ((input_handler->isRight() and type == 0) or
		    (input_handler->isRight2() and type == 1)) {
			if (checkCollision(1, 0) < 1) {
				++pos_x;
				has_input = true;
			}

			last_dir = 1;

			goto skip;
		}

		if ((input_handler->isDown() and type == 0) or
		    (input_handler->isDown2() and type == 1)) {
			if (checkCollision(0, 1) < 1) {
				++pos_y;
				has_input = true;
			}

			last_dir = 2;

			goto skip;
		}

		if ((input_handler->isLeft() and type == 0) or
		    (input_handler->isLeft2() and type == 1)) {
			if (checkCollision(-1, 0) < 1) {
				--pos_x;
				has_input = true;
			}

			last_dir = 3;

			goto skip;
		}

skip:
		if (has_input)
			input_deadline = input_time + INPUT_DELAY;
	}

	// determine which animation frame to use
	int frame = 0;
	int pos_offset = 0;

	if (input_time > input_deadline) {
		pos_offset = 0;
		frame = 0;

		if (input_time - input_deadline < INPUT_DELAY / 4)
			frame = 5;
	} else { 
		pos_offset = (input_deadline - input_time) * TILE_SIZE / INPUT_DELAY;

		if (input_deadline - input_time < INPUT_DELAY / 4)
			frame = 1;
		else if (input_deadline - input_time < INPUT_DELAY * 2 / 4)
			frame = 2;
		else if (input_deadline - input_time < INPUT_DELAY * 3 / 4)
			frame = 3;
		else
			frame = 4;
	}

	// render player
	switch (last_dir) {
	case 0:
		renderer->addRenderItem(up[frame], pos_x * TILE_SIZE, pos_y * TILE_SIZE + pos_offset, false, false, 1);
		break;
	case 1:
		renderer->addRenderItem(side[frame], pos_x * TILE_SIZE - pos_offset, pos_y * TILE_SIZE, false, false, 1);
		break;
	case 2:
		renderer->addRenderItem(down[frame], pos_x * TILE_SIZE, pos_y * TILE_SIZE - pos_offset, false, false, 1);
		break;
	case 3:
		renderer->addRenderItem(side[frame], pos_x * TILE_SIZE + pos_offset, pos_y * TILE_SIZE, true, false, 1);
		break;
	}
}

GameManager::GameManager(Manager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	map_manager(this)
{
	// set renderer size
	// aspect ratio is 4:3 for classy feel
	static const int multiplier = 5;
	renderer->setSize(4 * multiplier * TILE_SIZE, 3 * multiplier * TILE_SIZE);

	// player should always be first object
	objects.push_back(new Player(this, 0));
	objects.push_back(new Player(this, 1));

	// TODO: load objects from file
}

GameManager::~GameManager()
{
	for (auto obj : objects)
		delete obj;
}

Manager *GameManager::getManager()
{
	return parent;
}

Renderer *GameManager::getRenderer()
{
	return renderer;
}

MapManager *GameManager::getMapManager()
{
	return &map_manager;
}

Player *GameManager::getPlayer()
{
	return static_cast<Player *>(objects.front());
}

void GameManager::runTick(uint64_t delta)
{
	map_manager.render();

	// calculate camera center
	int camera_count = 0;
	int camera_x = 0;
	int camera_y = 0;
	// calculate map size to include everything
	int min_x = std::numeric_limits<int>::max();
	int min_y = std::numeric_limits<int>::max();
	int max_x = std::numeric_limits<int>::min();
	int max_y = std::numeric_limits<int>::min();

	for (auto obj : objects) {
		obj->runTick(delta);

		if (obj->isCameraCenter()) {
			int tmp_x, tmp_y;

			obj->getCenter(&tmp_x, &tmp_y);

			camera_count++;
			camera_x += tmp_x;
			camera_y += tmp_y;

			min_x = std::min(min_x, tmp_x);
			min_y = std::min(min_y, tmp_y);
			max_x = std::max(max_x, tmp_x);
			max_y = std::max(max_y, tmp_y);
		}
	}

	camera_x /= camera_count;
	camera_y /= camera_count;

	int size_x = std::abs(max_x - min_x);
	int size_y = std::abs(max_y - min_y);
	int multiplier = 5;

	while (4 * multiplier < size_x and 3 * multiplier < size_y) 
		multiplier++;

	/* TODO: change this to work with absolute screen values
	 * so that smooth movement can be achieved
	 */
	renderer->setCenter(camera_x * TILE_SIZE, camera_y * TILE_SIZE);
	renderer->setSize(4 * multiplier * TILE_SIZE, 3 * multiplier * TILE_SIZE);
}
