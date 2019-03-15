#include <stdio.h>
#include <stdbool.h>
#include<stdlib.h>
#include <string.h>

bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection (char board[][26], int n, int row, int col,
							char colour, int deltaRow, int deltaCol);
bool checkLegalityOfPosition (char board[][26],int n, int row, int col, char colour);
void printBoard(char board[][26], int n);
void initializeBoard(char board[][26], int n);

void requestForMove(char board[][26],int n, char *colourPtr, int *rowPtr, int *colPtr);
void flipTiles(char board[][26],int n, char colour, int row, int col);
void makeMove(char board[][26],int n, char colour, char oppoColour);
int computeScore(char board[][26],int n, int row, int col, char colour);
bool checkPositionAvailable(char board[][26],int n, char colour);
void checkWhoWon(char board[][26],int n, char computerColour, char humanColour);
int computeActionNum (char board[][26],int n, char colour, char oppoColour);
int evalPiecesValue (char board[][26], int n, char colour, char oppoColour);

void copyBoard(char board1[][26], int n, char board2[][26]);
void makeMove2 (char board[][26],int n, int stageNum, char colour, char oppoColour);
void makeMoveHelper(char board[][26],int n, int stageNum, char colour, char oppoColour, int bestRow, int bestCol);


int main(void)
{
	int boardSize = 0;
	char computerColour, humanColour, colourOfTurn = 'B';
	printf("This is Part II tester. Enter the board dimension: ");
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
			//testing: printf("Num of Available moves for computer is: %d\n", computeActionNum(board,boardSize, computerColour, humanColour));
			makeMove(board, boardSize, computerColour, humanColour);
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
			
		} else { //Turn of the other computer
			//testing: printf("Num of Available moves for computer is: %d\n", computeActionNum(board,boardSize, computerColour, humanColour));
			makeMove2(board, boardSize, 9, humanColour, computerColour);
			printBoard(board,boardSize);
			
			if (checkPositionAvailable(board,boardSize,computerColour)){
				colourOfTurn = computerColour; //Switch turn to computerColour player 1 if human has a move available.
			} else if (checkPositionAvailable(board,boardSize,humanColour)){
				//Case that the human player has no move available but computer 2 still has.
				colourOfTurn = computerColour;//Still computer 2's turn
			} else {//No one has move availble, game ends.
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
				//printf("Position %d%d is available for colour %c\n",row,col, colour);
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

void makeMove(char board[][26],int n, char colour, char oppoColour){

	int highScore = 0, bestRow = 0, bestCol = 0;
	int rowNum, colNum;
	int count = 0;
	//int a = evalPiecesValue ( board,  n,  colour, oppoColour);
	for (rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
		for (colNum=0; colNum<n; colNum++){
			if (checkLegalityOfPosition (board, n, rowNum, colNum, colour)){
				char stageOneBoard[26][26]={{0}};
				copyBoard(board, n, stageOneBoard);
				flipTiles(stageOneBoard,n,colour, rowNum, colNum);
				//printBoard(stageOneBoard,n);
				int thisScore=(computeActionNum(stageOneBoard, n, colour, oppoColour)*100+evalPiecesValue(stageOneBoard, n, colour, oppoColour));
				
				/*
				 * 
				 * 
				 * Need to add deeper
				 * 
				 */
				//printf("Score at %d%d: %d\n",rowNum,colNum,thisScore);
				if (count ==0){ //Set the first score to the highest score
					highScore = thisScore;
					bestRow = rowNum;
					bestCol = colNum;
					count++;
					
				}
				else if (thisScore > highScore){//Update the high score when a higher one is computed
					highScore = thisScore;
					bestRow = rowNum;
					bestCol = colNum;
				}else{
					
				}
			}
				
				
				/*int thisScore;
				thisScore = computeScore (board, n, rowNum, colNum, colour);
				//printf("%d",thisScore);
				if (thisScore > highScore){//Update the high score when a higher one is computed
					highScore = thisScore;
					highScoreRow = rowNum;
					highScoreCol = colNum;
				}*/
			}
		}
	
	printf("Computer places %c at %c%c.\n",colour,(char)('a'+bestRow),(char)('a'+ bestCol));
	flipTiles(board,n,colour,bestRow,bestCol);
	
}

char **increasedStage(char initialBoardStage[][26], int n, char colour, char oppoColour){
	char finalBoardStage[26][26]={{0}};//Initialize a character array containing output.
	copyBoard(initialBoardStage, n, finalBoardStage);
	
	int rowNum, colNum;
	for (rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
		for (colNum=0; colNum<n; colNum++){
			if (checkLegalityOfPosition (finalBoardStage, n, rowNum, colNum, colour)){
				//flipTiles(finalBoardStage, n, colour,rowNum, colNum);
			}
		}
	}
	return finalBoardStage;
}


/*
 * Iterates through the board to count the number of moves available
 * 
 */
int computeActionNum (char board[][26],int n, char colour, char oppoColour){
	int rowIndex, colIndex;
	int actionNum = 0;
	for (rowIndex = 0; rowIndex < n; rowIndex++){
		for (colIndex = 0; colIndex < n; colIndex++){ //Iterates through the board
			
				if (checkLegalityOfPosition(board, n, rowIndex,colIndex, colour)){
						actionNum++;
				}
		}
	}
	return actionNum;	
}

int evalPiecesValue (char board[][26], int n, char colour, char oppoColour){
	/*int **valueArray = (int**)malloc (sizeof(int*)*n); //create a 2D array of size of board dynamically to contain the value of pieces on each position.  
		int initalizeIdx;
		for (initalizeIdx=0;initalizeIdx<n;initalizeIdx++){
			valueArray[initalizeIdx]= (int*)malloc (sizeof(int)*n); //initialize each row of the array
		}
		
		free(valueArray);//Avoid overflow
	*/
		
		int valueArray[26][26]={0};
		int totalPieceValue = 0;
		int rowIdx, colIdx;
		
		for (rowIdx=0; rowIdx<n; rowIdx++){ //Assign this array to their respective values
			for (colIdx=0; colIdx<n; colIdx++){
				if ((rowIdx==1&&colIdx==1)||(rowIdx==n-2&&colIdx==n-2)||(rowIdx==1&&colIdx==n-2)||(rowIdx==n-2&&colIdx==1)){
					valueArray[rowIdx][colIdx]= -70; //These are the corner pieces inside the broadest corner pieces
				} else if ((rowIdx==0&&colIdx==0)||(rowIdx==n-1&&colIdx==n-1)||(rowIdx==0&&colIdx==n-1)||(rowIdx==n-1&&colIdx==0)){
					valueArray[rowIdx][colIdx]= 100; //These are the corner pieces, most valuable because these are impossible to be flipped
				}else if ((rowIdx==0&&colIdx==1)||(rowIdx==0&&colIdx==n-2)||(rowIdx==n-1&&colIdx==1)||(rowIdx==n-1&&colIdx==n-2)){
					valueArray[rowIdx][colIdx]= -60; //These are the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if ((rowIdx==1&&colIdx==0)||(rowIdx==1&&colIdx==n-1)||(rowIdx==n-2&&colIdx==0)||(rowIdx==n-2&&colIdx==n-1)){
					valueArray[rowIdx][colIdx]= -60; //These are also the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if (rowIdx==0||colIdx==0||rowIdx==n-1||colIdx==n-1){
					valueArray[rowIdx][colIdx]= 15;//These are the edge pieces, stable because it has limited freedom for flipping 
				}else if (rowIdx==1||colIdx==1||rowIdx==n-2||colIdx==n-2){
					valueArray[rowIdx][colIdx]= 6;//These are the pieces inside the edge piece, stable to a limited extend.
				}else{
					valueArray[rowIdx][colIdx]= 1;//These are the rest of the pieces in the center.
				}
		
			} 
		}
		
		for (rowIdx=0; rowIdx<n; rowIdx++){
			for (colIdx=0; colIdx<n; colIdx++){
				if (board[rowIdx][colIdx]==colour){
					totalPieceValue+=valueArray[rowIdx][colIdx];
				} else if (board[rowIdx][colIdx]==oppoColour){
					totalPieceValue-=valueArray[rowIdx][colIdx];
				}
			}
		}
		
		/*for (int i=0; i<n;i++){
			for (int j=0; j<n; j++){
				printf("%3d ",valueArray[i][j]);
			}
			printf("\n");
		}*/
	
	return totalPieceValue;
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

void copyBoard(char board1[][26], int n, char board2[][26]){
	int row, col;
	for (row=0; row<n; row++){
		for (col=0; col<n; col++){
			board2[row][col]=board1[row][col]; //Assign each value in the second board to be the same value as on the first board.
		}
	}
	
}

int evalPiecesValue2 (char board[][26], int n, char colour, char oppoColour){
	/*int **valueArray = (int**)malloc (sizeof(int*)*n); //create a 2D array of size of board dynamically to contain the value of pieces on each position.  
		int initalizeIdx;
		for (initalizeIdx=0;initalizeIdx<n;initalizeIdx++){
			valueArray[initalizeIdx]= (int*)malloc (sizeof(int)*n); //initialize each row of the array
		}
		
		free(valueArray);//Avoid overflow
	*/
		
		int valueArray[26][26]={0};
		int totalPieceValue = 0;
		int rowIdx, colIdx;
		
		for (rowIdx=0; rowIdx<n; rowIdx++){ //Assign this array to their respective values
			for (colIdx=0; colIdx<n; colIdx++){
				if ((rowIdx==1&&colIdx==1)||(rowIdx==n-2&&colIdx==n-2)||(rowIdx==1&&colIdx==n-2)||(rowIdx==n-2&&colIdx==1)){
					valueArray[rowIdx][colIdx]= -70; //These are the corner pieces inside the broadest corner pieces
				} else if ((rowIdx==0&&colIdx==0)||(rowIdx==n-1&&colIdx==n-1)||(rowIdx==0&&colIdx==n-1)||(rowIdx==n-1&&colIdx==0)){
					valueArray[rowIdx][colIdx]= 100; //These are the corner pieces, most valuable because these are impossible to be flipped
				}else if ((rowIdx==0&&colIdx==1)||(rowIdx==0&&colIdx==n-2)||(rowIdx==n-1&&colIdx==1)||(rowIdx==n-1&&colIdx==n-2)){
					valueArray[rowIdx][colIdx]= -60; //These are the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if ((rowIdx==1&&colIdx==0)||(rowIdx==1&&colIdx==n-1)||(rowIdx==n-2&&colIdx==0)||(rowIdx==n-2&&colIdx==n-1)){
					valueArray[rowIdx][colIdx]= -60; //These are also the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if (rowIdx==0||colIdx==0||rowIdx==n-1||colIdx==n-1){
					valueArray[rowIdx][colIdx]= 15;//These are the edge pieces, stable because it has limited freedom for flipping 
				}else if (rowIdx==1||colIdx==1||rowIdx==n-2||colIdx==n-2){
					valueArray[rowIdx][colIdx]= 6;//These are the pieces inside the edge piece, stable to a limited extend.
				}else{
					valueArray[rowIdx][colIdx]= 1;//These are the rest of the pieces in the center.
				}
		
			} 
		}
		
		for (rowIdx=0; rowIdx<n; rowIdx++){
			for (colIdx=0; colIdx<n; colIdx++){
				if (board[rowIdx][colIdx]==colour){
					totalPieceValue+=valueArray[rowIdx][colIdx];
				} else if (board[rowIdx][colIdx]==oppoColour){
					totalPieceValue-=valueArray[rowIdx][colIdx];
				}
			}
		}
		
		/*for (int i=0; i<n;i++){
			for (int j=0; j<n; j++){
				printf("%3d ",valueArray[i][j]);
			}
			printf("\n");
		}*/
	
	return totalPieceValue;
}




void makeMove2 (char board[][26],int n, int stageNum, char colour, char oppoColour){
	int bestRow = 0, bestCol = 0;
	char[26][26] boardCopy = 
	makeMoveHelper(board, n, stageNum, colour, oppoColour, bestRow, bestCol);
}


void makeMoveHelper(char board[][26],int n, int stageNum, char colour, char oppoColour, int bestRow, int bestCol){
	
	if (stageNum == 1){
		printf("Computer places %c at %c%c.\n",colour,(char)('a'+bestRow),(char)('a'+ bestCol));
		flipTiles(board,n,colour,bestRow,bestCol);
		
	} else if (stageNum %2 ==1){
		int highScore = 0;
		int rowNum, colNum;
		int count = 0;
	//int a = evalPiecesValue ( board,  n,  colour, oppoColour);
		for (rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
			for (colNum=0; colNum<n; colNum++){
				if (checkLegalityOfPosition (board, n, rowNum, colNum, colour)){
					char stageOneBoard[26][26]={{0}};
					copyBoard(board, n, stageOneBoard);
					flipTiles(stageOneBoard,n,colour, rowNum, colNum);
					//printBoard(stageOneBoard,n);
					int thisScore=(computeActionNum(stageOneBoard, n, colour, oppoColour)*125+evalPiecesValue(stageOneBoard, n, colour, oppoColour));
				
				/*
				 * 
				 * 
				 * Need to add deeper
				 * 
				 */
				//printf("Score at %d%d: %d\n",rowNum,colNum,thisScore);
				
					if (count ==0){ //Set the first score to the highest score
						highScore = thisScore;
						bestRow = rowNum;
						bestCol = colNum;
						count++;	
					}
					else if (thisScore > highScore){//Update the high score when a higher one is computed
						highScore = thisScore;
						bestRow = rowNum;
						bestCol = colNum;
					}
				}
				
				
				}
			}
			
		makeMoveHelper(board, n, stageNum-1, colour, oppoColour, bestRow, bestCol);
	} else{
		int highScore = 0;
		int rowNum, colNum;
		int count = 0;
	//int a = evalPiecesValue ( board,  n,  colour, oppoColour);
		for (rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
			for (colNum=0; colNum<n; colNum++){
				if (checkLegalityOfPosition (board, n, rowNum, colNum, colour)){
					char stageOneBoard[26][26]={{0}};
					copyBoard(board, n, stageOneBoard);
					flipTiles(stageOneBoard,n,colour, rowNum, colNum);
					//printBoard(stageOneBoard,n);
					int thisScore= (computeActionNum(stageOneBoard, n, oppoColour, colour)*125+evalPiecesValue(stageOneBoard, n, oppoColour, colour));
				
					if (count ==0){ //Set the first score to the highest score
						highScore = thisScore;
						bestRow = rowNum;
						bestCol = colNum;
						count++;	
					}
					else if (thisScore > highScore){//Update the high score when a higher one is computed
						highScore = thisScore;
						bestRow = rowNum;
						bestCol = colNum;
					}
				}
				
				
				}
			}
			
		makeMoveHelper(board, n, stageNum-1, colour, oppoColour, bestRow, bestCol);
	}
	
}
