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

struct Piece
{
	char label;
	Pos pos;
	Size size;
};
