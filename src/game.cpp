#include "game.h"

#include "config.h"

#include <algorithm>
#include <limits>
#include <cmath>
#include <utility>
#include <random>
#include <fstream>
#include <iostream>

MapManager::MapManager(GameManager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	texture_manager(renderer->getTextureManager())
{
	// load available maps
	std::ifstream maps_file("data/maps.txt");

	int id;
	std::filesystem::path path;

	while (maps_file >> id >> path) {
		if (id >= maps.size())
			maps.resize(id + 1);

		maps[id] = path;
	}
}

void MapManager::loadMap(int map, bool respawn)
{
	// don't reload maps?
	//if (map == current_map)
	//	return;

	std::ifstream data(maps[map]);

	// update current map
	current_map = map;

	// clear old data
	resizeMapStorage(-1, -1, true);

	// read default spawn coords
	data >> spawn_x >> spawn_y;

	// set player to spawn if requested
	if (respawn)
		parent->getPlayer()->setMapPos(spawn_x, spawn_y, false);

	int pos_x, pos_y;
	std::filesystem::path path;

	while (data >> pos_x >> pos_y >> path) {
		// expand map storage if needed
		resizeMapStorage(pos_x, pos_y);

		if (path.extension() == ".png") {
			// load tile
			bool coll;
			int layer;
			data >> coll >> layer;

			tile[pos_x][pos_y][layer] = texture_manager->loadTexture(path);
			collision[pos_x][pos_y] = coll;
		} else if (path.extension() == ".txt") {
			// load object
			//int map_x, map_y;
			//data >> map_x >> map_y;
			parent->loadObject(path, pos_x, pos_y);
		}
	}
}

void MapManager::getSpawn(int *x, int *y)
{
	*x = spawn_x;
	*y = spawn_y;
}

 bool MapManager::getCollision(int pos_x, int pos_y)
{
	if (pos_x < 0 or pos_y < 0 or pos_x >= collision.size() or pos_y >=
	    collision[pos_x].size())
		return true; // default collision for OOB

	return collision[pos_x][pos_y];
}

void MapManager::getSize(int *x, int *y)
{
	*x = tile.size();

	if (not tile.empty())
		*y = tile[0].size();
	else
		*y = 0;
}

void MapManager::render()
{
	for (int i = 0; i < tile.size(); ++i)
		for (int j = 0; j < tile[i].size(); ++j) {
			if (tile[i][j][0] != texture_manager->getMissingTexture())
				renderer->addRenderItem(tile[i][j][0], i * TILE_SIZE, j * TILE_SIZE, false, false, 0);

			if (tile[i][j][1] != texture_manager->getMissingTexture())
				renderer->addRenderItem(tile[i][j][1], i * TILE_SIZE, j * TILE_SIZE, false, false, 2);
		}
}

void MapManager::resizeMapStorage(int x, int y, bool absolute)
{
	// resize so that tile[x][y] is a valid position
	++x; ++y;
	int size_x, size_y;

	if (absolute) {
		size_x = x;
		size_y = y;
	} else {
		size_x = tile.size() > x ? tile.size() : x;

		if (not tile.empty())
			size_y = tile[0].size() > y ? tile[0].size() : y;
		else
			size_y = y;
	}

	tile.resize(size_x);
	collision.resize(size_x);

	for (int i = 0; i < size_x; ++i) {
		tile[i].resize(size_y);

		for (int j = 0; j < tile[i].size(); ++j)
			tile[i][j].resize(2);

		collision[i].resize(size_y, 1);
	}
}

GameObject::GameObject(GameManager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	input_handler(parent->getManager()->getInputHandler()),
	map_manager(parent->getMapManager()),
	object_walker(nullptr),
	current_frame(0),
	loop_frame(0),
	end_frame(0),
	stop_frame(0),
	dir(DOWN),
	camera_center(false),
	size_x(1),
	size_y(1),
	collision(false)
{
	// default position off screen
	setMapPos(-1, -1, false);

	// initialize textures with missing texture
	TextureManager *texture_manager = renderer->getTextureManager();

	up.resize(1);
	down.resize(1);
	side.resize(1);

	/*
	up[0] = texture_manager->getMissingTexture();
	down[0] = texture_manager->getMissingTexture();
	side[0] = texture_manager->getMissingTexture();
	*/
}

GameObject::~GameObject()
{
	if (object_walker)
		delete object_walker;
}

void GameObject::setScreenPos(int x, int y, bool anim)
{
	if (object_walker and anim)
		object_walker->setDestination(x, y);
	else
		anim = false;

	if (not anim) {
		// skip animation, set to destination
		screen_x = x;
		screen_y = y;
	}
}

void GameObject::getScreenPos(int *x, int *y)
{
	*x = screen_x;
	*y = screen_y;
}

void GameObject::getCenter(int *x, int *y)
{
	*x = screen_x + TILE_SIZE * size_x / 2;
	*y = screen_y + TILE_SIZE * size_y / 2;
}

bool GameObject::isCameraCenter()
{
	return camera_center;
}

void GameObject::setMapPos(int x, int y, bool anim)
{
	map_x = x;
	map_y = y;

	// update ObjectWalker
	if (object_walker)
		object_walker->setDestination(map_x * TILE_SIZE, map_y * TILE_SIZE);
	else
		anim = false;

	if (not anim) {
		// if not animating, move now
		screen_x = map_x * TILE_SIZE;
		screen_y = map_y * TILE_SIZE;
	}
}

void GameObject::getMapPos(int *x, int *y)
{
	*x = map_x;
	*y = map_y;
}

void GameObject::getSize(int *x, int *y)
{
	*x = size_x;
	*y = size_y;
}

bool GameObject::checkMapCollision(int offset_x, int offset_y)
{
	int tmp_x = map_x + offset_x;
	int tmp_y = map_y + offset_y;

	//bool coll = false;

	for (int x = 0; x < size_x; ++x)
		for (int y = 0; y < size_y; ++y)
			//coll = coll or map_manager->getCollision(tmp_x + x, tmp_y + y);
			if (map_manager->getCollision(tmp_x + x, tmp_y + y))
				return true;

	//return coll;
	return false;
}

bool GameObject::checkObjectCollision(int offset_x, int offset_y)
{
	int tmp_x = map_x + offset_x;
	int tmp_y = map_y + offset_y;

	GameObject *coll = nullptr;

	for (int x = 0; x < size_x; ++x)
		for (int y = 0; y < size_y; ++y)
			if (parent->getCollision(tmp_x + x, tmp_y + y) != this)
				coll = std::max(
					coll, 
					parent->getCollision(tmp_x + x, tmp_y + y)
				);

	if (coll)
		return coll->collide();
	else
		return false;
}

void GameObject::render()
{
	switch (dir) {
	case UP:
		renderer->addRenderItem(up[current_frame], screen_x, screen_y, false, false, 1);
		break;
	case LEFT:
		renderer->addRenderItem(side[current_frame], screen_x, screen_y, false, false, 1);
		break;
	case DOWN:
		renderer->addRenderItem(down[current_frame], screen_x, screen_y, false, false, 1);
		break;
	case RIGHT:
		renderer->addRenderItem(side[current_frame], screen_x, screen_y, true, false, 1);
		break;
	}
}

bool GameObject::collide()
{
	// reference implementation
	return collision;
}

void GameObject::runTick(uint64_t delta)
{
	// run ObjectWalker
	if (object_walker)
		object_walker->runTick(delta);
}

/*
void GameObject::_setScreenPos(int x, int y)
{
	screen_x = x;
	screen_y = y;
}
*/

void GameObject::advanceFrame(DIR dir)
{
	this->dir = dir;
	if (++current_frame > end_frame) {
		current_frame = loop_frame;
	}
}

void GameObject::stopFrame(DIR dir)
{
	//this->dir = dir;
	//current_frame = current_frame != stop_frame ? stop_frame : 0;
	if (current_frame != stop_frame and current_frame != 0)
		current_frame = stop_frame;
	else if (current_frame != 0)
		current_frame = 0;
}

ObjectWalker::ObjectWalker(GameObject *parent) :
	parent(parent)
{
	/* 
	 * useless, since constructor will set correct values
	 * when available
	 */
	//parent->getScreenPos(&dest_x, &dest_y);
}

void ObjectWalker::setDestination(int x, int y)
{
	dest_x = x;
	dest_y = y;

	// respond instantly to new destination
	movement_deadline = 0;
	animation_deadline = 0;
}

/*
void ObjectWalker::cancel()
{
	parent->getScreenPos(&dest_x, &dest_y);
}
*/

void ObjectWalker::runTick(uint64_t delta)
{
	tick += delta;

	if (movement_deadline < tick) {
		// temporary screen coords
		int tmp_x, tmp_y;
		// invert move direction
		int sgn_x, sgn_y;

		// get current position
		parent->getScreenPos(&tmp_x, &tmp_y);
		sgn_x = sgn(dest_x - tmp_x);
		sgn_y = sgn(dest_y - tmp_y);

		// move object
		tmp_x += sgn_x ;
		tmp_y += sgn_y;

		// check overshoot
		if (sgn_x != sgn(dest_x - tmp_x)) {
			tmp_x = dest_x;
			sgn_x = 0;
		}

		if (sgn_y != sgn(dest_y - tmp_y)) {
			tmp_y = dest_y;
			sgn_y = 0;
		}

		// get movement direction
		DIR dir = UP;
		if (std::abs(dest_x - tmp_x) > std::abs(dest_y - tmp_y)) {
			if (sgn_x >= 0)
				dir = LEFT;
			else
				dir = RIGHT;
		} else {
			if (sgn_y >= 0)
				dir = DOWN;
			else
				dir = UP;
		}

		// send screen position to object
		parent->setScreenPos(tmp_x, tmp_y, false);

		movement_deadline = tick + SPEED;

		// send animation data to object
		if (animation_deadline < tick) {
			if (sgn_x == 0 and sgn_y == 0)
				parent->stopFrame(dir);
			else
				parent->advanceFrame(dir);

			animation_deadline = tick + FRAME_TIME;
		}
	}
}

Player::Player(GameManager *parent, int type) :
	GameObject(parent),
	type(type)
{
	// construct ObjectWalker
	object_walker = new ObjectWalker(this);

	// make camera center
	camera_center = true;

	// set correct size
	size_x = 2;
	size_y = 2;

	// enable collision
	collision = true;

	// load spawn location
	int tmp_x, tmp_y;
	map_manager->getSpawn(&tmp_x, &tmp_y);
	setMapPos(tmp_x, tmp_y, false);

	// load textures
	// TODO: load male/female based on choice
	// TODO: improve texture loading logic

	TextureManager *texture_manager = renderer->getTextureManager();

	/*
	 * keep all frames in order
	 * even if duplicated
	 * pointers don't take much memory
	 * and it simplifies code massively
	 */

	// set ObjectWalker frames
	loop_frame = 1;
	end_frame = 4;
	stop_frame = 5;

	up.resize(stop_frame + 1);
	down.resize(stop_frame + 1);
	side.resize(stop_frame + 1);

	switch (type) {
	case 0:
		up[0] = texture_manager->loadTexture("data/sprite/mc_male/u.png");
		up[1] = texture_manager->loadTexture("data/sprite/mc_male/u1.png");
		up[2] = texture_manager->loadTexture("data/sprite/mc_male/u.png");
		up[3] = texture_manager->loadTexture("data/sprite/mc_male/u2.png");
		up[4] = texture_manager->loadTexture("data/sprite/mc_male/u.png");
		up[5] = texture_manager->loadTexture("data/sprite/mc_male/u.png");

		down[0] = texture_manager->loadTexture("data/sprite/mc_male/d.png");
		down[1] = texture_manager->loadTexture("data/sprite/mc_male/d1.png");
		down[2] = texture_manager->loadTexture("data/sprite/mc_male/d.png");
		down[3] = texture_manager->loadTexture("data/sprite/mc_male/d2.png");
		down[4] = texture_manager->loadTexture("data/sprite/mc_male/d.png");
		down[5] = texture_manager->loadTexture("data/sprite/mc_male/d.png");

		side[0] = texture_manager->loadTexture("data/sprite/mc_male/s.png");
		side[1] = texture_manager->loadTexture("data/sprite/mc_male/s1.png");
		side[2] = texture_manager->loadTexture("data/sprite/mc_male/s.png");
		side[3] = texture_manager->loadTexture("data/sprite/mc_male/s2.png");
		side[4] = texture_manager->loadTexture("data/sprite/mc_male/s.png");
		side[5] = texture_manager->loadTexture("data/sprite/mc_male/s3.png");

		break;

	case 1:
		up[0] = texture_manager->loadTexture("data/sprite/mc_female/u.png");
		up[1] = texture_manager->loadTexture("data/sprite/mc_female/u1.png");
		up[2] = texture_manager->loadTexture("data/sprite/mc_female/u.png");
		up[3] = texture_manager->loadTexture("data/sprite/mc_female/u2.png");
		up[4] = texture_manager->loadTexture("data/sprite/mc_female/u.png");
		up[5] = texture_manager->loadTexture("data/sprite/mc_female/u.png");

		down[0] = texture_manager->loadTexture("data/sprite/mc_female/d.png");
		down[1] = texture_manager->loadTexture("data/sprite/mc_female/d1.png");
		down[2] = texture_manager->loadTexture("data/sprite/mc_female/d.png");
		down[3] = texture_manager->loadTexture("data/sprite/mc_female/d2.png");
		down[4] = texture_manager->loadTexture("data/sprite/mc_female/d.png");
		down[5] = texture_manager->loadTexture("data/sprite/mc_female/d.png");

		side[0] = texture_manager->loadTexture("data/sprite/mc_female/s.png");
		side[1] = texture_manager->loadTexture("data/sprite/mc_female/s1.png");
		side[2] = texture_manager->loadTexture("data/sprite/mc_female/s.png");
		side[3] = texture_manager->loadTexture("data/sprite/mc_female/s2.png");
		side[4] = texture_manager->loadTexture("data/sprite/mc_female/s.png");
		side[5] = texture_manager->loadTexture("data/sprite/mc_female/s3.png");

		break;
	}
}

void Player::runTick(uint64_t delta)
{
	// run base class tick
	GameObject::runTick(delta);

	// get input
	if (current_frame == 0 or 
	    current_frame == stop_frame) {
		if (((input_handler->isPlayer(UP) and type == 0) or 
		    (input_handler->isPlayer2(UP) and type == 1)) and
		    not checkMapCollision(0, -1) and
		    not checkObjectCollision(0, -1)) {
			setMapPos(map_x, map_y - 1);
			//return;
		}

		if (((input_handler->isPlayer(RIGHT) and type == 0) or
		    (input_handler->isPlayer2(RIGHT) and type == 1)) and
		    not checkMapCollision(1, 0) and
		    not checkObjectCollision(1, 0)) {
			setMapPos(map_x + 1, map_y);
			//return;
		}

		if (((input_handler->isPlayer(DOWN) and type == 0) or
		    (input_handler->isPlayer2(DOWN) and type == 1)) and
		    not checkMapCollision(0, 1) and
		    not checkObjectCollision(0, 1)) {
			setMapPos(map_x, map_y + 1);
			//return;
		}

		if (((input_handler->isPlayer(LEFT) and type == 0) or
		    (input_handler->isPlayer2(LEFT) and type == 1)) and
		    not checkMapCollision(-1, 0) and
		    not checkObjectCollision(-1, 0)) {
			setMapPos(map_x - 1, map_y);
			//return;
		}
	}
}

StaticObject::StaticObject(
	GameManager *parent,
	std::filesystem::path texture_path,
	int size_x, int size_y,
	int map_x, int map_y
) : GameObject(parent)
{
	this->size_x = size_x;
	this->size_y = size_y;
	collision = true;
	setMapPos(map_x, map_y, false);

	TextureManager *texture_manager = renderer->getTextureManager();
	auto tex = texture_manager->loadTexture(texture_path);

	up[0] = tex;
	down[0] = tex;
	side[0] = tex;
}

PickupObject::PickupObject(
	GameManager *parent,
	std::filesystem::path texture_path,
	int size_x, int size_y,
	int map_x, int map_y,
	std::string hint
) : 
	GameObject(parent),
	hint(hint)
{
	this->size_x = size_x;
	this->size_y = size_y;
	setMapPos(map_x, map_y, false);

	TextureManager *texture_manager = renderer->getTextureManager();
	auto tex = texture_manager->loadTexture(texture_path);

	up[0] = tex;
	down[0] = tex;
	side[0] = tex;

	parent->addCollectible();
}

bool PickupObject::collide()
{
	/* 
	 * override collision
	 * to handle pick-up
	 */

	parent->getQuizManager()->startQuiz();
	parent->useCollectible();
	parent->addHint(hint);
	parent->unloadObject(this);

	return false;
}

QuizManager::QuizManager(GameManager *parent) :
	parent(parent),
	//ui_manager(parent->getManager()->getUIManager()),
	in_quiz(false),
	question_asked(-1),
	have_answer(false)
{
	// TODO: implement
	std::ifstream question_file("data/questions.txt");

	QUESTION temp;
	temp.answers.resize(3);
	temp.correct.resize(3);

	while (not question_file.eof()) {
		std::getline(question_file, temp.text);
		std::getline(question_file, temp.answers[0]);
		std::getline(question_file, temp.answers[1]);
		std::getline(question_file, temp.answers[2]);
		// ugly fix since vector<bool> is weird
		bool correct0, correct1, correct2;
		question_file >> correct0 >> correct1
			      >> correct2 >> std::ws;
		temp.correct[0] = correct0;
		temp.correct[1] = correct1;
		temp.correct[2] = correct2;

		questions.push_back(temp);
	}
}

void QuizManager::startQuiz()
{
	in_quiz = true;
}

void QuizManager::provideAnswer(std::vector<bool> answer)
{
	this->answer = answer;
	this->answer.resize(3, 0);
	have_answer = true;
}

void QuizManager::runTick(uint64_t delta)
{
	// TODO: implement
	if (not in_quiz) {
		//question_asked = -1;
		//have_answer = false;

		return;
	}

	//parent->setPaused(true);

	static std::random_device r;
	static std::default_random_engine e(r());
	static std::uniform_int_distribution<int> dist(0, questions.size() - 1);
	static UIManager *ui_manager = parent->getManager()->getUIManager();

	if (question_asked < 0) {
		question_asked = dist(e);
		ui_manager->displayQuiz(
			questions[question_asked].text,
			questions[question_asked].answers
		);
	}

	if (have_answer) {
		have_answer = false;

		// TODO: inform ui
		if (answer == questions[question_asked].correct) {
			in_quiz = false;
			ui_manager->endQuiz();
		} else {
			//parent->setPaused(false);
		}

		question_asked = -1;
	}

	//parent->setPaused(false);
	//in_quiz = true;
}

GameManager::GameManager(Manager *parent) :
	parent(parent),
	renderer(parent->getRenderer()),
	map_manager(this),
	quiz_manager(this),
	playtime(0),
	paused(false),
	collectibles(0),
	collected(0)
{
	// set renderer size
	// aspect ratio is 4:3 for classy feel
	static const int multiplier = 5;
	renderer->setSize(4 * multiplier * TILE_SIZE, 3 * multiplier * TILE_SIZE);

	// player should always be first object
	objects.push_back(new Player(this, 0));
	//objects.push_back(new Player(this, 1));
	
	// load first hint
	std::ifstream firsthint("data/firsthint.txt");
	std::string hint;
	std::getline(firsthint, hint);
	hints.push_back(hint);
	
	// debug
	map_manager.loadMap(2, true);
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

QuizManager *GameManager::getQuizManager()
{
	return &quiz_manager;
}

Player *GameManager::getPlayer()
{
	return static_cast<Player *>(objects.front());
}

void GameManager::loadObject(std::filesystem::path object_path, int map_x, int map_y)
{
	std::ifstream object_file(object_path);

	// get object type
	std::string type;
	object_file >> type;

	// load data based on type
	if (type == "static") {
		std::filesystem::path tex;
		int size_x, size_y;

		object_file >> tex >> size_x >> size_y;

		objects.push_back(new StaticObject(this, tex, size_x, size_y, map_x, map_y));
	} else if (type == "pickup") {
		std::filesystem::path tex;
		int size_x, size_y;
		std::string hint;

		object_file >> tex >> size_x >> size_y >> std::ws;
		std::getline(object_file, hint);

		objects.push_back(new PickupObject(this, tex, size_x, size_y, map_x, map_y, hint));
	}

	// TODO: add more types

	// update collision
	updateCollision();
}

void GameManager::unloadObject(GameObject *object)
{
	auto it = std::find(objects.begin(), objects.end(), object);

	if (it != objects.end()) {
		objects.erase(it);

		// update collision
		updateCollision();
	}
}

void GameManager::updateCollision()
{
	// make sure object collision map size is correct
	{
		int size_x, size_y;
		map_manager.getSize(&size_x, &size_y);

		collision.resize(size_x);

		for(int i = 0; i < collision.size(); ++i)
			collision[i].resize(size_x);
	}

	// construct up to date collision map
	for(int i = 0; i < collision.size(); ++i)
		for(int j = 0; j < collision[i].size(); ++j)
			collision[i][j] = nullptr;

	for (auto obj : objects) {
		int map_x, map_y, size_x, size_y;

		obj->getMapPos(&map_x, &map_y);
		obj->getSize(&size_x, &size_y);

		for(int i = map_x; i < map_x + size_x; ++i)
			for(int j = map_y; j < map_y + size_y; ++j) {
				if (i < 0 or i >= collision.size() or
				    j < 0 or j >= collision[i].size())
					continue;

				collision[i][j] = obj;	
			}
	}
}

GameObject *GameManager::getCollision(int pos_x, int pos_y)
{
	if (pos_x < 0 or pos_x >= collision.size() or 
	    pos_y < 0 or pos_y >= collision[pos_x].size())
		return nullptr;
	
	return collision[pos_x][pos_y];
}

uint64_t GameManager::getPlaytime()
{
	// almost an hour
	//return 30000000;
	
	return playtime;
}

void GameManager::setPaused(bool paused)
{
	this->paused = paused;
}

bool GameManager::getPaused()
{
	return paused;
}

int GameManager::getCollected()
{
	return collected;
}

int GameManager::getRemaining()
{
	return collectibles - collected;
}

int GameManager::getTotalCollectibles()
{
	return collectibles;
}

void GameManager::addCollectible()
{
	++collectibles;
}

void GameManager::useCollectible()
{
	++collected;
}

void GameManager::addHint(std::string hint)
{
	hints.push_back(hint);
}

std::list<std::string> GameManager::getHints()
{
	return hints;
}

void GameManager::runTick(uint64_t delta)
{
	// compute playtime
	if (not paused)
		playtime += delta;

	// update collision map
	updateCollision();

	// render map tiles
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
		// run object tick
		if (not paused)
			obj->runTick(delta);
		obj->render();

		// camera calculations
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

	if (camera_count > 0) {
		camera_x /= camera_count;
		camera_y /= camera_count;
	}

	//int size_x = std::abs(max_x - min_x);
	//int size_y = std::abs(max_y - min_y);
	//int multiplier = 5;

	//while (4 * multiplier * TILE_SIZE < size_x and 3 * multiplier * TILE_SIZE < size_y) 
		//++multiplier;

	renderer->setCenter(camera_x, camera_y);
	//renderer->setSize(4 * multiplier * TILE_SIZE, 3 * multiplier * TILE_SIZE);
	
	// update quiz if needed
	quiz_manager.runTick(delta);
}
