#include "render.h"

#include "config.h"

#include <stdexcept>
#include <compare>
#include <sstream>

#include <SDL2/SDL_image.h>

Texture::Texture(Renderer *renderer, std::filesystem::path path, bool keep) :
	path(path),
	usage(1),
	keep(keep)
{
	SDL_Surface *surface;
	if (!path.empty() and std::filesystem::exists(path)) {
		surface = IMG_Load(path.c_str());

		if (!surface)
			throw std::runtime_error(IMG_GetError());
	} else {
		// create missing texture
		surface = SDL_CreateRGBSurface(
				0, 
				TILE_SIZE, TILE_SIZE, 32,
				0, 0, 0, 0
			);

		if (!surface)
			throw std::runtime_error(SDL_GetError());

		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 169, 169, 169));
	}

	texture = SDL_CreateTextureFromSurface(renderer->getRenderer(), surface);

	SDL_FreeSurface(surface);

	if (!texture)
		throw std::runtime_error(SDL_GetError());

	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
}

Texture::Texture(Renderer *renderer, std::string text, COLOR color, bool keep) :
	path(""),
	usage(1),
	keep(keep)
{
	// TODO: implement different colors
	SDL_Color color_real = {0, 0, 0};

	switch (color) {
	default:
	case BLACK:
		color_real = {0, 0, 0};
		break;

	case GRAY:
		color_real = {169, 169, 169};
		break;

	case WHITE:
		color_real = {255, 255, 255};
		break;

	case RED:
		color_real = {255, 0, 0};
		break;

	case GREEN:
		color_real = {0, 255, 0};
		break;

	case BLUE:
		color_real = {0, 0, 255};
		break;
	}

	SDL_Surface *surface = TTF_RenderUTF8_Solid(
			renderer->getFont(), 
			text.c_str(), 
			color_real
		);

	if (!surface)
		throw std::runtime_error(TTF_GetError());

	texture = SDL_CreateTextureFromSurface(renderer->getRenderer(), surface);

	SDL_FreeSurface(surface);

	if (!texture)
		throw std::runtime_error(SDL_GetError());

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

bool Texture::operator==(const Texture &other) const
{
	return path == other.path;
}

TextureAccess::TextureAccess(Texture *texture) :
	texture(texture)
{
	if (texture)
		texture->addUsage();
}

TextureAccess::TextureAccess(const TextureAccess &other) :
	texture(other.texture)
{
	if (texture)
		texture->addUsage();
}

TextureAccess::~TextureAccess()
{
	if (texture)
		texture->remUsage();
}

Texture *TextureAccess::operator()()
{
	return texture;
}

bool TextureAccess::operator==(const TextureAccess &other) const
{
	return texture == other.texture;
}

TextureManager::TextureManager(Renderer *parent) :
	parent(parent)
{
	// initialize missing texture
	textures.emplace_back(parent, std::filesystem::path(""), true);
}

TextureAccess TextureManager::getMissingTexture()
{
	return TextureAccess(&textures.front());
}

TextureAccess TextureManager::loadTexture(std::filesystem::path path)
{
	// TODO: check if path is already loaded, return that instead
	textures.emplace_back(parent, path);
	return TextureAccess(&textures.back());
}

TextureAccess TextureManager::makeText(std::string text, COLOR color)
{
	textures.emplace_back(parent, text, color);
	return TextureAccess(&textures.back());
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

RenderItem::RenderItem(TextureAccess texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer, bool overlay) :
	texture(texture),
	pos_x(pos_x),
	pos_y(pos_y),
	flip_vert(flip_vert),
	flip_horz(flip_horz),
	layer(layer),
	overlay(overlay)
{}

/*
void RenderItem::setTexture(TextureAccess texture)
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

TextureAccess RenderItem::getTexture() const
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

bool RenderItem::getOverlay() const
{
	return overlay;
}

auto RenderItem::operator<=>(const RenderItem &other) const
{
	if (layer == other.layer)
		return std::strong_ordering::equal;
	else if (layer < other.layer)
		return std::strong_ordering::less;
	return std::strong_ordering::greater;
}

/*
bool RenderItem::operator<(const RenderItem &other) const
{
	return layer < other.layer;
}

bool RenderItem::operator>(const RenderItem &other) const
{
	return layer > other.layer;
}
*/

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

	if (not window)
		throw std::runtime_error(SDL_GetError());

	renderer = SDL_CreateRenderer(
	                   window,
	                   -1,
	                   SDL_RENDERER_ACCELERATED |
	                   SDL_RENDERER_PRESENTVSYNC //|
	                   //SDL_RENDERER_TARGETTEXTURE
	           );

	if (not renderer)
		throw std::runtime_error(SDL_GetError());

	SDL_Surface *surface = IMG_Load("data/logo/WSS.png");
	if (not surface)
		throw std::runtime_error(IMG_GetError());

	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);

	texture_manager = new TextureManager(this);

	font = TTF_OpenFont("data/font/Hack-Regular.ttf", 10);

	if (not font)
		throw std::runtime_error(TTF_GetError());
}

Renderer::~Renderer()
{
	TTF_CloseFont(font);
	delete texture_manager;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

SDL_Renderer *Renderer::getRenderer()
{
	return renderer;
}

TextureManager *Renderer::getTextureManager()
{
	return texture_manager;
}

TTF_Font *Renderer::getFont()
{
	return font;
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

void Renderer::addRenderItem(TextureAccess texture, int pos_x, int pos_y, bool flip_vert, bool flip_horz, int layer, bool overlay)
{
	render_queue.emplace(texture, pos_x, pos_y, flip_vert, flip_horz, layer, overlay);
}

void Renderer::operator()()
{
	int screen_width, screen_height;

	SDL_RenderGetLogicalSize(renderer, &screen_width, &screen_height);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	while (!render_queue.empty()) {
		auto render_item = render_queue.top();
		auto tex = render_item.getTexture();

		if (tex()) {
			SDL_Rect pos;
			if (not render_item.getOverlay())
				pos = {
					.x = render_item.getX()
			     	     	     - center_x + screen_width / 2,
					.y = render_item.getY()
			     	             - center_y + screen_height / 2,
					.w = tex()->getWidth(),
					.h = tex()->getHeight()
				};
			else
				pos = {
					.x = render_item.getX(),
					.y = render_item.getY(),
					.w = tex()->getWidth(),
					.h = tex()->getHeight()
				};

			SDL_RendererFlip flip = static_cast<SDL_RendererFlip>(
				(SDL_FLIP_VERTICAL and 
				 render_item.getFlipVert()) |
				(SDL_FLIP_VERTICAL and
				 render_item.getFlipHorz())
			);

			SDL_RenderCopyEx(renderer, tex()->getTexture(), NULL, &pos, 0, NULL, flip);
		}

		render_queue.pop();
	}

	SDL_RenderPresent(renderer);
}
