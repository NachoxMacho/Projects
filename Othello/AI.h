#include "Board.h"
using namespace std;

class AI
{
public:
	// Constructor
	AI() { game = new Game; stableValue = 10; pieceBaseValue = 1; FlippableValue = -1; movesToLookAhead = 6; DEBUG = false; }

	// The game we are playing
	Game *game;

	// Weights for how the SEF evaluates boards
	int stableValue;
	int pieceBaseValue;
	int FlippableValue;

	// How many moves to predict
	int movesToLookAhead;

	// Output information to the console?
	bool DEBUG;

	// Functions for evaluating states
	int SEF(Game *state);
	bool canCapturePieceNow(Game state, int row, int col);
	bool stablePiece(Game state, int row, int col);

	// Functions that generate a tree of potential moves and compares those
	// These functions work ( Don't Touch without backup)
	int populateChildren(Game *state, int depth, int maxParent, int minParent);
};

int AI::SEF(Game *state)
{
	int value = 0;

	// check if the game is over
	if (state->isGameOver(((state->ProgramTurn)? state->ProgramColor : state->HumanColor )))
	{
		// add up the pieces on the board
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++)
			{
				// rest of the pieces < value we win OR rest of the pieces < -value we lose
				// There aren't enough spaces left to win
				if ((8 * (8 - i) + (8 - j)) < value || (8 * (8 - i) + (8 - j)) < -value)
					break;
				value += ((state->board[i][j] == game->ProgramColor) ? 1 : -1);
			}

		if (value > 0)
			return 1000;

		if (value < 0)
			return -1000;

		return 0;
	}
	
	// If there are more than 32 stable pieces for either player, we can consider the game over as those pieces wont be flipped
	int ProgramStablePieces = 0;
	int HumanStablePieces = 0;

	// scan the board
	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 8; j++)
		{
			if (state->board[i][j] != ' ')
			{
				// if the piece is stable, meaning it won't flip ever
				if (stablePiece(state, i, j))
					if (state->board[i][j] == game->ProgramColor)
						value += stableValue, ProgramStablePieces++;
					else
						value -= stableValue, HumanStablePieces++;
				else
					// otherwise if the opponent could flip it next turn
					if (canCapturePieceNow(state, i, j))
						if (state->board[i][j] == game->ProgramColor)
							value += FlippableValue;
						else
							value -= FlippableValue;
					// else if there is just a piece, no special case
					else if (state->board[i][j] == game->ProgramColor)
							value += pieceBaseValue;
						else
							value -= pieceBaseValue;
			}
					
			// We can't lose
			if (ProgramStablePieces > 32)
				return 1000;
			
			// We can't win
			else if (HumanStablePieces > 32)
				return -1000;
		}

	return value;
}

bool AI::canCapturePieceNow(Game state, int row, int col)
{
	// if there is no piece, it cant be captured
	if (state.board[row][col] == ' ')
		return false;

	// get the color we are checking
	char color = state.board[row][col];

	// set the opposite color
	char oppositeColor = 'X';
	if (oppositeColor == color)
		oppositeColor = 'O';

	bool skipN = false;
	bool skipE = false;
	bool skipNW = false;
	bool skipNE = false;

	// this is the radius we are checking in, will increase each loop
	int dif = 0;

	while (!(skipN && skipE && skipNW && skipNE) && ++dif != 8)
	{
		if (!skipN && row + dif < 7)
			if (state.board[row + dif][col] != color)
			{
				char hit = state.board[row + dif][col];
				// we hit a space or opponents piece
				for (int i = 1; row - i > 0; i++)
				{
					if (hit == oppositeColor ^ state.board[row - i][col] == oppositeColor)
						return true;
					
					// There are spaces on either side or both opponents pieces
					else if(state.board[row - i][col] != color)
						break;
				}
				skipN = true;
			}
			
		if (!skipE && col + dif  < 7)
			if (state.board[row][col + dif] != color)
			{
				char hit = state.board[row][col + dif];
				// we hit a space or opponents piece
				for (int i = 1; col - i > 0; i++)
				{
					if (hit == oppositeColor ^ state.board[row][col - i] == oppositeColor)
						return true;
					
					// There are spaces on either side
					else if(state.board[row][col - i] != color)
						break;
				}
				skipE = true;
			}
			
		if (!skipNE && row + dif < 7 && col + dif < 7)
			if (state.board[row + dif][col + dif] != color)
			{
				char hit = state.board[row + dif][col + dif];
				// we hit a space or opponents piece
				for (int i = 1; row - i > 0 && col - i > 0; i++)
				{
					if (hit == oppositeColor ^ state.board[row - i][col - i] == oppositeColor)
						return true;
					
					// There are spaces on either side
					else if(state.board[row - i][col - i] != color)
						break;
				}
				skipNE = true;
			}
			
		if (!skipNW && row + dif < 7 && col - dif > 0)
			if (state.board[row + dif][col - dif] != color)
			{
				char hit = state.board[row + dif][col - dif];
				// we hit a space or opponents piece
				for (int i = 1; row - i > 0 && col + i < 7; i++)
				{
					if (hit == oppositeColor ^ state.board[row - i][col + i] == oppositeColor)
						return true;
					
					// There are spaces on either side
					else if(state.board[row - i][col + i] != color)
						break;
				}
				skipNW = true;
			}
	}

	return false;
}

bool AI::stablePiece(Game state, int row, int col)
{
	// if there is no piece, it cant be stable
	if (state.board[row][col] == ' ')
		return false;

	// all corners are stable
	if ((row == 0 || row == 7) && (col == 0 || col == 7))
		return true;

	// get the color we are checking
	char color = state.board[row][col];

	// set the opposite color
	char oppositeColor = 'X';
	if (oppositeColor == color)
		oppositeColor = 'O';

	bool skipN = false;
	bool skipE = false;
	bool skipS = false;
	bool skipW = false;
	bool skipSW = false;
	bool skipSE = false;
	bool skipNW = false;
	bool skipNE = false;

	// this is the radius we are checking in, will increase each loop
	int dif = 0;

	while (!(skipN && skipE && skipS && skipW &&
		skipSW && skipNW && skipSE && skipNE) && ++dif != 8)
	{
		// if directions are out of bounds, skip those checks
		if ((col + dif) > 7)
			skipE = skipSW = skipSE = true;
		else if ((col - dif) < 0)
			skipW = skipNW = skipNE = true;
		if ((row + dif) > 7)
			skipN = skipNW = skipSW = true;
		else if ((row - dif) < 0)
			skipS = skipSE = skipNE = true;
		
		// Testing Half check
		if (!skipN)
			if(state.board[row - dif][col] != color)
			{
				// We hit a space or opponents piece
				for (int i = 1; row + i < 7; i++)
				{
					if (state.board[row + i][col] != color && // not our piece
					!(state.board[row - dif][col] == oppositeColor && state.board[row + i][col] == oppositeColor)) // not both opponents pieces
						// we hit a space on one side and opponents piece on the other side then this piece is not stable
						return false;
					
				}
				skipS = true;
				skipN = true;
			}
			
		if (!skipE)
			if(state.board[row][col + dif] != color)
			{
				// We hit a space or opponents piece
				for (int i = 1; col - i > 0; i++)
					if (state.board[row][col - i] != color && // not our piece
					!(state.board[row][col + dif] == oppositeColor && state.board[row][col - i] == oppositeColor))
						// we hit a space on one side and opponents piece on the other side
						return false;
					
				skipE = true;
				skipW = true;
			}
			
		if (!skipNE)
			if(state.board[row - dif][col + dif] != color)
			{
				// We hit a space or opponents piece
				for (int i = 1; row + i < 7 && col - i > 0; i++)
					if (state.board[row + i][col - i] != color && // not our piece
					!(state.board[row - dif][col + dif] == oppositeColor && state.board[row + i][col - i] == oppositeColor)) // not both opponents pieces
						// we hit a space on one side and opponents piece on the other side
						return false;
					
				skipNE = true;
				skipSW = true;
			}
			
		if (!skipNW)
			if(state.board[row - dif][col - dif] != color)
			{
				// We hit a space or opponents piece
				for (int i = 1; row + i < 7 && col + i < 7; i++)
					if (state.board[row + i][col + i] != color && // not our piece
					!(state.board[row - dif][col - dif] == oppositeColor && state.board[row + i][col + i] == oppositeColor)) // not both opponents pieces
						// we hit a space on one side and opponents piece on the other side
						return false;
					
				skipNW = true;
				skipSE = true;
			}
	}

	return true;
}

int AI::populateChildren(Game *state, int depth, int maxParent, int minParent)
{
	// Get what player we are simulating for 
	char color = (state->ProgramTurn) ? state->ProgramColor : state->HumanColor;

	// If game over
	if (state->isGameOver(color))
		return SEF(state);
	
	// If we are at a leaf
	if (depth >= movesToLookAhead)
	{
		int legalMovesLeft = 0;
		
		for (int i = 0; i < boardSize; i++)
			for (int j = 0; j < boardSize; j++)
				if (state->legalMove(i, j, color))
					legalMovesLeft++;
				
		// If there are more then 2 moves left, may not be a game over soon
		if(legalMovesLeft > 2)
			return SEF(state);
	}

	bool max = state->ProgramTurn;

	Game * bestGame = new Game(state);
	int bestR, bestC;
	bestR = bestC = -1;

	// Generate a leaf
	for (int i = 0; i < boardSize; i++)
		for (int j = 0; j < boardSize; j++)
			if (state->legalMove(i, j, color))
			{
				// if a leaf is found

				// make a copy of the game with that move
				Game * newGame = new Game(state);
				newGame->doMove(i, j);
				int score;

				// calculate the potential score of that move
				if (max) // Maximizing
					score = populateChildren(newGame, depth + 1, maxParent, minParent);
				else // Minimizing
					score = populateChildren(newGame, depth + 1, maxParent, minParent);
				
				if (max && score > maxParent)
				{
					// Maximizing
					// Found a new better score
					maxParent = score;
					bestGame = newGame;
					bestR = i;
					bestC = j;
					
					// Test if the values have crossed
					// This means that the node above us will not accept any other boards as they would have to be worse for that node then it would accept
					if (maxParent >= minParent)
						return minParent;
				}
				else if (!max && score < minParent)
				{
					// Minimizing
					// Found a new better score
					minParent = score;
					bestGame = newGame;
					bestR = i;
					bestC = j;
					
					// Test if the values have crossed
					// This means that the node above us will not accept any other boards as they would have to be worse for that node then it would accept
					if (maxParent >= minParent)
						return maxParent;
					
				} 
			}

	if (depth == 0)
	{
		game = bestGame;
		cout << "Score: " << maxParent << endl;
		cout << "I will place a piece at " << bestR << ", " << bestC << endl;
	}

	return ((max)? maxParent: minParent);
}