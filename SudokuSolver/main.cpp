#include "solver.h"

#define filename "hard1.txt"
/*
	NINTHS LAYOUT:
	0|1|2
	-----
	3|4|5
	-----
	6|7|8
*/

// load the board from board.txt
void load_board(sudoku_board * sudoku)
{
	std::fstream file;
	file.open(filename);

	for (int i = 0; i < board_size; i++)
		for (int j = 0; j < board_size; j++)
			file >> sudoku->board[i][j];

	file.close();
}

int main()
{
	sudoku_board * game = new sudoku_board; // the base game
	sudoku_solver * solver = new sudoku_solver; // a test board
	solver->real = game; // give the solver the real board input
	load_board(game); // get the board from the input file
	game->print(std::cout); // outpu the intial state

	if (game->validate_board()) // verify the game board is valid to start
		std::cout << "Board is Valid" << std::endl;

	solver->fill_possibilities(); // fill out the matrix of possibilities

	// Use a Depth First search to solve the baord and output the result
	solver->df_solve(0, 0, 0, solver->possible)->convert_to_real().print(std::cout);

	return 0;
}