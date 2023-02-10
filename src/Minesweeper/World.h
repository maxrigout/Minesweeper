#pragma once

#include <vector>
#include <iostream>
#include <time.h>

#include "Renderer2D/Renderer2D.h"
#include "Math/vect2d.h"
#include "Tile.h"

struct World
{
	enum REVEAL_RESULT
	{
		UNKNOWN = -1,
		NONE,
		OK,
		MINE,
	};

	World() = default;
	~World();
	void Init(int w, int h, int nMines);
	bool CheckWin() const;

	Tile& GetTile(unsigned int x, unsigned int y);
	const Tile& GetTile(unsigned int x, unsigned int y) const;

	REVEAL_RESULT RevealTile(unsigned int x, unsigned int y);

	void DrawTiles(Renderer2D* renderer) const;

	void DrawTile(Renderer2D* renderer, int x, int y) const;

	const std::vector<vect2d> neighbors = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };
	int nb_mines;
	int nMinesRemaining;
	unsigned int width;
	unsigned int height;
	Tile* tiles_arr = nullptr;
	Tile junk;
};