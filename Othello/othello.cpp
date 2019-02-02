#include "AI.h"
using namespace std;

// Global Variables
AI bob;

void doHumanMove()
{
	int row = -1;
	int col = -1;
	
	// Show the game state
	bob.game->displayBoard();

	cout << "Human Turn:" << endl
		<< "You are " << bob.game->HumanColor << endl;

	while (!bob.game->legalMove(row, col, bob.game->HumanColor))
	{
		cout << "Input Row: ";
		cin >> row;

		cout << "Input Column: ";
		cin >> col;
	}

	// Do the move and show the result
	bob.game->doMove(row, col);
	// bob.game->displayBoard();
	cout << "-----------------------------------------------------------" << endl;
}

int main()
{
	// Decide who goes first
	int first;
	cout <<
		"Who is to go first?" << endl <<
		"1 Human" << endl <<
		"2 Program" << endl <<
		"Please select a number: ";
	cin >> first;

	if (first == 1)
	{
		// Player goes first
		bob.game->HumanColor = 'X';
		bob.game->ProgramColor = 'O';
		
		doHumanMove();
	}
	else if (first == 2)
	{
		// Program goes first
		bob.game->HumanColor = 'O';
		bob.game->ProgramColor = 'X';
		bob.game->ProgramTurn = true;
	}

	while (!bob.game->isGameOver(bob.game->ProgramColor)) 
	{
		// Do program turn
		bob.populateChildren(bob.game, 0, -2000, 2000); 

		// check for game over again
		if (bob.game->isGameOver(bob.game->HumanColor))
			break;

		// Do human turn
		doHumanMove();
	}
	cout << "Game over." << endl;

	int playerScore = 0, programScore = 0;
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			if (bob.game->board[i][j] == bob.game->ProgramColor)
				programScore++;
			else if (bob.game->board[i][j] == bob.game->HumanColor)
				playerScore++;
	
	int endScore = bob.SEF(bob.game);
	if( endScore > 0)
		cout << "I win." << endl;
	else if (endScore == 0)
		cout << "It's a draw." << endl;
	else 
		cout << "I lose." << endl;

	cout << "Opponent: " << playerScore << " and Program: " << programScore << endl;

	bob.game->displayBoard();
	return 0;
}