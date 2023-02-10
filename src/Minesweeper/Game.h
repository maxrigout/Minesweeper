#pragma once
#include <SDL2/SDL.h>

#include "Math/vect2d.h"
#include "Renderer2D/Renderer2D.h"
#include "World.h"

enum class MouseButton
{
	LEFT = 1,
	RIGHT = 2,
	MIDDLE = 3,
};

class Game
{
public:
	Game() = default;
	~Game() = default;
	void Init(int width, int height);
	void Run();
	void Cleanup();

private:

	void InitSDL(int width, int height);
	void InitWorld();
	void FreeSDL();
	void FreeWorld();

	void HandleInput();
	void Update(float dt);
	void Render();

	bool IsMouseButtonJustPressed(MouseButton button);
	bool IsMouseButtonJustReleased(MouseButton button);


	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;
	vect2d m_mousePos;
	unsigned int m_currentMouseButtonState;
	unsigned int m_mouseButtonState;
	unsigned int m_previousMouseButtonState;

	Renderer2D m_renderer;

	bool m_isInitialized;
	bool m_isPlaying;

	World m_world;
	bool m_gameover = false;
	bool m_has_won = false;
	vect2d m_pressed_tile;

	// config
	int m_world_w = 20;
	int m_world_h = 20;
	int m_nMines = 20;
};