#pragma once 

#include "Math/vect2d.h"

struct Tile
{
	enum class FLAG_TYPE
	{
		NONE = 0,
		MINE,
		QUESTION,
	};

	bool is_revealed = false;
	bool has_mine = false;
	int nb_mines = 0;
	FLAG_TYPE flag = FLAG_TYPE::NONE;
	vect2d pos{-1, -1};
};