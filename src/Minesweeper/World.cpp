#include "World.h"
#include "Math/vect2d.h"

World::~World()
{
	delete[] tiles_arr;
}

void World::Init(int w, int h, int nMines)
{
	if (tiles_arr != nullptr)
		return;
	width = w;
	height = h;
	nb_mines = nMines;
	nMinesRemaining = nMines;
	tiles_arr = new Tile[w * h];
	junk.is_revealed = true;
	junk.has_mine = true;
	junk.nb_mines = 9;
	junk.flag = Tile::FLAG_TYPE::NONE;
	time_t seed;
	time(&seed);
	srand(seed);
	for (int n = 0; n < nb_mines; n++)
	{
		int x;
		int y;
		do
		{
			x = rand() % width;
			y = rand() % height;
		} while (GetTile(x, y).has_mine);

		GetTile(x, y).has_mine = true;

		// for (const auto& n : neighbors)
		// {
		// 	Tile& neighborTile = GetTile(x + n.x, y + n.y);
		// 	neighborTile.nb_mines++;
		// }
	}
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Tile& tile = GetTile(i, j);
			tile.pos.x = i;
			tile.pos.y = j;

			if (tile.has_mine)
			{
				for (const auto& n : neighbors)
				{
					Tile& neighborTile = GetTile(i + n.x, j + n.y);
					if (!neighborTile.has_mine)
					{
						neighborTile.nb_mines++;
					}
				}
			}

			// for (const auto& n : neighbors)
			// {
			// 	Tile& neighborTile = GetTile(i + n.x, j + n.y);
			// 	if (neighborTile.has_mine)
			// 	{
			// 		tile.nb_mines++;
			// 	}
			// }
		}
	}
}

bool World::CheckWin() const
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			const Tile& tile = GetTile(i, j);
			if (!tile.is_revealed && !tile.has_mine) return false;
			if (tile.has_mine && tile.is_revealed) return false;
		}
	}
	return true;
}

Tile& World::GetTile(unsigned int x, unsigned int y)
{
	if (x < width && y < height)
		return tiles_arr[x + y * width];
	return junk;
}

const Tile& World::GetTile(unsigned int x, unsigned int y) const
{
	if (x < width && y < height)
		return tiles_arr[x + y * width];
	return junk;
}

World::REVEAL_RESULT World::RevealTile(unsigned int x, unsigned int y)
{
	if (x < width && y < height) {
		Tile& tile = GetTile(x, y);
		if (!tile.is_revealed && tile.flag != Tile::FLAG_TYPE::MINE) {
			if (!tile.has_mine && tile.nb_mines != 0)
			{
				tile.is_revealed = true;
				return OK;
			}
			else if (!tile.has_mine && tile.nb_mines == 0)
			{
				tile.is_revealed = true;
				for (const auto& n : neighbors)
				{
					if (!GetTile(x + n.x, y + n.y).is_revealed)
					{
						RevealTile(x + n.x, y + n.y);
					}
				}
				return OK;
			}
			else if (tile.has_mine)
			{
				tile.is_revealed = true;
				return MINE;
			}
			else
			{
				return UNKNOWN;
			}
		}
	}
	return UNKNOWN;
}

void World::DrawTiles(Renderer2D* renderer) const
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			DrawTile(renderer, i, j);
		}
	}
}

void World::DrawTile(Renderer2D* renderer, int x, int y) const
{
	int tile_width = renderer->tileWidth;
	int tile_height = renderer->tileHeight;
	int pad_x = 1;
	int pad_y = 1;
	Color col = GREY;
	const Tile& tile = GetTile(x, y);
	if (tile.is_revealed)
	{
		switch (tile.nb_mines)
		{
		case 0: col = WHITE; break;
		case 1: col = BLUE; break;
		case 2: col = GREEN; break;
		case 3: col = RED; break;
		case 4: col = MAGENTA; break;
		case 5: col = YELLOW; break;
		case 6: col = CYAN; break;
		case 7: col = DARK_GREEN; break;
		case 8: col = VERY_DARK_GREEN; break;
		default: col = GREY; break;
		}
	}
	SDL_Rect rect{ x * tile_width + pad_x,
		y * tile_height + pad_y,
		tile_width - pad_x * 2,
		tile_height - pad_y * 2 };
	SDL_SetRenderDrawColor(renderer->renderer, col.r, col.g, col.b, col.a);
	SDL_RenderFillRect(renderer->renderer, &rect);
	// FillRect(x * tile_width + pad_x, y * tile_height + pad_y, tile_width - pad_x * 2, tile_height - pad_y * 2, col);
	//int d = (tile_width - pad_x * 2 < tile_height - pad_y * 2) ? tile_width - pad_x * 2 : tile_height - pad_y * 2;
	//DrawCircle(x * tile_width + pad_x + d/2, y * tile_height + pad_y + d/2, d/2, col);
	//FillCircle((x+2) * tile_width + pad_x + d / 2, y * tile_height + pad_y + d / 2, d / 2, col);
	//FillCircle2((x+1) * tile_width + pad_x + d / 2, y * tile_height + pad_y + d / 2, d / 2, col);
	//FillCircle3(x * tile_width + pad_x + d/2, y * tile_height + pad_y + d/2, d/2, col);
	if (!tile.is_revealed) {
		// (x1,y1)--------(x5,y1)--------(x2,y2)
		//    |                             |
		//    |                             |
		// (x3,y3)--------(x5,y3)--------(x4,y4)
		int x1 = x * tile_width + pad_x;
		int y1 = y * tile_height + pad_y;
		int x2 = x * tile_width - pad_x * 2 + tile_width;
		int y2 = y1;
		int x3 = x1;
		int y3 = y * tile_height - 2 * pad_y + tile_height;
		int x4 = x2;
		int y4 = y3;
		int x5 = x1 + tile_width / 2;
		SDL_SetRenderDrawColor(renderer->renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
		switch (tile.flag)
		{
		case Tile::FLAG_TYPE::MINE:
			SDL_RenderDrawLine(renderer->renderer, x1, y1, x4, y4);
			SDL_RenderDrawLine(renderer->renderer, x2, y2, x3, y3);
			// DrawLine(x1, y1, x4, y4, olc::BLACK);
			// DrawLine(x2, y2, x3, y3, olc::BLACK);
			break;
		case Tile::FLAG_TYPE::QUESTION:
			SDL_RenderDrawLine(renderer->renderer, x5, y1, x5, y3);
			// DrawLine(x5, y1, x5, y3, olc::BLACK);
			break;
		default: break;
		}
	}
	if (tile.is_revealed && tile.has_mine) {
		int x_width = tile_width - pad_x * 2;
		int y_height = tile_height - pad_y * 2;
		int diameter = (x_width < y_height) ? x_width : y_height;
		vect2d center{ x * tile_width + pad_x + diameter / 2, y * tile_height + pad_y + diameter / 2 };
		renderer->FillCircle(center, diameter / 2, VERY_DARK_RED);
		// FillCircle(x * tile_width + pad_x + d / 2, y * tile_height + pad_y + d / 2, diameter / 2, olc::VERY_DARK_RED);
	}
}