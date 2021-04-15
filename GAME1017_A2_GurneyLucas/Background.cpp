#include "Background.h"
#include "Engine.h"
#include "TextureManager.h"

void Background::Update()
{
	// Check if at end of journey.
	if (GetDst()->x <= m_endX) // Instead of m_endX: (m_startX - GetDst()->w)
		GetDst()->x = m_startX;
	// Scroll it.
	GetDst()->x -= m_scrollSpeed;
}

void Background::Render()
{
	SDL_RenderCopyF(Engine::Instance().GetRenderer(), TEMA::GetTexture("bg"), GetSrc(), GetDst());
}