#include "board.hpp"

Board::Board()
{
	tile = new Piece * [8];
	for (int x = 0; x < 8; ++x)
	{
		tile[x] = new Piece[8];
		for (int y = 0; y < 8; ++y)
		{
			if (y == 0 || y == 7)
			{
				tile[x][y].team = (y - 1) / 6;
				switch (x)
				{
				case 0:
				case 7:
					tile[x][y].type = ROOK;
					break;
				case 1:
				case 6:
					tile[x][y].type = KNIGHT;
					break;
				case 2:
				case 5:
					tile[x][y].type = BISHOP;
					break;
				case 3:
					tile[x][y].type = QUEEN;
					break;
				case 4:
					tile[x][y].type = KING;
					break;
				}
			}
			else if (y == 1 || y == 6)
			{
				tile[x][y].team = (y - 1) / 5;
				tile[x][y].type = PAWN;
			}
		}
	}
}

Board::Board(Board& copy)
{
	tile = new Piece * [8];
	for (int x = 0; x < 8; ++x)
	{
		tile[x] = new Piece[8];
		for (int y = 0; y < 8; ++y)
			set_tile(x, y, copy.get_tile(x, y));
	}
}

bool Board::is_tile_valid(int x, int y)
{
	return x >= 0 && x < 8 && y >= 0 && y < 8;
}

Piece Board::get_tile(int x, int y)
{
	return tile[x][y];
}

void Board::set_tile(int x, int y, Piece piece)
{
	tile[x][y] = piece;
}

void Board::move_piece(int old_x, int old_y, int new_x, int new_y)
{
	set_tile(new_x, new_y, get_tile(old_x, old_y));
	set_tile(old_x, old_y, EMPTY_TILE);
}
