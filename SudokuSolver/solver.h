#include "potentialBoard.h"

class sudoku_solver
{
public:
	sudoku_board *real;		  // the actual board
	potentialBoard *possible; // the list of potential values for each space

	// fill in the potential board will all possible numbers
	void fill_possibilities()
	{
		possible = new potentialBoard();

		// if the value is not 0 in the real board, need to fill it in in the potential board as a starting point
		for (int i = 0; i < board_size; i++)
			for (int j = 0; j < board_size; j++)
				if (real->board[i][j] != 0)
					possible->solve_pos(i, j, real->board[i][j]);
	}

	potentialBoard *df_solve(int row, int col, int val, potentialBoard *p)
	{
		// ending condition is if a board is solved or no longer valid

		// check if board is no longer valid
		if (!p->is_valid() || p->is_solved())
			return p;

		// set the starting values
		int r, c, v;
		r = row, c = col, v = val;

		// increment the starting position by 1 and verify that everything is still inbounds
		v++;
		if (v == board_size + 1)
			v = 1, c++;
		if (c == board_size)
			c = 0, r++;

		while (r < board_size) {

			if (p->count(r, c) != 1 && p->board[r][c][v])
			{
				potentialBoard *new_p = new potentialBoard(p);
				new_p->solve_pos(r, c, v);
				new_p->update_board();
				new_p = df_solve(r, c, v + 1, new_p); // after this, the board should only be returned if it is invalid or solved.

				if (new_p->is_solved())
					return new_p;
				else
				{
					p->board[r][c][v] = false;
					delete new_p;

					if (!p->is_valid()) 
						return p;
				}
			}

			// go to the next space and value
			v++;
			if (v == board_size + 1)
				v = 1, c++;
			if (c == board_size)
				c = 0, r++;
		}
		// should never get to here
		return p;
	}
};