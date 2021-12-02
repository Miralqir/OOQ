#pragma once

#include "manager.h"
#include "renderer.h"

#include <cstdint>
#include <vector>
#include <list>
#include <filesystem>

class Manager;
class GameManager;

class MapManager
{
private:
	GameManager *parent;
	Renderer *renderer;
	//TextureManager *texture_manager;
	std::filesystem::path map;
	int spawn_x, spawn_y;
	std::vector<std::vector<std::list<Texture>::iterator>> tile;
	std::vector<std::vector<int>> collision;

public:
	MapManager(GameManager *parent);

	void loadMap(std::filesystem::path map);

	void getSpawn(int *x, int *y);
	int getCollision(int pos_x, int pos_y);

	void render();
};

class GameObject
{
public:
	virtual ~GameObject() = default;

	virtual bool isCameraCenter() = 0;

	virtual void setPos(int pos_x, int pos_y) = 0;
	virtual void getPos(int *pos_x, int *pos_y) = 0;
	virtual void getSize(int *size_x, int *size_y) = 0;
	virtual void getCenter(int *pos_x, int *pos_y) = 0;

	virtual void runTick(uint64_t delta) = 0;

};

class Player : public GameObject
{
private:
	GameManager *parent;
	Renderer *renderer;
	//TextureManager *texture_manager;
	InputHandler *input_handler;
	MapManager *map_manager;

	std::vector<std::list<Texture>::iterator> up;
	std::vector<std::list<Texture>::iterator> down;
	std::vector<std::list<Texture>::iterator> side;

	int type;
	uint64_t input_time;
	uint64_t input_deadline;
	int last_dir;

	int pos_x;
	int pos_y;

public:
	Player(GameManager *parent, int type);
	~Player() = default;

	bool isCameraCenter();

	void setPos(int pos_x, int pos_y);
	void getPos(int *pos_x, int *pos_y);
	void getSize(int *size_x, int *size_y);
	void getCenter(int *pos_x, int *pos_y);

	void centerCamera();
	int checkCollision(int offset_x, int offset_y);

	void runTick(uint64_t delta);
};

class GameManager
{
private:
	Manager *parent;
	Renderer *renderer;
	MapManager map_manager;
	std::list<GameObject *> objects;

public:
	GameManager(Manager *parent);
	~GameManager();

	Manager *getManager();
	Renderer *getRenderer();
	MapManager *getMapManager();
	Player *getPlayer();

	void runTick(uint64_t delta);
};
