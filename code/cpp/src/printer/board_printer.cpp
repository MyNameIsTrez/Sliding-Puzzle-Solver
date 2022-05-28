#include "board_printer.hpp"


void BoardPrinter::print_board(const pieces_t &pieces)
{
	const std::vector<std::vector<char>> board = get_board(pieces);

	for (std::vector<std::vector<char>>::const_iterator it_row = board.cbegin(); it_row != board.cend(); ++it_row)
	{
		for (std::vector<char>::const_iterator it_chr = it_row->cbegin(); it_chr != it_row->cend(); ++it_chr)
		{
			std::cout << *it_chr;
		}
		std::cout << std::endl;
	}
}


const std::vector<std::vector<char>> BoardPrinter::get_board(const pieces_t &pieces)
{
	std::vector<std::vector<char>> board = get_2d_vector();

	set_pieces_on_board(board, pieces);
	set_walls_on_board(board);

	return board;
}


const std::vector<std::vector<char>> BoardPrinter::get_2d_vector(void)
{
	return std::vector<std::vector<char>>(
		sps.height,
		std::vector<char>(sps.width, sps.empty_character)
	);
}


void BoardPrinter::set_pieces_on_board(std::vector<std::vector<char>> &board, const pieces_t &pieces)
{
	for (cell_id piece_index = 0; piece_index != sps.pieces_count; ++piece_index)
	{
		const Piece &piece = pieces[piece_index];

		const Pos &top_left = piece.top_left;
		const int top_left_x = top_left.x;
		const int top_left_y = top_left.y;

		const StartingPieceInfo &starting_piece_info = sps.starting_pieces_info[piece_index];
		const std::vector<Rect> &rects = starting_piece_info.rects;

		for (const auto &rect : rects)
		{
			const Offset &rect_offset = rect.offset;
			const int top_left_rect_x = top_left_x + rect_offset.x;
			const int top_left_rect_y = top_left_y + rect_offset.y;

			const Size &rect_size = rect.size;
			const int rect_width = rect_size.width;
			const int rect_height = rect_size.height;

			for (int y = top_left_rect_y; y < top_left_rect_y + rect_height; ++y)
			{
				for (int x = top_left_rect_x; x < top_left_rect_x + rect_width; ++x)
				{
					board[y][x] = sps.piece_labels[piece_index];
				}
			}
		}
	}
}


void BoardPrinter::set_walls_on_board(std::vector<std::vector<char>> &board)
{
	for (const auto &wall : sps.walls)
	{
		const Pos &wall_pos = wall.pos;
		const int top_left_wall_x = wall_pos.x;
		const int top_left_wall_y = wall_pos.y;

		const Size &wall_size = wall.size;

		for (int y = top_left_wall_y; y < top_left_wall_y + wall_size.height; ++y)
		{
			for (int x = top_left_wall_x; x < top_left_wall_x + wall_size.width; ++x)
			{
				board[y][x] = sps.wall_character;
			}
		}
	}
}
