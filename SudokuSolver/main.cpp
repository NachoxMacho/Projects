#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cassert>
using namespace std;

#define board_size 9
#define filename "board.txt"

#define convert_col(x) x % 3
#define convert_row(x) x / 3
#define convert_ninth(row, col) row / 3 * 3 + col / 3

/*
	NINTHS LAYOUT:
	0|1|2
	-----
	3|4|5
	-----
	6|7|8
*/


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
	void print(ostream &out_s)
	{
		out_s << endl << "------------------" << endl;
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
				out_s << board[i][j] << "|";
			out_s << endl << "------------------" << endl;
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
					cout << "Pos " << row << ", " << i << "isn't valid with value " << board[row][i] << endl;
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
					cout << "Pos " << i << ", " << col << "isn't valid with value " << board[i][col] << endl;
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
		assert(ninth > -1 && ninth < 9);
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
						cout << "Pos " << i << ", " << j << "is NOT valid with value " << board[i][j] << endl;
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
			if (!valid_row(i))
			{
				cout << "Row " << i << "isn't valid" << endl;
				return false;
			}
			if (!valid_col(i))
			{
				cout << "Col " << i << "isn't valid" << endl;
				return false;
			}
			if (!valid_ninth(i))
			{
				cout << "Ninth " << i << "isn't valid" << endl;
				return false;
			}
		}
		return true;
	}
};

// manually input the board
void input_board(sudoku_board * sudoku)
{
	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
			std::cin >> sudoku->board[i][j];
}

// load the board from board.txt
void load_board(sudoku_board * sudoku)
{
	fstream file;
	file.open(filename);

	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
			file >> sudoku->board[i][j];

	file.close();
}

class sudoku_solver
{
public:
	sudoku_board * game;

	bool possiblities[board_size][board_size][board_size + 1];

	int possible_count(int row, int col, bool pos[board_size][board_size][board_size + 1])
	{
		int count = 0;
		for (int i = 1; i < board_size + 1; i++)
		{
			if (pos[row][col][i])
				count++;
		}
		return count;
	}

	int possible_count(int row, int col) { return possible_count(row, col, possiblities); }

	bool is_solved(bool pos[board_size][board_size][board_size + 1])
	{
		if (!is_valid(pos))
			return false;

		for (int row = 0; row < board_size; row++)
			for (int col = 0; col < board_size; col++)
				if (possible_count(row, col, pos) != 1)
					return false;

		return true;
	}

	void debug(int row, int col, int val)
	{
		cout << "Count: " << possible_count(row, col) << "| Row: " << row << "| Col: " << col << "| VAL: " << val << endl;
		for (int i = 1; i < board_size + 1; i++)
		{
			if (possiblities[row][col][i])
				cout << i;
			else
				cout << "x";
		}
		cout << endl;

		game->print(cout);
		print(cout);
	}

	bool single_pos_check(int row, int col)
	{

		if (game->board[row][col] != 0)
			return false;
		int count = possible_count(row, col);

		/*if (count != 1)
			return false;*/

		int solution;
		for (int i = 1; i < board_size + 1; i++)
			if (possiblities[row][col][i])
				solution = i;

		if (count <= 0)
			debug( row, col, solution);

		assert(count > 0);

		if (count == 1)
		{
			solve_pos(row, col, solution);
			cout << "Solved: " << game->board[row][col] << " | Pos: " << row << ", " << col << endl;
			return true;
		}
		return false;
	}

	void solve_pos(int row, int col, int val)
	{
		bool debugging = false;

		int ninth = convert_ninth(row, col);
		int row_start = ninth / 3 * 3, col_start = ninth % 3 * 3;

		if (debugging)
			cout << "Ninth: " << ninth << " | Row: " << row_start << " | Col: " << col_start << endl;

		for (int i = 0; i < board_size; i++)
			possiblities[i][col][val] = false, possiblities[row][i][val] = false;
		possiblities[row][col][val] = true;

		if (!valid_board())
			debug(row, col, val);

		assert(valid_board());

		for (int i = 1; i < board_size + 1; i++)
			possiblities[row][col][i] = false;

		for (int i = row_start; i < row_start + 3; i++)
			for (int j = col_start; j < col_start + 3; j++)
				if (game->board[i][j] == 0 && possiblities[i][j][val])
					possiblities[i][j][val] = false;

		possiblities[row][col][val] = true;

		if(!valid_board())
			debug(row, col, val);
		assert(valid_board());

		
		game->board[row][col] = val;
	}

	void solve_pos(int row, int col, int val, bool pos[board_size][board_size][board_size + 1])
	{
		bool debugging = false;

		int ninth = convert_ninth(row, col);
		int row_start = ninth / 3 * 3, col_start = ninth % 3 * 3;

		if (debugging)
			cout << "Ninth: " << ninth << " | Row: " << row_start << " | Col: " << col_start << endl;

		for (int i = 0; i < board_size; i++)
			pos[i][col][val] = false, pos[row][i][val] = false;

		for (int i = 1; i < board_size + 1; i++)
			pos[row][col][i] = false;

		for (int i = row_start; i < row_start + 3; i++)
			for (int j = col_start; j < col_start + 3; j++)
				if (possible_count(row, col, pos) > 1 && pos[i][j][val])
					pos[i][j][val] = false;

		pos[row][col][val] = true;
		// game->board[row][col] = val;
	}

	void fill_possibilities()
	{
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				for (int k = 1; k < board_size + 1; k++)
				{
					possiblities[i][j][k] = true;
				}
			}
		}

		

		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{

				if (game->board[i][j] != 0)
				{
					cout << "Solved: " << game->board[i][j] << "| Pos: " << i << ", " << j << "| Ninth: " << convert_ninth(i, j) << endl;
					solve_pos(i, j, game->board[i][j]);
				}
			}
		}

		for (int row = 0; row < board_size; row++)
		{
			for (int col = 0; col < board_size; col++)
			{
				bool * numbers = possiblities[row][col];
				for (int i = 0; i < board_size; i++)
				{
					if (!numbers[game->board[row][i]] && game->board[row][i] != 0)
						numbers[game->board[row][i]] = false;

					if (!numbers[game->board[i][col]] && game->board[i][col] != 0)
						numbers[game->board[i][col]] = false;
				}
			}
		}
	}

	bool random_solve(bool pos[board_size][board_size][board_size + 1])
	{
		if (!is_valid(pos))
			return false;

		for (int row = 0; row < board_size; row++)
		{
			for (int col = 0; col < board_size; col++)
			{
				if (possible_count(row, col, pos) > 1)
				{
					cout << "Found a potential cell." << endl;
					for (int i = 1; i < board_size + 1; i++)
					{
						if (pos[row][col][i])
						{
							cout << "Found a potential solution of " << i << " in cell " << row << ", " << col << endl;
							if (random_solve(row, col, i, pos))
							{
								solve_pos(row, col, i, pos);
								break;
							}
							else
							{
								cout << "Solution of " << i << " in cell " << row << ", " << col << " is NOT correct."<< endl;
								pos[row][col][i] = false;
							}
						}
					}
				}
			}
		}
	}

	bool random_solve(int row, int col, int val, bool pos[board_size][board_size][board_size + 1])
	{
		bool p[board_size][board_size][board_size + 1];
		for (int row = 0; row < board_size; row++)
		{
			for (int col = 0; col < board_size; col++)
			{
				for (int i = 1; i < board_size + 1; i++)
				{
					p[row][col][i] = pos[row][col][i];
				}
			}
		}

		solve_pos(row, col, val, p);

		if (!is_valid(p))
			return false;

		if (is_solved(p))
		{

			return true;
		}

		for (int row = 0; row < board_size; row++)
		{
			for (int col = 0; col < board_size; col++)
			{
				if (possible_count(row, col, pos) > 1)
				{
					cout << "Found a potential cell." << endl;
					for (int i = 1; i < board_size + 1; i++)
					{
						if (pos[row][col][i])
						{
							cout << "Found a potential solution of " << i << " in cell " << row << ", " << col << endl;
							print(cout, pos);
							if (random_solve(row, col, i, pos))
							{
								solve_pos(row, col, i, pos);
								break;
							}
							else
							{
								cout << "Solution of " << i << " in cell " << row << ", " << col << " is NOT correct." << endl;
								pos[row][col][i] = false;
							}
						}
					}
				}
			}
		}
	}

	void print(ostream & out_s)
	{
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				for (int k = 1; k < board_size + 1; k++)
				{
					if (possiblities[i][j][k])
						cout << k;
					else
						cout << " ";
				}
				cout << " | ";
			}
			cout << endl << "-----------------------------------------------------------------------------------------------------------" << endl;
		}
	}

	void print(ostream & out_s, bool pos[board_size][board_size][board_size + 1])
	{
		for (int i = 0; i < board_size; i++)
		{
			for (int j = 0; j < board_size; j++)
			{
				for (int k = 1; k < board_size + 1; k++)
				{
					if (pos[i][j][k])
						cout << k;
					else
						cout << " ";
				}
				cout << " | ";
			}
			cout << endl << "-----------------------------------------------------------------------------------------------------------" << endl;
		}
	}

	bool is_valid(bool pos[board_size][board_size][board_size + 1])
	{
		for (int row = 0; row < board_size; row++)
			for (int col = 0; col < board_size; col++)
			{
				bool safe = false;
				for (int i = 1; i < board_size + 1; i++)
				{
					if (pos[row][col][i])
					{
						safe = true;
						break;
					}
				}

				if (!safe)
					return false;
			}

		return true;
	}
	
	// validate the board by making sure each space has at least one possible number
	bool valid_board()
	{
		for (int row = 0; row < board_size; row++)
			for (int col = 0; col < board_size; col++)
			{
				bool safe = false;
				for (int i = 1; i < board_size + 1; i++)
				{
					if (possiblities[row][col][i])
					{
						safe = true;
						break;
					}
				}
				
				if (!safe)
					return false;
			}

		return true;
	}
};

int main()
{
	sudoku_board * game = new sudoku_board;
	sudoku_solver * solver = new sudoku_solver;
	solver->game = game;
	load_board(game);
	game->print(cout);

	if (game->validate_board())
		cout << "Board is Valid" << endl;

	solver->fill_possibilities();
	solver->print(cout);

	solver->random_solve(solver->possiblities);

	//bool stop = true;
	//while (stop)
	//{
	//	stop = false;

	//	for (int row = 0; row < board_size; row++)
	//	{
	//		for (int col = 0; col < board_size; col++)
	//		{
	//			if (solver->single_pos_check(row, col))
	//				stop = true;
	//		}
	//	}
	//}
	solver->print(cout);
	game->print(cout);


}