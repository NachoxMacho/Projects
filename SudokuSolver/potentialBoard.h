#include "board.h"

class potentialBoard
{
public:
    bool board[board_size][board_size][board_size + 1];

    // by default the values are all blank
    potentialBoard()
    {
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                for (int val = 1; val < board_size + 1 ; val++)
                {
                    board[row][col][val] = true;
                }
            }
        }
    }

    potentialBoard(potentialBoard *p)
    {
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                for (int val = 1; val < board_size + 1; val++)
                {
                    board[row][col][val] = p->board[row][col][val];
                }
            }
        }
    }

    // return the number of remaining possible values for row, col
    int count(int row, int col)
    {
        int count = 0;
        for (int i = 1; i < board_size + 1; i++)
            if (board[row][col][i])
                count++;

        return count;
    }

    // convert to real board
    sudoku_board convert_to_real()
    {
        sudoku_board output;

        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                if (count(row, col) == 1)
                {
                    for (int i = 1; i < board_size + 1; i++)
                        if (board[row][col][i])
                            output.board[row][col] = i;
                }
                else
                {
                    output.board[row][col] = 0;
                }
            }
        }
        return output;
    }

    // is valid
    bool is_valid()
    {
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                if (count(row, col) == 0)
                {
                    return false;
                }
            }
        }
        return this->convert_to_real().validate_board();
    }

    // solve position
    void solve_pos(int row, int col, int val)
    {
        // get the ninth
        int ninth = row / 3 * 3 + col / 3;
        int row_start = ninth / 3 * 3, col_start = ninth % 3 * 3; // get where the row & col starts

        // remove the number from the row and column
        for (int i = 0; i < board_size; i++)
        {
            board[i][col][val] = false, board[row][i][val] = false;
        }

        // remove all numbers from the designated space
        for (int i = 1; i < board_size + 1; i++)
        {
            board[row][col][i] = false;
        }

        // remove the number from the ninth
        for (int i = row_start; i < row_start + 3; i++)
        {
            for (int j = col_start; j < col_start + 3; j++)
            {
                board[i][j][val] = false;
            }
        }

        // set the value in the space
        board[row][col][val] = true;

        return;
    }

    // output the current board state
    void print(std::ostream &out_s)
    {
        for (int i = 0; i < board_size; i++)
        {
            for (int j = 0; j < board_size; j++)
            {
                for (int k = 1; k < board_size + 1; k++)
                {
                    if (board[i][j][k])
                        std::cout << k;
                    else
                        std::cout << " ";
                }
                std::cout << " | ";
            }
            std::cout << std::endl
                      << "-----------------------------------------------------------------------------------------------------------" << std::endl;
        }
    }

    // check if the potential board is completed
    // NOTE: this does note check numbers at this point, although this shouldn't be an issue
    bool is_solved()
    {
        for (int row = 0; row < board_size; row++)
        {
            for (int col = 0; col < board_size; col++)
            {
                if (count(row, col) != 1)
                    return false;
            }
        }

        return this->convert_to_real().validate_board();
    }

    void update_board()
    {
        for (int row = 0; row < board_size; row++)
            for (int col = 0; col < board_size; col++)
                if (count(row, col) == 1)
                    for (int val = 1; val < board_size + 1; val++)
                        if (board[row][col][val])
                            solve_pos(row, col, val);
    }
};