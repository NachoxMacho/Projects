#include <iostream>
using namespace std;

const int boardSize = 8;

class Game
{
public:
	char board[boardSize][boardSize];

	bool DEBUG;

	Game & operator=(const Game& game);
	Game(const Game& game);
	Game(const Game *game);

	Game() { clearBoard(); DEBUG = false; }

	void displayBoard();
	void clearBoard();

	bool legalMove(int row, int col, char color);
	bool isGameOver(char color);
	void doMove(int row, int col);

	char HumanColor; // piece color for human
	char ProgramColor; // piece color for program
	bool ProgramTurn;

};

Game::Game(const Game& game) :ProgramColor(game.ProgramColor), ProgramTurn(game.ProgramTurn), HumanColor(game.HumanColor)
{
	Game board[boardSize][boardSize];
	this->clearBoard();

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			this->board[i][j] = game.board[i][j];
}

Game::Game(const Game *game) :ProgramColor(game->ProgramColor), ProgramTurn(game->ProgramTurn), HumanColor(game->HumanColor)
{
	Game board[boardSize][boardSize];
	this->clearBoard();

	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			this->board[i][j] = game->board[i][j];
}

void Game::displayBoard()
{
	cout << " |0|1|2|3|4|5|6|7|";

	for (int i = 0; i < boardSize; i++)
	{
		cout << endl << "-+-+-+-+-+-+-+-+-+" << endl << i << "|";

		for (int j = 0; j < boardSize; j++)
			cout << board[i][j] << "|";
	}
	cout << endl << "-+-+-+-+-+-+-+-+-+" << endl;
}

void Game::clearBoard()
{
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			board[i][j] = ' ';

	board[3][4] = 'X';
	board[3][3] = 'O';
	board[4][3] = 'X';
	board[4][4] = 'O';
}

bool Game::legalMove(int row, int col, char color)
{
	if (board[row][col] != ' ')
	{
		//cout << row << ", " << col << " is not an open space." << endl;
		return false;
	}

	if (row < 0 || row > 7 || col < 0 || col > 7)
	{
		//cout << row << ", " << col << " is out of bounds." << endl;
		return false;
	}

	char oppositeColor = 'X';
	if (oppositeColor == color)
		oppositeColor = 'O';

	// int dif = 1;
	
	if (board[row + 1][col] == oppositeColor)
		for (int i = 1; row + i < 7; i++)
			if (board[row + i][col] != ' ')
				if (board[row + i][col] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row - 1][col] == oppositeColor)
		for (int i = 1; row - i > -1; i++)
			if (board[row - i][col] != ' ')
				if (board[row - i][col] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row][col + 1] == oppositeColor)
		for (int i = 1; col + i < 7; i++)
			if (board[row][col + i] != ' ')
				if (board[row][col + i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row][col - 1] == oppositeColor)
		for (int i = 1; col - i > -1; i++)
			if (board[row][col - i] != ' ')
				if (board[row][col - i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row + 1][col + 1] == oppositeColor)
		for (int i = 1; row + i < 7 && col + i < 7; i++)
			if (board[row + i][col + i] != ' ')
				if (board[row + i][col + i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row - 1][col - 1] == oppositeColor)
		for (int i = 1; row - i > -1 && col - i > -1; i++)
			if (board[row - i][col - i] != ' ')
				if (board[row - i][col - i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row + 1][col - 1] == oppositeColor)
		for (int i = 1; row + i < 7 && col - i > -1; i++)
			if (board[row + i][col - i] != ' ')
				if (board[row + i][col - i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	if (board[row - 1][col + 1] == oppositeColor)
		for (int i = 1; row - i > -1 && col + i < 7; i++)
			if (board[row - i][col + i] != ' ')
				if (board[row - i][col + i] == color)
					return true;
				else 
					continue;
			else 
				break;
			
	return false;
}

bool Game::isGameOver(char color)
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (legalMove(i, j, color))
			{
				if (DEBUG)
					cout << i << ", " << j << " is a legal move for " << color << endl;
				return false;
			}
			//cout << i << ", " << j << " is not a legal move." << endl;
		}
	}

	// whoever's turn it is can't move
	return true;
}

void Game::doMove(int row, int col)
{
	// Get what color we are moving for
	char color = HumanColor;
	if (ProgramTurn)
		color = ProgramColor;

	// Get set the piece to the new value
	board[row][col] = color;

	// Swap whose turn it is 
	ProgramTurn = !ProgramTurn;

	bool skipN = false;
	bool skipE = false;
	bool skipS = false;
	bool skipW = false;
	bool skipSW = false;
	bool skipSE = false;
	bool skipNW = false;
	bool skipNE = false;

	char oppositeColor = 'X';
	if (oppositeColor == color)
		oppositeColor = 'O';

	int dif = 1;

	while (!(skipN && skipE && skipS && skipW &&
		skipSW && skipNW && skipSE && skipNE) && dif != 8)
	{
		// These are correct
		if (!skipN && board[row - dif][col] != oppositeColor)
			skipN = true;
		if (!skipE && board[row][col + dif] != oppositeColor)
			skipE = true;
		if (!skipS && board[row + dif][col] != oppositeColor)
			skipS = true;
		if (!skipW && board[row][col - dif] != oppositeColor)
			skipW = true;
		if (!skipNE && board[row - dif][col + dif] != oppositeColor)
			skipNE = true;
		if (!skipSE && board[row + dif][col + dif] != oppositeColor)
			skipSE = true;
		if (!skipNW && board[row - dif][col - dif] != oppositeColor)
			skipNW = true;
		if (!skipSW && board[row + dif][col - dif] != oppositeColor)
			skipSW = true;

		dif++;
		
		// These are correct
		if (row + dif > 7)
			skipS = skipSW = skipSE = true;
		if (row - dif < 0)
			skipN = skipNW = skipNE = true;
		if (col + dif > 7)
			skipE = skipNE = skipSE = true;
		if (col - dif < 0)
			skipW = skipSW = skipNW = true;

		if (!skipN)
			if (board[row - dif][col] == color)
			{
				for (int i = dif - 1; i > 0; i--)
					board[row - i][col] = color;
				skipN = true;
			}

		if (!skipE)
			if (board[row][col + dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row][col + i] = color;
				skipE = true;
			}

		if (!skipS)
			if (board[row + dif][col] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row + i][col] = color;
				skipS = true;
			}

		if (!skipW)
			if (board[row][col - dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row][col - i] = color;

				skipW = true;
			}

		if (!skipNE)
			if (board[row - dif][col + dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row - i][col + i] = color;
				skipNE = true;
			}

		if (!skipSE)
			if (board[row + dif][col + dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row + i][col + i] = color;
				skipSE = true;
			}

		if (!skipNW)
			if (board[row - dif][col - dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row - i][col - i] = color;
				skipNW = true;
			}

		if (!skipSW)
			if (board[row + dif][col - dif] == color)
			{
				for (int i = dif; i > 0; i--)
					board[row + i][col - i] = color;
				skipSW = true;
			}
	}
}

Game & Game::operator=(const Game & game)
{
	this->ProgramColor = game.ProgramColor;
	this->HumanColor = game.HumanColor;
	this->ProgramTurn = game.ProgramTurn;

	for (int i = 0; i < boardSize; i++)
	{
		for (int j = 0; j < boardSize; j++)
		{
			this->board[i][j] = game.board[i][j];
		}
	}

	return *this;
}
