#include "Box.h"
#include "Engine.h"
#define SCROLLSPD 2

Spritebg::Spritebg() :m_dstbg({ 0,0,0,0 }), m_colorbg({ 255,255,255,255 }) {}

Spritebg::Spritebg(const SDL_Rect r, const SDL_Color c) : m_dstbg(r), m_colorbg(c) {}

void Spritebg::Renderbg()
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), m_colorbg.r, m_colorbg.g, m_colorbg.b, m_colorbg.a);
	SDL_RenderFillRect(Engine::Instance().GetRenderer(), &m_dstbg);
}

Box::Box(const SDL_Point p, bool hasSprite, int numSprites,
	const SDL_Rect r, const SDL_Color c) :m_posbg(p), m_pSpritebg(nullptr), m_numSpritesbg(numSprites)
{
	if (hasSprite)
	{
		m_pSpritebg = new Spritebg[m_numSpritesbg];
		if (m_numSpritesbg == 1)
		{
			Spritebg newSprite(r, c);
			m_pSpritebg[0] = newSprite;
		}
	}
}

Box::~Box()
{
	if (m_numSpritesbg > 0) // Optional but reccomended. Canadian programming.
	{
		delete[] m_pSpritebg;
	}
}

Box* Box::Clonebg()
{
	Box* clone = new Box(m_posbg, (m_numSpritesbg > 0), m_numSpritesbg);
	for (int i = 0; i < m_numSpritesbg; i++)
	{
		clone->Addspritebg(i, this->m_pSpritebg[i]);
	}
	return clone;
}

void Box::Updatebg()
{
	m_posbg.x -= SCROLLSPD; // Scroll the box.
	for (int i = 0; i < m_numSpritesbg; i++)
	{
		m_pSpritebg[i].m_dstbg.x = m_posbg.x;
	}// Scrolling sprite with box.
}

void Box::Renderbg()
{
	// Render sprite.
	for (int i = 0; i < m_numSpritesbg; i++)
	{
		m_pSpritebg[i].Renderbg();
	}
	// Render white border. Extra for solution and supplemental video.
	SDL_Rect m_dstbg = { m_posbg.x, m_posbg.y, 128, 128 };
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), 128, 128, 128, 255);
	SDL_RenderDrawRect(Engine::Instance().GetRenderer(), &m_dstbg);
}

void Box::Addspritebg(const int index, const SDL_Rect r, const SDL_Color c)
{
	Spritebg newSprite(r, c);
	m_pSpritebg[index] = newSprite;
}

void Box::Addspritebg(const int index, const Spritebg& s)
{
	this->m_pSpritebg[index] = s;
}
