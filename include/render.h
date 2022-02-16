#pragma once

#include <list>
#include <queue>
#include <string>
#include <functional>
#include <filesystem>

#ifdef __unix__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#elif _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#else
#error Unsupported platform
#endif

#define MAX_TILE_LAYER 5

enum COLOR {BLACK, GRAY, WHITE, RED, GREEN, BLUE};

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

	Texture(Renderer *renderer, std::filesystem::path path, bool keep = false);
	Texture(Renderer *renderer, std::string text, COLOR color = BLACK, bool keep = false);
	~Texture();

	SDL_Texture *getTexture();
	std::filesystem::path getPath();

	int getWidth();
	int getHeight();

	long addUsage();
	long remUsage();
	long getUsage();

	bool isKeep();

	bool operator==(const Texture &other) const;
};

class TextureAccess
{
private:
	Texture *texture;

public:
	TextureAccess(Texture *texture = nullptr);
	TextureAccess(const TextureAccess &other);
	~TextureAccess();

	Texture *operator()();

	bool operator==(const TextureAccess &other) const;
};

class TextureManager
{
private:
	Renderer *parent;
	std::list<Texture> textures;

public:
	TextureManager(Renderer *parent);

	TextureAccess getMissingTexture();
	TextureAccess loadTexture(std::filesystem::path path);
	TextureAccess makeText(std::string text, COLOR color = BLACK);
	void cleanup();
};

class RenderItem
{
private:
	TextureAccess texture;
	int pos_x;
	int pos_y;
	bool flip_vert;
	bool flip_horz;
	int layer;
	bool overlay;

public:
	RenderItem(TextureAccess texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer, bool overlay = false);

	/* are these setters really necessary?
	void setTexture(TextureAccess texture);
	void setX(int pos_x);
	void setY(int pos_y);
	void setLayer(int layer);
	*/

	TextureAccess getTexture() const;
	int getX() const;
	int getY() const;
	bool getFlipVert() const;
	bool getFlipHorz() const;
	int getLayer() const;
	bool getOverlay() const;

	auto operator<=>(const RenderItem &other) const;
	// defined by compiler since C++20
	//bool operator<(const RenderItem &other) const;
	//bool operator>(const RenderItem &other) const;
};

class Renderer
{
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	TextureManager *texture_manager;
	TTF_Font *font;
	int center_x, center_y;
	std::priority_queue<RenderItem, std::vector<RenderItem>, std::greater<RenderItem>>
	                render_queue;

public:
	Renderer();
	~Renderer();

	SDL_Renderer *getRenderer();
	TextureManager *getTextureManager();
	TTF_Font *getFont();

	void setSize(int width, int height);
	void setCenter(int x, int y);

	void addRenderItem(const RenderItem &item);
	void addRenderItem(TextureAccess texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer, bool overlay = false);

	void operator()();
};
