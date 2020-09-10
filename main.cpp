#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>

const int world_w = 20;
const int world_h = 20;
const int nMines = 20;

struct vect2d
{
	int x;
	int y;
};

struct sTile
{
	static enum FLAG_TYPE
	{
		NONE = 0,
		MINE,
		QUESTION,
	};

	bool is_revealed = false;
	bool has_mine = false;
	int nb_mines = 0;
	FLAG_TYPE flag = FLAG_TYPE::NONE;
	int x = -1;
	int y = -1;
};

struct World
{
	static enum REVEAL_RESULT
	{
		UNKNOWN = -1,
		NONE,
		OK,
		MINE,
	};

	World(int w, int h, int nMines)
	{

		width = w;
		height = h;
		nb_mines = nMines;
		nMinesRemaining = nMines;
		tiles_arr = new sTile[w * h];
		junk.is_revealed = true;
		junk.has_mine = true;
		junk.nb_mines = 9;
		junk.flag = sTile::FLAG_TYPE::NONE;
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
			GetTile(x, y).nb_mines = -1;
		}
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				GetTile(i, j).x = i;
				GetTile(i, j).y = j;

				if (GetTile(i, j).has_mine)
				{
					for (auto& n : neighbors)
					{
						if (!GetTile(i + n.x, j + n.y).has_mine)
						{
							GetTile(i + n.x, j + n.y).nb_mines++;
						}
					}
				}
			}
		}
	}
	~World()
	{
		delete[] tiles_arr;
	}

	bool CheckWin()
	{
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				sTile& tile = GetTile(i, j);
				if (!tile.is_revealed && !tile.has_mine) return false;
				if (tile.has_mine && tile.is_revealed) return false;
			}
		}
		return true;
	}

	sTile& GetTile(unsigned int x, unsigned int y) {
		if (x < width && y < height)
		{
			return tiles_arr[x + y * width];
		}
		return junk;
	}

	REVEAL_RESULT RevealTile(unsigned int x, unsigned int y)
	{
		if (x < width && y < height) {
			sTile& tile = GetTile(x, y);
			if (!tile.is_revealed && tile.flag != sTile::FLAG_TYPE::MINE) {
				if (!tile.has_mine && tile.nb_mines != 0)
				{
					tile.is_revealed = true;
					return OK;
				}
				else if (!tile.has_mine && tile.nb_mines == 0)
				{
					tile.is_revealed = true;
					for (auto& n : neighbors)
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

	std::vector<vect2d> neighbors = { {-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1} };
	int nb_mines;
	int nMinesRemaining;
	unsigned int width;
	unsigned int height;
	sTile* tiles_arr;
	sTile junk;
};

class MineSweeper : public olc::PixelGameEngine
{
public:
	bool OnUserCreate() override
	{
		has_won = false;
		sAppName = "Mine Sweeper";
		tile_width = ScreenWidth() / world_w;
		tile_height = ScreenHeight() / world_h;
		world = new World(world_w, world_h, nMines);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		vect2d screenMouse = { GetMouseX(), GetMouseY() };
		vect2d worldMouse = { GetMouseX() / tile_width, GetMouseY() / tile_height };
		int click_result = -1;
		if (GetMouse(0).bPressed)
		{
			pressed_tile = worldMouse;
		}
		if (GetMouse(0).bReleased && pressed_tile.x == worldMouse.x && pressed_tile.y == worldMouse.y)
		{
			click_result = world->RevealTile(pressed_tile.x, pressed_tile.y);
		}

		if (GetMouse(1).bPressed)
		{
			sTile& tile = world->GetTile(worldMouse.x, worldMouse.y);
			if (!tile.is_revealed) {
				int f = tile.flag;
				switch (f)
				{
				case sTile::FLAG_TYPE::NONE:
					tile.flag = sTile::FLAG_TYPE::MINE;
					world->nMinesRemaining--;
					std::cout << world->nMinesRemaining << " out of " << world->nb_mines << std::endl;
					break;
				case sTile::FLAG_TYPE::MINE:
					tile.flag = sTile::FLAG_TYPE::QUESTION;
					world->nMinesRemaining++;
					std::cout << world->nMinesRemaining << " out of " << world->nb_mines << std::endl;
					break;
				case sTile::FLAG_TYPE::QUESTION: tile.flag = sTile::FLAG_TYPE::NONE; break;
				default: tile.flag = sTile::FLAG_TYPE::NONE; break;
				}
			}
		}


		if (click_result == World::REVEAL_RESULT::MINE)
		{
			std::cout << "Boom" << std::endl;
		}

		if (world->CheckWin() && !has_won)
		{
			has_won = true;
			std::cout << "Congratulations you Win! Have cookie for your achievement." << std::endl;
		}

		Clear(olc::WHITE);

		for (int i = 0; i < world->width; i++)
		{
			for (int j = 0; j < world->height; j++)
			{
				DrawTile(i, j);
			}
		}
		return true;
	}

	bool OnUserDestroy() override
	{
		delete world;
		return true;
	}

	void DrawTile(int x, int y)
	{
		int pad_x = 1;
		int pad_y = 1;
		olc::Pixel col = olc::GREY;
		sTile& tile = world->GetTile(x, y);
		if (tile.is_revealed)
		{
			switch (tile.nb_mines)
			{
			case 0: col = olc::WHITE; break;
			case 1: col = olc::BLUE; break;
			case 2: col = olc::GREEN; break;
			case 3: col = olc::RED; break;
			case 4: col = olc::MAGENTA; break;
			case 5: col = olc::YELLOW; break;
			case 6: col = olc::CYAN; break;
			case 7: col = olc::DARK_GREEN; break;
			case 8: col = olc::VERY_DARK_GREEN; break;
			default: col = olc::GREY; break;
			}
		}
		else
		{
			col = olc::GREY;
		}
		FillRect(x * tile_width + pad_x, y * tile_height + pad_y, tile_width - pad_x * 2, tile_height - pad_y * 2, col);
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
			switch (tile.flag)
			{
			case sTile::FLAG_TYPE::MINE:
				DrawLine(x1, y1, x4, y4, olc::BLACK);
				DrawLine(x2, y2, x3, y3, olc::BLACK);
				break;
			case sTile::FLAG_TYPE::QUESTION:
				DrawLine(x5, y1, x5, y3, olc::BLACK);
				break;
			default: break;
			}
		}
		if (tile.is_revealed && tile.has_mine) {
			int d = (tile_width - pad_x * 2 < tile_height - pad_y * 2) ? tile_width - pad_x * 2 : tile_height - pad_y * 2;
			FillCircle(x * tile_width + pad_x + d / 2, y * tile_height + pad_y + d / 2, d / 2, olc::VERY_DARK_RED);
		}
	}

private:
	vect2d pressed_tile;
	int tile_width;
	int tile_height;
	World* world;
	bool has_won;
};

int main(int argc, char** argv)
{
	MineSweeper* game = new MineSweeper;

	int nFrameRate = 60;

	if (game->Construct(960, 800, 1, 1)) game->Start();

	delete game;

	return EXIT_SUCCESS;
}