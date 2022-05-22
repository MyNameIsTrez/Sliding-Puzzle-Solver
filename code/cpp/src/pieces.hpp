#pragma once


struct Offset
{
	int x;
	int y;
};

struct Pos
{
	int x;
	int y;
	bool operator==(const Pos &other) const
	{
		return x == other.x && y == other.y;
	}
};

struct Size
{
	int width;
	int height;
};

struct Rect
{
	Offset offset;
	Size size;
};

struct StartingPieceInfo
{
	Pos top_left;
	std::vector<Rect> rects;
};

struct Wall
{
	Pos pos;
	Size size;
};

struct EndingPiece
{
	std::size_t piece_index;
	Pos top_left;
};

struct Piece
{
	Pos top_left;
	// bool operator==(const Piece &other) const
	// {
	// 	return top_left == other.top_left;
	// }
};
