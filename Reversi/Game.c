#include <stdio.h>
#include "Direction.h"
#include  <conio.h>

#define DIRECTIONS 8
#define SIZE 10

#define RED_COLOR "\x1B[31m"
#define BLUE_COLOR "\x1B[34m"
#define RESET "\x1B[0m"

typedef enum
{
	RED,
	BLUE,
	EMPTY
} CellStatus, PlayerColor;

void PrintPlayerColor(CellStatus player);
void printBoard(CellStatus matrix[][SIZE]);


void getInput(int* row, int* col)
{
	// ask input from the user

	printf("\nPlease Press row number (1-8):");
	scanf("%d", row);


	printf("Please Press col number (1-8):");
	scanf("%d", col);
}

int isValid(int row, int col, CellStatus mat[SIZE][SIZE])
{
	// check if row and col are valid in the matrix and in empty Cell
	return row >= 1 && row < SIZE - 1 && col >= 1 && col < SIZE - 1 && mat[row][col] == EMPTY;
}

void initDirection(Direction arr[]) {

	// init direction matrix

	int i, j, count = 0;

	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			if (i != 0 || j != 0) {
				arr[count].row = i;
				arr[count].col = j;
				count++;
			}
		}
	}
}

void initBoard(CellStatus matrix[][SIZE]) {


	// init the borders Cells to empty cells

	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			matrix[i][j] = EMPTY;
		}
	}

	// make the start of the game state

	matrix[SIZE / 2][SIZE / 2] = BLUE;
	matrix[SIZE / 2][SIZE / 2 - 1] = RED;

	matrix[SIZE / 2 - 1][SIZE / 2] = RED;
	matrix[SIZE / 2 - 1][SIZE / 2 - 1] = BLUE;

}

void changePlayer(int* player)
{
	// change the player Playing 
	*player = ((*player) + 1) % 2;

}


int Play(int row, int col, PlayerColor player, CellStatus matrix[][SIZE], Direction dir, int numFlipped, int change)
{
	// the function try do find bridge in direction dir 
	// if found one return 1 and change the color of the othet player
	// else return 0
	// In addition: if change is 0, the function not flipp the other player color if find bridge only search the bridge
	//              if change is 1, the functio also flip the color, do actually the turn

	if (matrix[row][col] == EMPTY)
		return 0;

	if (matrix[row][col] == player)
	{
		// if we get to my player piece check if find a bridge by checking the number of flipped so far 
		if (numFlipped == 0)
			return 0;
		// we flipped ! find bridge !
		else return 1;
	}


	// we met other player piece

	// recursive call to next index in this direction 
	int nextIndex = Play(row + dir.row, col + dir.col, player, matrix, dir, numFlipped + 1, change);

	if (nextIndex && change)
		// flip this piece to my color
		matrix[row][col] = player;

	return nextIndex;
}

int canPlay(int row, int col, PlayerColor player, CellStatus matrix[][SIZE], Direction dir[], int change)
{

	// check if the player can do a turn in [row,col] index and do it if he can
	// return 1 if did a turn and 0 if not 

	int i;
	int validMove = 0;

	matrix[row][col] = player;

	// try to find bridge in all the directions 
	for (i = 0; i < DIRECTIONS; i++)
	{
		// call the play fnction with depends on change
		if (Play(row + dir[i].row, col + dir[i].col, player, matrix, dir[i], 0, change) > 0)
			validMove = 1;
	}

	if (!validMove)
	{
		matrix[row][col] = EMPTY;
		return 0;
	}

	return 1;


}

int fullBoard(CellStatus matrix[][SIZE])
{
	// check if the board is full

	int i, j;
	for (i = 1; i < SIZE - 1; i++)
	{
		for (j = 1; j < SIZE - 1; j++)
		{
			if (matrix[i][j] == EMPTY)
				return 0;
		}
	}
	return 1;
}

int checkWin(CellStatus matrix[][SIZE])
{
	// check who is the winner
	// return count of reds - count of blue
	int reds = 0, blues = 0;
	int row, col;
	for (row = 0; row < SIZE; row++)
	{
		for (col = 0; col < SIZE; col++)
		{
			switch (matrix[row][col])
			{
			case RED:
				reds++;
				break;
			case BLUE:
				blues++;
				break;
			}
		}
	}

	return reds - blues;
}

int haveMoves(PlayerColor player, CellStatus matrix[][SIZE], Direction dir[])
{	
	// return 1 if the player can do any move else return 0 

	int i;
	int row, col;

	// try to find bridge in the empty cells

	for (row = 1; row < SIZE - 1; row++)
	{
		for (col = 1; col < SIZE - 1; col++)
		{
			if (matrix[row][col] == EMPTY)
			{
				matrix[row][col] = player;

				for (i = 0; i < DIRECTIONS; i++)
				{
					// the last argument is 0 because we dont want the function will the turn only check if he valid
					if (Play(row + dir[i].row, col + dir[i].col, player, matrix, dir[i], 0, 0) > 0)
					{
						// find place that can cause bridge, return 1
						matrix[row][col] = EMPTY;
						return 1;

					}
				}
				matrix[row][col] = EMPTY;
			}
		}
	}

	return 0;


}




int main() {

	int attempets;

	int running = 1;
	int row, col;
	PlayerColor currentPlayer = RED;

	CellStatus mat[SIZE][SIZE];
	Direction directions[DIRECTIONS];




	initDirection(directions);

	initBoard(mat);
	printBoard(mat);


	// check if the board is full or the player has no more Valid Moves to do
	while (!fullBoard(mat) && haveMoves(currentPlayer, mat, directions))
	{
		printf("The turn of:");
		PrintPlayerColor(currentPlayer);
		printf("\n");

		attempets = 0;

		// get input from the user 
		getInput(&row, &col);

		// ask input and make turn until it is valid
		while (!isValid(row, col, mat) || !canPlay(row, col, currentPlayer, mat, directions, 1))
		{
			printf("The input is Unvalid Please Try Again :) \n");

			if (++attempets == 3)
			{
				printf("you three times dont succsees to give correct indexex, the turn is switching!\n");
				break;
			}
			else
				getInput(&row, &col);
		}

		// the turn was done 

		system("cls");
		printBoard(mat);
		changePlayer(&currentPlayer);
	}

	// check who is the winner and print it

	int res = checkWin(mat);

	if (res == 0)
	{
		printf("It's A tie!");
	}
	else
	{
		if (res > 0)
		{
			printf("RED is the Winner!");
		}
		else
			printf("Blue is the Winner!");
	}





	return 0;
}
void PrintPlayerColor(CellStatus player)
{
	switch (player)
	{
	case RED:
		printf(RED_COLOR " * " RESET);
		break;
	case BLUE:
		printf(BLUE_COLOR " * " RESET);
		break;
	default:
		printf("   ");
		break;
	}
}
void printBoard(CellStatus matrix[][SIZE])
{

	// this function print the board
	int i, j;


	for (i = 1; i < SIZE - 1; i++) {
		for (j = 1; j < SIZE - 1; j++)
			printf("----");
		printf("\n");

		for (j = 1; j < SIZE - 1; j++) {
			PrintPlayerColor(matrix[i][j]);
			printf("|");
		}
		printf("\n");
	}

	for (j = 1; j < SIZE - 1; j++)
		printf("----");
	printf("\n");
}
