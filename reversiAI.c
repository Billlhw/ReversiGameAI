#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection (char board[][26], int n, int row, int col,
							char colour, int deltaRow, int deltaCol);
bool checkLegalityOfPosition (char board[][26],int n, int row, int col, char colour);
void printBoard(char board[][26], int n);
void initializeBoard(char board[][26], int n);

void requestForMove(char board[][26],int n, char *colourPtr, int *rowPtr, int *colPtr);
void flipTiles(char board[][26],int n, char colour, int row, int col);
void computeMove(char board[][26],int n, char colour);
int computeScore(char board[][26],int n, int row, int col, char colour);
bool checkPositionAvailable(char board[][26],int n, char colour);
void checkWhoWon(char board[][26],int n, char computerColour, char humanColour);


int main(void)
{
	int boardSize = 0;
	char computerColour, humanColour, colourOfTurn = 'B';
	printf("Enter the board dimension: ");
	scanf("%d", &boardSize); //Request for board size
	printf("Computer plays (B/W): ");
	scanf(" %c",&computerColour);
	
	if (computerColour == 'B'){
		humanColour = 'W';
	} else {
		humanColour = 'B';
	} //Set the colour of human player to be opposite from computer's colour.
	
	char board[26][26]={{0}}; //Declares the 2D array board
	int row = 0, col = 0; //Initialize the variable containing user input
	//int *rowPtr = &row, *colPtr = &col;
	initializeBoard(board, boardSize);
	printBoard(board, boardSize);
	
	bool continuePlaying = true;
	while (continuePlaying){
		//char needCheckStatus;//If colour of turn is assigned as this, will bring to the case that
		//there might have been people won as no position is available
		if (colourOfTurn == computerColour){
		
			computeMove(board, boardSize, computerColour);
			printBoard(board,boardSize);
			
			if (checkPositionAvailable(board,boardSize,humanColour)){
				colourOfTurn = humanColour; //Switch turn to human player if human has a move available.
			} else if (checkPositionAvailable(board,boardSize,computerColour)){
				//Case that the human player has no move available but computer still has.
				colourOfTurn = computerColour;//Still computer's turn
			} else {//No one has move availble, game ends.
				checkWhoWon(board,boardSize, computerColour, humanColour);
				break;
			}
			
		} else { //Turn of the human player
			printf("Enter move for colour %c (RowCol): ", humanColour);
			char inputRow, inputCol;
			scanf(" %c%c", &inputRow, &inputCol);
			row = (int)(inputRow - 'a');
			col = (int)(inputCol - 'a'); //Request and define user input

			if (positionInBounds(boardSize, row, col) && checkLegalityOfPosition(board,boardSize,row,col,humanColour)){ //Case that the position entered by human player is valid
				flipTiles(board,boardSize,humanColour, row,col);
				printBoard(board,boardSize);
			} else{
				printf("Invalid move.\n");
				printf("%c player wins.\n", computerColour); //Case that the compter has won because human player inputted a invalid input
				break;
		}
		if (checkPositionAvailable(board, boardSize, computerColour)){
				colourOfTurn = computerColour;//Case that the computer has a position available and switch the turn to computer
			} else if (checkPositionAvailable(board,boardSize,humanColour)){
				//Case that the human player has no move available but computer still has.
				colourOfTurn = humanColour;//Still human player's turn
			} else { //No one has move available, so check who has won.
				checkWhoWon(board,boardSize, computerColour, humanColour);
				break;
			}
		}
	}
	return 0;
}

	

/*
 * Assign letters to the pieces on the board
 */
void initializeBoard(char board[][26], int n){
	int middleIndex = n/2-1;
	int rowInd,colInd;
	for (rowInd = 0; rowInd<n; rowInd++){
		for (colInd = 0; colInd<n;colInd++){
			if (rowInd == middleIndex && colInd == middleIndex){ //Initializes the center of the board to the configuration given
				board[rowInd][colInd] = 'W';
			} else if (rowInd == middleIndex +1 && colInd == middleIndex){
				board[rowInd][colInd] = 'B';
			} else if (rowInd == middleIndex && colInd == middleIndex + 1){
				board[rowInd][colInd] = 'B';
		} else if (rowInd == middleIndex +1 && colInd == middleIndex + 1){
				board[rowInd][colInd] = 'W';
		} else {
			board[rowInd][colInd] = 'U';
		}
		
	}
}
}


/*
 * Check if the position entered is within boundary
 */
bool positionInBounds(int n, int row, int col){
	if (row>=n || col>=n){
		return false;
	} else{
		return true;
	}
}

/*
 * The function that checks for the legality of a piece to be a candidate, given the direction checking
 */
bool checkLegalInDirection (char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol){
	int currentRow = row+deltaRow, currentCol = col+deltaCol;
	char oppoColour;
	if (colour == 'W'){ //Defines the char containing opponent's colour
		oppoColour = 'B';
	} else {
		oppoColour = 'W';
	}
	bool existOppoPieceBetween = false;
	while (currentRow<=n && currentCol<=n ){
	//Check the pieces in the indicated direction iteratively until reaches boundary or the two conditions for candidate piece are met,
	//namely at least one opponent's piece and one of the player's own piece exist in the indicated direction
		if (board[currentRow][currentCol] == oppoColour){ //Case that there is an opponent's piece in the specified direction
			existOppoPieceBetween = true;
			//printf("exists OppoPiece (%c) at: %d%d\n",oppoColour, currentRow,currentCol); //testing
			currentRow+=deltaRow;
			currentCol+=deltaCol; //Set the current index checking to be the next space
		} else if (board[currentRow][currentCol]==colour){
			if(existOppoPieceBetween){
				//printf("exists myPiece (%c) at: %d%d\n",colour, currentRow, currentCol);//testing
				return true; 
				//The following piece exists and there is at least one opponent's piece in between
			} else {
				return false; //A piece of the same colour is directly near it
			}
		} else { //Case that there is no piece on the indicated direction
			break; //Condition is not met since line of pieces must be continuous
		}
	}
			return false;
}

/*
 * Initiates the checking process of a position for legality of being a candidate
 * Calls the function checkLegalInDirection
 */
bool checkLegalityOfPosition (char board[][26],int n, int row, int col, char colour){
	if (board[row][col]=='B'||board[row][col]=='W'){//Will not proceed if this place has been occupied
		return false;
	}
	int rowIndex, colIndex;//Indices used for specifying the direction to check
	for (rowIndex = -1; rowIndex <=1; rowIndex++){
		for (colIndex = -1; colIndex <=1; colIndex++){
			if (!(rowIndex ==0 && colIndex == 0)){ //Do not proceed to check-in-direction when the direction in both ways are zero
				if (positionInBounds(n,row+rowIndex,col+colIndex)&&checkLegalInDirection(board, n, row, col, colour, rowIndex, colIndex))
					return true; //The conditions are met by the pieces on the current direction
			}
		}
	}
	return false; //Condition not met after checking all eight directions
}

/*
 * Prints the board, the board array and size of board are the parameter
 */
void printBoard (char board[][26], int n){

	printf("  ");//align the row indicators (a,b,c,d,etc.) with the chess board
	int indicatorIndex;
	for (indicatorIndex = 0; indicatorIndex < n; indicatorIndex++){ //Prints the row indicators on the top
		printf("%c",'a'+indicatorIndex);
	}
	printf("\n");
	
	int rowInd, colInd;
	for (rowInd = 0; rowInd < n; rowInd++){ //Prints the rest of the board
		printf("%c ",'a'+rowInd); //prints the column indicators
		for (colInd = 0; colInd < n; colInd++){
			printf("%c", board[rowInd][colInd]);
		}
		printf("\n");
	}
}

void requestForMove(char board[][26],int n, char *colourPtr, int *rowPtr, int *colPtr){
	printf("Enter a move:\n");
	char colour, row, col;
	scanf(" %c%c%c",&colour,&row,&col);//Extracts the colour, row, and col values from the string
	
		*colourPtr = colour;
		*rowPtr = row-'a';
		*colPtr = col-'a'; //Assign the values user inputed to the pointers
	
	if (positionInBounds(n,*rowPtr,*colPtr)&&checkLegalityOfPosition (board, n, (int)(row-'a'), (int)(col-'a'), colour)){
		//The move is valid as the piece is in bound and legal.
		printf("Valid move.\n");
	
	} else {
		printf("Invalid move.\n");
	}
	
}

/*
 * Row and col in the parameter is the location of the place user inputted, after verification
 * This function will flip the pieces caused by the piece placed. 
 */
void flipTiles(char board[][26],int n, char colour, int row, int col){
	int rowIndex, colIndex;//Indices used for specifying the direction to check
	
	board[row][col]=colour;
	for (rowIndex = -1; rowIndex <=1; rowIndex++){
		for (colIndex = -1; colIndex <=1; colIndex++){
			if (!(rowIndex ==0 && colIndex == 0)){ //Do not proceed to check-in-direction when the direction in both ways are zero
				if (checkLegalInDirection(board, n, row, col, colour, rowIndex, colIndex)){
					//The case that there tiles in this direction can be flipped
					int currentRow = row + rowIndex, currentCol = col+ colIndex;
					while (board[currentRow][currentCol]!=colour){
						board[currentRow][currentCol] = colour;//Set the pieces in between the given colour to the colour. 
						currentRow +=rowIndex;
						currentCol +=colIndex;
					}
				}
			}
		}
	}	
}

void computeMove(char board[][26],int n, char colour){

	int highScore = 0, highScoreRow = 0, highScoreCol = 0;
	int rowNum, colNum;
	
	for (rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
		for (colNum=0; colNum<n; colNum++){
			if (checkLegalityOfPosition (board, n, rowNum, colNum, colour)){
				int thisScore;
				thisScore = computeScore (board, n, rowNum, colNum, colour);
				//printf("%d",thisScore);
				if (thisScore > highScore){//Update the high score when a higher one is computed
					highScore = thisScore;
					highScoreRow = rowNum;
					highScoreCol = colNum;
				}
			}
		}
	}
	printf("Computer places %c at %c%c.\n",colour,(char)('a'+highScoreRow),(char)('a'+ highScoreCol));
	flipTiles(board,n,colour,highScoreRow,highScoreCol);
}

int computeScore(char board[][26],int n, int row, int col, char colour){
	int rowIndex, colIndex;//Indices used for specifying the direction to check
	int totalScore = 0;
	//printf("Computing Score!");
	for (rowIndex = -1; rowIndex <=1; rowIndex++){
		for (colIndex = -1; colIndex <=1; colIndex++){
			if (!(rowIndex ==0 && colIndex == 0)){ //Do not proceed to check-in-direction when the direction in both ways are zero
				if (checkLegalInDirection(board, n, row, col, colour, rowIndex, colIndex)){
					//The case that there tiles in this direction can be flipped
					int currentRow = row + rowIndex, currentCol = col+ colIndex;
					int thisScore = 0;
					while (board[currentRow][currentCol]!=colour){
						thisScore++;//Set the pieces in between the given colour to the colour. 
						currentRow += rowIndex;
						currentCol += colIndex;
					}
					totalScore += thisScore; //Adds the number flipped on the current direction
				}
			}
		}
	}	
	return totalScore;
}

bool checkPositionAvailable(char board[][26],int n, char colour){
	int rowInd, colInd;
	//Checks if move is available for a given colour
	for (rowInd = 0; rowInd<n;rowInd++){
		for (colInd=0;colInd<n;colInd++){
			if (checkLegalityOfPosition(board, n,rowInd, colInd, colour))
				return true;
		}
	}
	return false;
}

void checkWhoWon(char board[][26],int n, char computerColour, char humanColour){
	int computerTileNum = 0, humanTileNum = 0;
	int rowNum, colNum;
	for (rowNum = 0; rowNum < n; rowNum++){
		for (colNum = 0; colNum < n; colNum++){
			if (board[rowNum][colNum] == computerColour){
				computerTileNum++;
			} else if (board[rowNum][colNum] == humanColour){
				humanTileNum++;
			}		
		}
	}
	
	if (computerTileNum>humanTileNum){
		printf("%c player wins.\n",computerColour);
	} else if (computerTileNum<humanTileNum){
		printf("%c player wins.\n",humanColour);
	} else {
		printf("Draw!");
	}
}
