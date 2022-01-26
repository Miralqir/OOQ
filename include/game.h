#pragma once

#include "utilities.h"
#include "manager.h"
#include "render.h"

#include <cstdint>
#include <vector>
#include <list>
#include <filesystem>

// necessary forward declarations
class Manager;
class GameManager;
class ObjectWalker;

class MapManager
{
private:
	GameManager *parent;
	Renderer *renderer;
	TextureManager *texture_manager;

	std::vector<std::filesystem::path> maps;

	int current_map;
	int spawn_x, spawn_y;
	std::vector<std::vector<std::list<Texture>::iterator>> tile;
	std::vector<std::vector<int>> collision;

public:
	MapManager(GameManager *parent);

	void loadMap(int map);

	void getSpawn(int *x, int *y);
	int getCollision(int pos_x, int pos_y);

	void render();

private:
	void resizeMapStorage(int x, int y, bool absolute = false);
};

class GameObject
{
	/*
	 * TODO: move Player code to GameObject
	 * stuff is decently generic already
	 */
protected:
	GameManager *parent;
	Renderer *renderer;
	InputHandler *input_handler;
	MapManager *map_manager;

	// render stuff
	ObjectWalker *object_walker;

	std::vector<std::list<Texture>::iterator> up;
	std::vector<std::list<Texture>::iterator> down;
	std::vector<std::list<Texture>::iterator> side;

	int current_frame;
	int loop_frame;
	int end_frame;
	int stop_frame;
	DIR dir;

	int screen_x;
	int screen_y;

	bool camera_center;

	// collision stuff
	int map_x;
	int map_y;
	int size_x;
	int size_y;

protected:
	GameObject(GameManager *parent);

public:
	virtual ~GameObject();

	void setScreenPos(int x, int y, bool anim = true);
	void getScreenPos(int *x, int *y);
	void getCenter(int *x, int *y);

	bool isCameraCenter();

	void setMapPos(int x, int y, bool anim = true);
	void getMapPos(int *x, int *y);
	void getSize(int *x, int *y);
	int checkCollision(int offset_x, int offset_y);

	void render();
	virtual void runTick(uint64_t delta);


private:
	// ObjectWalker specific
	void _setScreenPos(int x, int y);

	void advanceFrame(DIR dir);
	void stopFrame(DIR dir);

	friend class ObjectWalker;
};

class ObjectWalker
{
	/*
	 * a helper class to smoothly move
	 * game objects across tiles
	 */

private:
	// measured in ms/pixel
	const uint64_t SPEED = 5;
	// ms/animation frame, reccomended multiple of SPEED
	const uint64_t FRAME_TIME = SPEED * 10;

	GameObject *parent;
	int dest_x, dest_y;
	uint64_t tick;
	uint64_t movement_deadline;
	uint64_t animation_deadline;

public:
	ObjectWalker(GameObject *parent);

	void setDestination(int x, int y);
	//void cancel();

	void runTick(uint64_t delta);
};

class Player : public GameObject
{
private:
	int type;

public:
	Player(GameManager *parent, int type);
	~Player() = default;

	void runTick(uint64_t delta);
};

class GameManager
{
private:
	Manager *parent;
	Renderer *renderer;
	MapManager map_manager;
	std::list<GameObject *> objects;

	bool paused;

public:
	GameManager(Manager *parent);
	~GameManager();

	Manager *getManager();
	Renderer *getRenderer();
	MapManager *getMapManager();
	Player *getPlayer();

	void setPaused(bool paused);
	bool getPaused();

	int getCollected();
	int getRemaining();
	uint64_t getPlaytime();

	void runTick(uint64_t delta);
};
