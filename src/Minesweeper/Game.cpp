
#include <iostream>
#include "Game.h"
#include "Tile.h"

bool isButton(int b, MouseButton button)
{
	uint32_t btn = 0;
	switch (button)
	{
	case MouseButton::LEFT:
		btn = SDL_BUTTON_LEFT;
		break;
	case MouseButton::RIGHT:
		btn = SDL_BUTTON_RIGHT;
		break;
	case MouseButton::MIDDLE:
		btn = SDL_BUTTON_MIDDLE;
		break;
	
	default:
		break;
	}
	return b == btn;
}

void Game::Init(int width, int height)
{
	InitSDL(width, height);
	InitWorld();
}

void Game::InitSDL(int width, int height)
{
	SDL_Init(SDL_INIT_VIDEO);

	// SDL_WindowFlags windowFlags = SDL_WINDOW_VULKAN;

	m_window = SDL_CreateWindow(
		"Minesweeper", // window title
		SDL_WINDOWPOS_UNDEFINED, // window x
		SDL_WINDOWPOS_UNDEFINED, // window y
		width, // width in px
		height, // height in px
		0
	);

	m_sdlRenderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	m_renderer.renderer = m_sdlRenderer;
	m_renderer.windowWidth = width;
	m_renderer.windowHeight = height;
}

void Game::InitWorld()
{
	m_has_won = false;
	m_renderer.tileWidth = m_renderer.windowWidth / m_world_w;
	m_renderer.tileHeight = m_renderer.windowWidth / m_world_h;
	m_world.Init(m_world_w, m_world_h, m_nMines);
}

void Game::FreeSDL()
{
	SDL_DestroyRenderer(m_sdlRenderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Game::FreeWorld()
{

}

void Game::Run()
{
	if (!m_isInitialized)
		return;
	m_isPlaying = true;
	while(m_isPlaying)
	{
		HandleInput();
		Update(0.16);
		Render();
	}
}

void Game::Cleanup()
{
	FreeWorld();
	FreeSDL();
}

void Game::HandleInput()
{
	SDL_Event event;
	m_previousMouseButtonState = 0;
	m_currentMouseButtonState = 0;
	while (SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				m_isPlaying = false;
				break;

			case SDL_MOUSEMOTION:
				m_mousePos.x = event.motion.x;
				m_mousePos.y = event.motion.y;
				// std::cout << "SDL_MOUSEMOTION: (" << m_mousePos.x << ", " << m_mousePos.y << ")\n";
				break;

			case SDL_MOUSEBUTTONUP:
				m_previousMouseButtonState = m_mouseButtonState;
				m_mouseButtonState = 0;
				break;

			case SDL_MOUSEBUTTONDOWN:
				m_mouseButtonState = event.button.button;
				m_currentMouseButtonState = m_mouseButtonState;
				break;

			case SDL_MOUSEWHEEL:
				std::cout << "SDL_MOUSEWHEEL: " << event.wheel.y << "\n";
				break;

			case SDL_KEYDOWN:
				std::cout << "SDL_KEYDOWN: " << event.key.keysym.scancode << "\n";
				break;

			case SDL_KEYUP:
				std::cout << "SDL_KEYUP: " << event.key.keysym.scancode << "\n";
				break;
		}
	}
}

bool Game::IsMouseButtonJustPressed(MouseButton button)
{
	return isButton(m_currentMouseButtonState, button);
}

bool Game::IsMouseButtonJustReleased(MouseButton button)
{
	return isButton(m_previousMouseButtonState, button);
}

void Game::Update(float dt)
{
	vect2d worldMouse = { m_mousePos.x / m_renderer.tileWidth, m_mousePos.y / m_renderer.tileHeight };
	int click_result = -1;
	if (IsMouseButtonJustPressed(MouseButton::LEFT))
	{
		m_pressed_tile = worldMouse;
	}
	if (IsMouseButtonJustReleased(MouseButton::LEFT) && m_pressed_tile.x == worldMouse.x && m_pressed_tile.y == worldMouse.y)
	{
		click_result = m_world.RevealTile(m_pressed_tile.x, m_pressed_tile.y);
	}

	if (IsMouseButtonJustPressed(MouseButton::RIGHT))
	{
		Tile& tile = m_world.GetTile(worldMouse.x, worldMouse.y);
		if (!tile.is_revealed) {
			switch (tile.flag)
			{
			case Tile::FLAG_TYPE::NONE:
				tile.flag = Tile::FLAG_TYPE::MINE;
				m_world.nMinesRemaining--;
				std::cout << m_world.nMinesRemaining << " out of " << m_world.nb_mines << std::endl;
				break;
			case Tile::FLAG_TYPE::MINE:
				tile.flag = Tile::FLAG_TYPE::QUESTION;
				m_world.nMinesRemaining++;
				std::cout << m_world.nMinesRemaining << " out of " << m_world.nb_mines << std::endl;
				break;
			case Tile::FLAG_TYPE::QUESTION: tile.flag = Tile::FLAG_TYPE::NONE; break;
			default: tile.flag = Tile::FLAG_TYPE::NONE; break;
			}
		}
	}


	if (click_result == World::REVEAL_RESULT::MINE)
	{
		std::cout << "Boom" << std::endl;
	}

	if (m_world.CheckWin() && !m_has_won)
	{
		m_has_won = true;
		std::cout << "Congratulations you Win! Have cookie for your achievement." << std::endl;
	}
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer.renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
	SDL_RenderClear(m_renderer.renderer);
	m_world.DrawTiles(&m_renderer);
	SDL_RenderPresent(m_renderer.renderer);
}