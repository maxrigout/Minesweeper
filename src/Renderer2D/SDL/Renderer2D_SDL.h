#pragma once

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>

#include "Renderer2D/Renderer2D.h"

class Renderer2D_SDL : public Renderer2D
{
public:
	Renderer2D_SDL(SDL_Renderer* renderer);
	virtual ~Renderer2D_SDL();

	virtual void Begin();
	virtual void End();
	virtual void Clear(const Color& color) const;
	virtual void DrawDisk(vect2d center, int radius, const Color& color) const;
	virtual void DrawCircle(vect2d center, int radius, const Color& color) const;
	virtual void FillCircle(vect2d center, int radius, const Color& color) const;
	virtual void DrawRect(vect2d position, vect2d dimensions, const Color& color) const;
	virtual void FillRect(vect2d position, vect2d dimensions, const Color& color) const;
	virtual void DrawLine(vect2d start, vect2d end, const Color& color) const;
	virtual void DrawText(vect2d position, const std::string& text, const Color& color) const;

	virtual vect2d GetWindowDim() const;
	virtual vect2d GetCellDim() const;
	virtual vect2d GetViewPortDim() const;
	virtual void SetWindowDim(vect2d dim);
	virtual void SetCellDim(vect2d dim);
	virtual void SetViewPortDim(vect2d dim);

private:
	void SetRenderDrawColor(const Color& c) const { SDL_SetRenderDrawColor(m_renderer, c.r, c.g, c.b, c.a); }

	SDL_Renderer* m_renderer;
	vect2d m_windowDim;
	vect2d m_cellDim;
	vect2d m_viewPortDim;

	TTF_Font* defaultFont = nullptr;
};