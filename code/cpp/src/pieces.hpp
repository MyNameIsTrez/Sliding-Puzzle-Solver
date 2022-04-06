#pragma once

////////

#include <limits> // std::numeric_limits<int>::max()

////////

struct Pos
{
	int x;
	int y;
	bool operator<(const Pos &right) const
	{
		long left_id = static_cast<long>(this->y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(this->x);
		long right_id = static_cast<long>(right.y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(right.x);
		return left_id < right_id;
	}
	bool operator!=(const Pos &right) const
	{
		long left_id = static_cast<long>(this->y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(this->x);
		long right_id = static_cast<long>(right.y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(right.x);
		return left_id != right_id;
	}
};

struct Size
{
	int width;
	int height;
};

struct StartingPieceInfo
{
	Pos pos;
	Size size;
};

struct Piece
{
	Pos pos;
	bool operator<(const Piece &right) const { return this->pos < right.pos; }
	bool operator!=(const Piece &right) const { return this->pos != right.pos; }
};
