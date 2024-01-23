#include "chess.hpp"

ControlMatrix::ControlMatrix()
{
	data = new bool* [8];
	for (int x = 0; x < 8; ++x)
	{
		data[x] = new bool[8];
		for (int y = 0; y < 8; ++y)
			data[x][y] = false;
	}
}

bool* ControlMatrix::operator[](int index)
{
	return data[index];
}

ControlMatrix ControlMatrix::operator+(ControlMatrix rhs)
{
	ControlMatrix new_matrix;
	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
			new_matrix.data[x][y] = data[x][y] || rhs.data[x][y];
	return new_matrix;
}

Chess::Chess()
{
	
}

void Chess::attempt_move(int old_x, int old_y, int new_x, int new_y)
{
	if (get_controlled_tiles(board, old_x, old_y)[new_x][new_y])
	{
		board.move_piece(old_x, old_y, new_x, new_y);
		end_turn();
	}
}

void Chess::end_turn()
{
	++turn;
	if (turn == 2)
	{
		turn = 0;
		++round;
	}

	if (!can_move(turn, board))
	{
		if (is_check(turn, board))
			std::cout << "CHECKMATE";
		else
			std::cout << "STALEMATE";
	}
}

bool Chess::is_check(int team, Board& piece_set)
{
	ControlMatrix opponent_control;
	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
			if (piece_set.get_tile(x, y).team == 1 - team)
				opponent_control = opponent_control + get_controlled_tiles(piece_set, x, y, true);
	for (int x = 0; x < 8; ++x)
		for (int y = 0; y < 8; ++y)
			if (opponent_control[x][y] && piece_set.get_tile(x, y).team == team && piece_set.get_tile(x, y).type == KING)
				return true;
	return false;
}

bool Chess::can_move(int team, Board& piece_set)
{
	for (int piece_x = 0; piece_x < 8; ++piece_x)
		for (int piece_y = 0; piece_y < 8; ++piece_y)
			if (piece_set.get_tile(piece_x, piece_y).team == team)
			{
				ControlMatrix valid_moves = get_controlled_tiles(piece_set, piece_x, piece_y);
				for (int move_x = 0; move_x < 8; ++move_x)
					for (int move_y = 0; move_y < 8; ++move_y)
						if (valid_moves[move_x][move_y])
							return true;
			}
	return false;
}

ControlMatrix Chess::get_controlled_tiles(Board& piece_set, int x, int y, bool allow_check)
{
	ControlMatrix matrix;
	Piece piece = piece_set.get_tile(x, y);

	const int knight_move_x[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
	const int knight_move_y[8] = { -1, -2, -2, -1, 1, 2, 2, 1 };

	switch (piece.type)
	{
		case PAWN:
			if (piece.team == 0)
			{
				matrix[x][3] = y == 1 && piece_set.get_tile(x, 2).type == -1 && piece_set.get_tile(x, 3).type == -1;
				matrix[x][y + 1] = piece_set.get_tile(x, y + 1).type == -1;
				if (x != 7)
					matrix[x + 1][y + 1] = piece_set.get_tile(x + 1, y + 1).team == 1;
				if (x != 0)
					matrix[x - 1][y + 1] = piece_set.get_tile(x - 1, y + 1).team == 1;
			}
			else
			{
				matrix[x][4] = y == 6 && piece_set.get_tile(x, 5).type == -1 && piece_set.get_tile(x, 4).type == -1;
				matrix[x][y - 1] = piece_set.get_tile(x, y - 1).type == -1;
				if (x != 7)
					matrix[x + 1][y - 1] = piece_set.get_tile(x + 1, y - 1).team == 0;
				if (x != 0)
				matrix[x - 1][y - 1] = piece_set.get_tile(x - 1, y - 1).team == 0;
			}
			break;
		case KNIGHT:
			for (int i = 0; i < 8; ++i)
				if (piece_set.is_tile_valid(x + knight_move_x[i], y + knight_move_y[i]))
					matrix[x + knight_move_x[i]][y + knight_move_y[i]] = piece_set.get_tile(x + knight_move_x[i], y + knight_move_y[i]).team != piece.team;
			break;
		case BISHOP:
			for (int x_direction = -1; x_direction < 2; x_direction += 2)
				for (int y_direction = -1; y_direction < 2; y_direction += 2)
					for (int distance = 1; piece_set.is_tile_valid(x + x_direction * distance, y + y_direction * distance); ++distance)
					{
						if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).type == -1)
							matrix[x + x_direction * distance][y + y_direction * distance] = true;
						else if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).team != piece.team)
						{
							matrix[x + x_direction * distance][y + y_direction * distance] = true;
							break;
						}
						else
							break;
					}
			break;
		case ROOK:
			for (int x_direction = -1; x_direction < 2; ++x_direction)
				for (int y_direction = -1; y_direction < 2; ++y_direction)
					if (y_direction == 0 || x_direction == 0)
						for (int distance = 1; piece_set.is_tile_valid(x + x_direction * distance, y + y_direction * distance); ++distance)
						{
							if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).type == -1)
								matrix[x + x_direction * distance][y + y_direction * distance] = true;
							else if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).team != piece.team)
							{
								matrix[x + x_direction * distance][y + y_direction * distance] = true;
								break;
							}
							else
								break;
						}
			matrix[x][y] = false;
			break;
		case QUEEN:
			for (int x_direction = -1; x_direction < 2; ++x_direction)
				for (int y_direction = -1; y_direction < 2; ++y_direction)
					for (int distance = 1; piece_set.is_tile_valid(x + x_direction * distance, y + y_direction * distance); ++distance)
					{
						if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).type == -1)
							matrix[x + x_direction * distance][y + y_direction * distance] = true;
						else if (piece_set.get_tile(x + x_direction * distance, y + y_direction * distance).team != piece.team)
						{
							matrix[x + x_direction * distance][y + y_direction * distance] = true;
							break;
						}
						else
							break;
					}
			matrix[x][y] = false;
			break;
		case KING:
			for (int x_direction = -1; x_direction < 2; ++x_direction)
				for (int y_direction = -1; y_direction < 2; ++y_direction)
					if (piece_set.get_tile(x + x_direction, y + y_direction).team != piece.team)
						matrix[x + x_direction][y + y_direction] = true;
			matrix[x][y] = false;
			break;
	}

	if (!allow_check)
		for (int new_x = 0; new_x < 8; ++new_x)
			for (int new_y = 0; new_y < 8; ++new_y)
				if (matrix[new_x][new_y])
				{
					Board test_move(piece_set);
					test_move.move_piece(x, y, new_x, new_y);
					matrix[new_x][new_y] = !is_check(piece.team, test_move);
				}

	return matrix;
}