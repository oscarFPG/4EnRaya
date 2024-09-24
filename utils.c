#include "utils.h"

void showError(const char *msg){
	perror(msg);
	exit(0);
}


void printBoard (tBoard board, char* message){

	int i, row, cell;

		// Clear screen
		printf("\n%s\n", message);

		// Show column numbers
		printf ("  0   1   2   3   4   5   6  \n");


		// Draw each row
		for (row = (BOARD_HEIGHT - 1); row>=0; row--){

			// Current cell in the board
			cell = row * BOARD_WIDTH;

			// Separator line
			printf ("|---|---|---|---|---|---|---|\n");

			// Print a row of cells
			for (i=cell; i<(cell+BOARD_WIDTH); i++){
				printf("| %c ",board[i]);
			}

			// End of the row!
			printf ("|\n");
		}

		// Print the base
		printf ("|---------------------------|\n\n\n");

}
