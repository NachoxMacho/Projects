#include <stdio.h>
#include <iostream>
#include <fstream>

#define board_size 9

class sudoku_board
{
public:
	// 9x9 
	int board[board_size][board_size];

	// CONSTRUCTOR
	sudoku_board()
	{
		for (int i = 0; i < board_size; i++)
			for (int j = 0; j < board_size; j++)
				board[i][j] = 0;
	}

	// PRINT
	// output the board state
	void print(std::ostream &out_s)
	{
		out_s << std::endl << "------------------" << std::endl;
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
				out_s << board[i][j] << "|";
			out_s << std::endl << "------------------" << std::endl;
		}
	}

	// validate the row (no repeated numbers)
	bool valid_row(int row)
	{
		bool numbers[board_size + 1];

		for (int i = 0; i < board_size + 1; i++)
			numbers[i] = false;

		for (int i = 0; i < board_size; i++)
		{
			if (board[row][i] != 0)
			{
				if (numbers[board[row][i]])
				{
					return false;
				}
				numbers[board[row][i]] = true;
			}
		}

		return true;
	}

	// validate the col (no repeated numbers)
	bool valid_col(int col)
	{
		bool numbers[board_size + 1];

		for (int i = 0; i < board_size + 1; i++)
			numbers[i] = false;

		for (int i = 0; i < board_size; i++)
		{
			if (board[i][col] != 0)
			{
				if (numbers[board[i][col]])
				{
					return false;
				}
				numbers[board[i][col]] = true;
			}
		}

		return true;
	}

	// validate a ninth (3x3 square) (no repeated numbers)
	bool valid_ninth(int ninth)
	{
		bool numbers[board_size + 1];

		for (int i = 0; i < board_size + 1; i++)
			numbers[i] = false;

		int row_start = ninth % 3 * 3;
		int col_start = ninth / 3 * 3;

		for (int i = row_start; i < row_start + 3; i++)
		{
			for (int j = col_start; j < col_start + 3; j++)
			{
				if (board[i][j] != 0)
				{
					if (numbers[board[i][j]])
					{
						return false;
					}
					numbers[board[i][j]] = true;
				}

			}
		}

		return true;
	}

	// validate the whole board
	bool validate_board()
	{
		for (int i = 0; i < board_size; i++)
		{
			if (!valid_row(i) || !valid_col(i) || !valid_ninth(i))
			{
				return false;
			}
		}
		return true;
	}
};