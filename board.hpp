#ifndef BOARD
#define BOARD

enum
{
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
};

struct Piece
{
	Piece()
	{
		team = -1;
		type = -1;
	}

	int team = -1, type = -1;
};

const Piece EMPTY_TILE;

class Board
{
public:
	Board();
	Board(Board&);

	bool is_tile_valid(int x, int y);

	Piece get_tile(int x, int y);
	void set_tile(int x, int y, Piece piece);

	void move_piece(int old_x, int old_y, int new_x, int new_y);

private:
	Piece** tile;
};

#endif