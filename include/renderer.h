#pragma once

#include <list>
#include <queue>
#include <string>
#include <functional>
#include <filesystem>

#include <SDL2/SDL.h>

#define MAX_TILE_LAYER 5

class Renderer;

class Texture
{
private:
	SDL_Texture *texture;
	std::filesystem::path path;
	int width;
	int height;
	long usage;
	bool keep;

public:
	Texture(SDL_Renderer *renderer, std::filesystem::path path, bool keep = false);
	~Texture();

	SDL_Texture *getTexture();
	std::filesystem::path getPath();

	int getWidth();
	int getHeight();

	long addUsage();
	long remUsage();
	long getUsage();

	bool isKeep();
};

class TextureManager
{
private:
	Renderer *parent;
	std::list<Texture> textures;

public:
	TextureManager(Renderer *parent);
	std::list<Texture>::iterator getMissingTexture();
	std::list<Texture>::iterator loadTexture(std::filesystem::path path);
	void unloadTexture(std::list<Texture>::iterator texture);
	void cleanup();
};

class RenderItem
{
private:
	std::list<Texture>::iterator texture;
	int pos_x;
	int pos_y;
	bool flip_vert;
	bool flip_horz;
	int layer;

public:
	RenderItem(std::list<Texture>::iterator texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer);

	/* are these setters really necessary?
	void setTexture(std::list<Texture>::iterator texture);
	void setX(int pos_x);
	void setY(int pos_y);
	void setLayer(int layer);
	*/

	std::list<Texture>::iterator getTexture() const;
	int getX() const;
	int getY() const;
	bool getFlipVert() const;
	bool getFlipHorz() const;
	int getLayer() const;

	bool operator<(const RenderItem &other) const;
	bool operator>(const RenderItem &other) const;
};

class Renderer
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	TextureManager *texture_manager;
	int center_x, center_y;
	std::priority_queue<RenderItem, std::vector<RenderItem>, std::greater<RenderItem>>
	                render_queue;

	friend class TextureManager;

public:
	Renderer();
	~Renderer();

	TextureManager *getTextureManager();

	void setSize(int width, int height);
	void setCenter(int x, int y);

	void addRenderItem(const RenderItem &item);
	void addRenderItem(std::list<Texture>::iterator texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer);

	void render();
};
