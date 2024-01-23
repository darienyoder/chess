#ifndef CHESS
#define CHESS

#include <iostream>
#include "board.hpp"

class ControlMatrix
{
public:
	ControlMatrix();
	bool* operator[](int index);
	ControlMatrix operator+(ControlMatrix rhs);
private:
	bool** data;
};

class Chess
{
public:

	Chess();
	
	void attempt_move(int old_x, int old_y, int new_x, int new_y);
	void end_turn();

	ControlMatrix get_controlled_tiles(Board& piece_set, int x, int y, bool allow_check = false);

	bool is_check(int team, Board& piece_set);
	bool can_move(int team, Board& piece_set);

	Board board;

	const int piece_value[6] = {1, 3, 3, 5, 9, 0};

	int round = 0;
	int turn = 0;
};

#endif