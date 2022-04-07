#pragma once

////////

struct Pos
{
	int x;
	int y;
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

struct EndingPiece
{
	std::size_t piece_index;
	Pos pos;
};

struct Piece
{
	Pos pos;
};
