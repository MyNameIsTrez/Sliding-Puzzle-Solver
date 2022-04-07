#pragma once

////////

struct Pos
{
	int x;
	int y;
	bool operator!=(const Pos &right) const
	{
		return x != right.x || y != right.y;
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
	bool operator!=(const Piece &right) const { return pos != right.pos; }
};
