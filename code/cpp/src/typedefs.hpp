#pragma once


#include <vector>
#include <queue>


typedef int cell_id;
typedef int piece_direction;


typedef std::vector<std::vector<cell_id>> cells_t;

struct Piece;
typedef std::vector<Piece> pieces_t;

typedef std::queue<std::pair<pieces_t, cells_t>> pieces_queue_t;

typedef std::vector<std::pair<cell_id, piece_direction>> path_t;

typedef std::queue<path_t> path_queue_t;
