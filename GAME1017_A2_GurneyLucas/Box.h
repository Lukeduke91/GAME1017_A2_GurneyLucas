#pragma once
#ifndef _BOX_H_
#define _BOX_H_
#include <SDL.h>

class Spritebg // Represents visual component of Box.
{
private:
	SDL_Rect m_dstbg; // Position on screen.
	SDL_Color m_colorbg; // Random color for box.
public:
	Spritebg();
	Spritebg(const SDL_Rect r, const SDL_Color c);
	void Renderbg();
	friend class Box; // Or make a setter for m_dst.
};

class Box // Obstacle parent/proxy for the Sprite.
{
private:
	SDL_Point m_posbg; // Position on screen. Y is optional.
	Spritebg* m_pSpritebg; // Pointer to dynamic array of sprites.
	int m_numSpritesbg;
public:
	Box(const SDL_Point p, bool hasSprite = false, int numSprites = 0,
		const SDL_Rect r = { 0,0,0,0 }, const SDL_Color c = { 255,255,255,255 });
	~Box();
	Box* Clonebg();
	void Updatebg();
	void Renderbg();
	const SDL_Point GetPosbg() { return m_posbg; }
	void Addspritebg(const int index, const SDL_Rect r, const SDL_Color c);
	void Addspritebg(const int index, const Spritebg& s);
};

#endif