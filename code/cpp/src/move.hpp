#pragma once


typedef int cell_id;
typedef int piece_direction;


struct MoveInfo
{
	cell_id index;
	piece_direction direction;
};

struct Move
{
	MoveInfo next;
	MoveInfo undo;
};
