#include "renderer.h"

#include "config.h"

#include <stdexcept>
#include <sstream>

#include <SDL2/SDL_image.h>

Texture::Texture(SDL_Renderer *renderer, std::filesystem::path path,
                 bool keep) :
	path(path),
	usage(1),
	keep(keep)
{
	if (!path.empty() and std::filesystem::exists(path)) {
		SDL_Surface *surface = IMG_Load(path.c_str());

		if (!surface)
			throw std::runtime_error(IMG_GetError());

		texture = SDL_CreateTextureFromSurface(renderer, surface);

		SDL_FreeSurface(surface);

		if (!texture)
			throw std::runtime_error(SDL_GetError());
	} else {
		// create missing texture
		SDL_Surface *missing = SDL_CreateRGBSurface(0, TILE_SIZE, TILE_SIZE, 32, 0, 0,
		                       0, 0);

		if (!missing)
			throw std::runtime_error(SDL_GetError());

		SDL_FillRect(missing, NULL, SDL_MapRGB(missing->format, 255, 192, 203));

		texture = SDL_CreateTextureFromSurface(renderer, missing);

		SDL_FreeSurface(missing);

		if (!texture)
			throw std::runtime_error(SDL_GetError());
	}

	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
}

Texture::~Texture()
{
	SDL_DestroyTexture(texture);
}

SDL_Texture *Texture::getTexture()
{
	return texture;
}

std::filesystem::path Texture::getPath()
{
	return path;
}

int Texture::getWidth()
{
	return width;
}

int Texture::getHeight()
{
	return height;
}

long Texture::addUsage()
{
	return ++usage;
}

long Texture::remUsage()
{
	return --usage;
}

long Texture::getUsage()
{
	return usage;
}

bool Texture::isKeep()
{
	return keep;
}

TextureManager::TextureManager(Renderer *parent) :
	parent(parent)
{
	// initialize missing texture
	textures.emplace_back(parent->renderer, "", true);
}

std::list<Texture>::iterator TextureManager::getMissingTexture()
{
	return textures.begin();
}

std::list<Texture>::iterator TextureManager::loadTexture(
        std::filesystem::path path)
{
	// TODO: check if path is already loaded, return that instead
	textures.emplace_back(parent->renderer, path);
	return --textures.end();
}

void TextureManager::unloadTexture(std::list<Texture>::iterator texture)
{
	texture->remUsage();
}

void TextureManager::cleanup()
{
	auto it = textures.begin();
	while (it != textures.end())
		if (it->getUsage() < 1 and not it->isKeep())
			it = textures.erase(it);
		else
			it++;
}

RenderItem::RenderItem(std::list<Texture>::iterator texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer) :
	texture(texture),
	pos_x(pos_x),
	pos_y(pos_y),
	flip_vert(flip_vert),
	flip_horz(flip_horz),
	layer(layer)
{}

/*
void RenderItem::setTexture(std::list<Texture>::iterator texture)
{
        this->texture = texture;
}

void RenderItem::setX(int pos_x)
{
        this->pos_x = pos_x;
}

void RenderItem::setY(int pos_y)
{
        this->pos_y = pos_y;
}

void RenderItem::setLayer(int layer)
{
        this->layer = layer;
}
*/

std::list<Texture>::iterator RenderItem::getTexture() const
{
	return texture;
}

int RenderItem::getX() const
{
	return pos_x;
}

int RenderItem::getY() const
{
	return pos_y;
}

bool RenderItem::getFlipVert() const
{
	return flip_vert;
}

bool RenderItem::getFlipHorz() const
{
	return flip_horz;
}

int RenderItem::getLayer() const
{
	return layer;
}

bool RenderItem::operator<(const RenderItem &other) const
{
	return layer < other.layer;
}

bool RenderItem::operator>(const RenderItem &other) const
{
	return layer > other.layer;
}


Renderer::Renderer() :
	center_x(0),
	center_y(0)
{
	window = SDL_CreateWindow(
	                 "Object Oriented Quest",
	                 SDL_WINDOWPOS_UNDEFINED,
	                 SDL_WINDOWPOS_UNDEFINED,
	                 800,
	                 600,
	                 SDL_WINDOW_RESIZABLE
	         );

	if (!window)
		throw std::runtime_error(SDL_GetError());

	renderer = SDL_CreateRenderer(
	                   window,
	                   -1,
	                   SDL_RENDERER_ACCELERATED |
	                   SDL_RENDERER_PRESENTVSYNC //|
	                   //SDL_RENDERER_TARGETTEXTURE
	           );

	if (!renderer)
		throw std::runtime_error(SDL_GetError());

	texture_manager = new TextureManager(this);

	SDL_Surface *surface = IMG_Load("data/logo/WSS.png");
	if (!surface)
		throw std::runtime_error(IMG_GetError());

	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);
}

Renderer::~Renderer()
{
	delete texture_manager;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

TextureManager *Renderer::getTextureManager()
{
	return texture_manager;
}

void Renderer::setSize(int width, int height)
{
	if (SDL_RenderSetLogicalSize(renderer, width, height))
		throw std::runtime_error(SDL_GetError());
}

void Renderer::setCenter(int x, int y)
{
	center_x = x;
	center_y = y;
}

void Renderer::addRenderItem(const RenderItem &item)
{
	render_queue.push(item);
}

void Renderer::addRenderItem(std::list<Texture>::iterator texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer)
{
	render_queue.emplace(texture, pos_x, pos_y, flip_vert, flip_horz, layer);
}

void Renderer::render()
{
	int screen_width, screen_height;

	SDL_RenderGetLogicalSize(renderer, &screen_width, &screen_height);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	while (!render_queue.empty()) {
		auto render_item = render_queue.top();
		auto tex = render_item.getTexture();

		SDL_Rect pos = {
			.x = render_queue.top().getX()
			     - center_x + screen_width / 2,
			.y = render_queue.top().getY()
			     - center_y + screen_height / 2,
			.w = tex->getWidth(),
			.h = tex->getHeight()
		};

		SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(
			(SDL_FLIP_VERTICAL and render_item.getFlipVert()) |
			(SDL_FLIP_VERTICAL and render_item.getFlipHorz())
		);

		render_queue.pop();

		SDL_RenderCopyEx(renderer, tex->getTexture(), NULL, &pos, 0, NULL, flip);
	}

	SDL_RenderPresent(renderer);
}
