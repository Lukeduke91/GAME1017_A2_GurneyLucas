#pragma once
#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_
#include "GameObject.h"
#include <SDL.h>

class Background : public SpriteObject
{
private:
	float m_scrollSpeed, m_startX, m_endX; // m_endX is optional.
public:
	Background(const SDL_Rect s, const SDL_FRect d, float ss) :SpriteObject(s, d), m_scrollSpeed(ss)
	{
		m_startX = GetDst()->x;
		m_endX = m_startX - GetDst()->w;
	}
	void Update();
	void Render();
};

#endif