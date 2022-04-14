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

struct Wall
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
	bool operator==(const Piece &other) const
	{
		return (pos.x == other.pos.x && pos.y == other.pos.y);
	}
	struct Hasher {
		// Hashing logic stolen from here: https://stackoverflow.com/a/27216842
		size_t operator() (const std::vector<Piece> &pieces) const
		{
			std::size_t seed = pieces.size();

			for (std::size_t piece_index = 0; piece_index != pieces.size(); ++piece_index)
			{
				const Piece &piece = pieces[piece_index];
				const Pos &piece_pos = piece.pos;
				const int x = piece_pos.x;
				const int y = piece_pos.y;

				// TODO: Profile whether calculating and storing width instead is faster.
				// const int piece_index = x + y * width;

				seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				seed ^= y + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			return seed;
		}
	};
};
