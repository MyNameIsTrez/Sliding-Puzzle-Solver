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
	bool operator==(const Piece &other) const
	{
		return top_left == other.top_left;
	}
	struct Hasher {
		// Hashing logic stolen from here: https://stackoverflow.com/a/27216842
		size_t operator() (const std::vector<Piece> &pieces) const
		{
			std::size_t seed = pieces.size();

			for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
			{
				const Piece &piece = pieces[piece_index];
				const Pos &piece_top_left = piece.top_left;
				const int x = piece_top_left.x;
				const int y = piece_top_left.y;

				// TODO: Profile whether calculating and storing width instead is faster.
				// const int piece_index = x + y * width;

				seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= y + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};
};
