#pragma once

////////

#include <limits> // std::numeric_limits<int>::max()

////////

typedef struct Pos
{
	int x;
	int y;
	bool operator<(const Pos &right) const
	{
		long left_id = static_cast<long>(this->y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(this->x);
		long right_id = static_cast<long>(right.y) * static_cast<long>(std::numeric_limits<int>::max()) + static_cast<long>(right.x);
		return left_id < right_id;
	}
} Pos;

typedef struct Size
{
	int width;
	int height;
} Size;

typedef struct StartingPiece
{
	Pos pos;
	Size size;
} StartingPiece;

typedef struct EndingPiece
{
	Pos pos;
} EndingPiece;

typedef struct Piece
{
	Pos pos;
	bool operator<(const Piece &right) const { return pos < right.pos; }
} Piece;
