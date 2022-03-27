#pragma once

////////

typedef struct Pos
{
	int x;
	int y;
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
} Piece;
