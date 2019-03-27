#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <lab8part2lib.h>
#include <math.h>

#define MOBILITY_WEIGHT 125

bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection (char board[][26], int n, int row, int col,
							char colour, int deltaRow, int deltaCol);
bool checkLegalityOfPosition (char board[][26],int n, int row, int col, char colour);
void printBoard(char board[][26], int n);
void initializeBoard(char board[][26], int n);

void requestForMove(char board[][26],int n, char *colourPtr, int *rowPtr, int *colPtr);
void flipTiles(char board[][26],int n, char colour, int row, int col);
void makeMove(char board[][26],int n, char colour, char oppoColour);
void makeMove2(char board[][26],int n, char colour, char oppoColour);
bool checkPositionAvailable(char board[][26],int n, char colour);
void checkWhoWon(char board[][26],int n, char computerColour, char humanColour);
int computeActionNum (char board[][26],int n, char colour, char oppoColour);
int evalPiecesValue (char board[][26], int n, char colour, char oppoColour);
int moveHelper(char board[][26], int row, int col, int n, char colour, char oppoColour, int depth, bool maximizingMove);
//void makeMoveHelper(char board[][26],int n, int stageNum, char colour, char oppoColour, int *bestRowPtr, int *bestColPtr);

void copyBoard(char board1[][26], int n, char board2[][26]);


int main(void)
{
	int boardSize = 0;
	char computerColour, humanColour, colourOfTurn = 'B';
	printf("This is Part II. Enter the board dimension: ");
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
			makeMove2(board, boardSize, computerColour, humanColour);
			printBoard(board,boardSize);
     //break;//For testing
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
			findSmarterMove(board, boardSize, humanColour, &row, &col);
			printf("Testing AI move (row, col): %c%c\n", row +'a', col+'a');
			/*Human Input: 
			 * printf("Enter move for colour %c (RowCol): ", humanColour);
			char inputRow, inputCol;
			scanf(" %c%c", &inputRow, &inputCol);
			row = (int)(inputRow - 'a');
			col = (int)(inputCol - 'a'); //Request and define user input*/

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

/*void makeMove (char board[][26],int n, int stageNum, char colour, char oppoColour){
	int bestRow = 0, bestCol = 0;
	int *bestRowPtr = &bestRow, *bestColPtr = &bestCol;
	char boardCopy[26][26]={{0}};
	copyBoard(board, n, boardCopy);
	makeMoveHelper(boardCopy, n, stageNum, colour, oppoColour, bestRowPtr, bestColPtr);
	flipTiles(board,n,colour, *bestRowPtr, *bestColPtr);//Flips the tiles on the actual board.
}*/




void makeMove(char board[][26],int n, char colour, char oppoColour){
		//printf("Making my move.\n");
		
		int rowNum1, colNum1;
		int count = 0;
		int numMoves = computeActionNum(board, n, colour, oppoColour); //The total number of moves available for this step
		
		int **scoreSums = (int**)malloc (sizeof(int*)*numMoves);
		for (int i=0;i<numMoves;i++){
			scoreSums[i]= (int*)malloc (sizeof(int)*75); //Allocates 75 possible values for the secondary moves by opponent
		}
		free(scoreSums);
		
		int *moveRows = (int*)malloc(sizeof(int)*numMoves);
		int *moveCols = (int*)malloc(sizeof(int)*numMoves);
		//int moveRows[150]={0};
		//int moveCols[150]={0};//initialize arrays containing the number indices of possible moves
		int rowIndex, colIndex;
		//printf("Num Moves: %d\n", numMoves);
		if (numMoves==1){
		//	printf("My Action Num =1\n");
			for (rowIndex = 0; rowIndex < n; rowIndex++){
			for (colIndex = 0; colIndex < n; colIndex++){ //Iterates through the board
				if (checkLegalityOfPosition(board, n, rowIndex,colIndex, colour)){
						printf("Computer plays %c at: %c%c\n", colour, (char)('a'+rowIndex), (char)('a'+colIndex));
						flipTiles(board,n,colour, rowIndex, colIndex); //Flips the board
					}
			}
			}
		} else {
		//	printf("N value: %d\n", n);
		int rowNum, colNum;
		for(rowNum = 0; rowNum<n; rowNum++){ //Iterate through the entire board
			//printf("HERE!");
			for (colNum=0;  colNum<n; colNum++){
				//printf("HERE!");
				if (checkLegalityOfPosition (board, n, rowNum, colNum, colour)){
				
					moveRows[count]=rowNum;
					moveCols[count]=colNum;
				
					int count2 = 0; //Variable two count number of secondary moves. 
					char stageOneBoard[26][26]={{0}};
					copyBoard(board, n, stageOneBoard);
					flipTiles(stageOneBoard,n,colour, rowNum, colNum);
					//printBoard(stageOneBoard,n);
					int thisScore=(0-computeActionNum(stageOneBoard, n, oppoColour, colour)*MOBILITY_WEIGHT+evalPiecesValue(stageOneBoard, n, colour, oppoColour));
					printf("thisScore: %d\n",thisScore);
					if (computeActionNum(board, n, oppoColour, colour)>=1){
						printf("Opponent's num: %d\n",computeActionNum(board, n, oppoColour, colour) );
						for (rowNum1 = 0; rowNum1<n; rowNum1++){ //Iterate through the entire board to check opponent's possible moves
							for (colNum1=0; colNum1<n; colNum1++){
								printf("%c ", board[rowNum][colNum]);
							}
						}
								if (checkLegalityOfPosition (board, n, rowNum, colNum, oppoColour)){
									char stageTwoBoard[26][26]={{0}};
									copyBoard(stageOneBoard, n, stageTwoBoard);
									flipTiles(stageTwoBoard,n,oppoColour, rowNum1, colNum1);
									int secondaryScore = (0-computeActionNum(stageTwoBoard, n, colour, oppoColour)*MOBILITY_WEIGHT+evalPiecesValue(stageTwoBoard, n, oppoColour, colour));
									printf("Secondary Score: %d; Primary Score: %d, count: %d, count2: %d", secondaryScore, thisScore, count, count2);
									scoreSums[count][count2]= thisScore-(int)(0.8*secondaryScore);//Gives the opponent's move a weight of 0.8
									printf("Scoresums: %d", scoreSums[count][count2]);
									count2++; //The next iteration will be storeing the sum of scores on the second position. 
								}
							
						
					//printf("Score at %d%d: %d\n",rowNum,colNum,thisScore);
					count++;
				} else{
					printf("Only primary score, no Opponent's move.");
					scoreSums[count][0]= thisScore+1500;//Case that after my move, the opponent would have no move available, this is considered a good move. 
					count++;
				}
				}
			}
		}
		
		int highScore = scoreSums[0][0]; //initialize the highest score
		
		/*for (int i=0; i<numMoves; i++){
			for (int j=0; j<75; j++ ){
				printf("%d ", scoreSums[i][j]);
			}
			printf("\n");
		}*/
		
		
		int highestIndex;
		int scoreRow, scoreCol;
		for (scoreRow = 0; scoreRow<numMoves; scoreRow++){
			for (scoreCol = 0; scoreCol<numMoves; scoreCol++){
					//printf("Score Row: %d, ", scoreRow);
				if (scoreSums[scoreRow][scoreCol]>highScore){
					//printf("Larger! ");
					highScore = scoreSums[scoreRow][scoreCol];
					highestIndex = scoreRow;
					//printf("highestIndex: %d\n, ", highestIndex);
				}
			}
		}
		printf("Available Rows: ");
		for (int i=0; i<numMoves; i++){
			printf("%d ", moveRows[i]);
		}
		printf("\n available Columns: ");
		for (int i=0; i<numMoves; i++){
			printf("%d ", moveCols[i]);
		}
		printf("\nHighest Index: %d\n", highestIndex);
		printf("Computer plays %c at: %c%c\n", colour, (char)('a'+moveRows[highestIndex]), (char)('a'+moveCols[highestIndex]));
		flipTiles(board,n,colour, moveRows[highestIndex], moveCols[highestIndex]); //Flips the board and pass onto the next iteratio
		//printBoard(board, n);


}
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
					valueArray[rowIdx][colIdx]= -85; //These are the corner pieces inside the broadest corner pieces
				} else if ((rowIdx==0&&colIdx==0)||(rowIdx==n-1&&colIdx==n-1)||(rowIdx==0&&colIdx==n-1)||(rowIdx==n-1&&colIdx==0)){
					valueArray[rowIdx][colIdx]= 100; //These are the corner pieces, most valuable because these are impossible to be flipped
				}else if ((rowIdx==0&&colIdx==1)||(rowIdx==0&&colIdx==n-2)||(rowIdx==n-1&&colIdx==1)||(rowIdx==n-1&&colIdx==n-2)){
			
		valueArray[rowIdx][colIdx]= -60; //These are the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if ((rowIdx==1&&colIdx==0)||(rowIdx==1&&colIdx==n-1)||(rowIdx==n-2&&colIdx==0)||(rowIdx==n-2&&colIdx==n-1)){
					valueArray[rowIdx][colIdx]= -60; //These are also the pieces beside the corner, undesirable because they allow the opponent to take the corner
				}else if (rowIdx==0||colIdx==0||rowIdx==n-1||colIdx==n-1){
					valueArray[rowIdx][colIdx]= 25;//These are the edge pieces, stable because it has limited freedom for flipping 
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


void makeMove2(char board[][26],int n, char colour, char oppoColour){
	int maxScore = -20000;
	int maxRowInd = -1, maxColInd=-1;
	bool existLarger = false;
	int rowIndex, colIndex;
	for (rowIndex=0; rowIndex<n; rowIndex++){
		for (colIndex=0; colIndex<n; colIndex++){
			if (checkLegalityOfPosition (board, n, rowIndex, colIndex, colour)){
				
				char newBoard[26][26]={{0}};
				copyBoard(board, n, newBoard);
				flipTiles(newBoard, n, colour, rowIndex, colIndex);
				int thisScore = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour, 3, true);
				printf("Piece's Position: %d%d; Score: %d.\n", rowIndex, colIndex, thisScore);
		
		
				if (thisScore>maxScore){
					//For testing: 
					existLarger = true;
					printf("Exist a score larger than %d. This score is: %d\n", maxScore, thisScore);
					maxScore = thisScore;
					maxRowInd = rowIndex;
					maxColInd = colIndex;
				}
				}
			}			
		}
	if (existLarger==false){
		//Testing
		printf("ERROR OCCURED\n");
		exit(0);
		
	}
	printf("Computer plays %c at: %c%c\n", colour, (char)('a'+ maxRowInd), (char)('a'+ maxColInd));
	flipTiles(board, n, colour, maxRowInd, maxColInd);
}

int moveHelper(char board[][26], int row, int col, int n, char colour, char oppoColour, int depth, bool maximizingMove){//Eval Over?
	if (depth ==0 ){//Reach the bottom of evaluation tree or the case that no move is available
	
		int thisScore = (computeActionNum(board, n, colour, oppoColour)*MOBILITY_WEIGHT*0.8-computeActionNum(board, n, oppoColour, colour)*MOBILITY_WEIGHT+evalPiecesValue(board, n, colour, oppoColour)*1.2-evalPiecesValue(board, n, oppoColour, colour));
		flipTiles(board, n, colour, row, col);
		//For testing: printf("Bottom Node Reached. The score of the board below is: %d\n", thisScore);
		//For testing: printBoard(board, n);
		return thisScore;
		
	} else if (maximizingMove){ //Trying to find the maximum of moves among moves available for mycolour
		//int maxVal = -(int)INFINITY; //Ensures however small the range of scores are, they will always be larger than this value to be stored
		int maxVal = -300000;
		if (computeActionNum(board, n, colour, oppoColour)>0){
		int rowIndex, colIndex;
		int maxRow=0, maxCol=0;
		for (rowIndex=0; rowIndex<n; rowIndex++){
			for (colIndex=0; colIndex<n; colIndex++){
				if (checkLegalityOfPosition (board, n, rowIndex, colIndex, colour)){
					//existMyMove = true;
					char newBoard[26][26]={{0}};
					copyBoard(board, n, newBoard);
					flipTiles(newBoard, n, colour, rowIndex, colIndex);
					int thisVal = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour, depth-1, false);
					//printf("ThisMaxVal: %d\n", thisVal);
					if (thisVal>maxVal){
						maxVal = thisVal;
						maxRow = rowIndex;
						maxCol = colIndex;
					}
				}
			}			
		}
			return maxVal;
		} else {
			//if (depth==3){ //Case that when the board was passed, there was only one possible move. 
			//return 0;
		}
		
		
		
		//flipTiles(board, n, colour, maxRow, maxCol);
		//printf("Depth: %d. MaxValue: %d. The board is shown below\n", depth, maxVal);
		//For testing: printBoard(board, n);
		
		
	} else {
		//int minVal = (int)INFINITY;
		int minVal = 400000;
		int rowIndex, colIndex;
		int minRow=0, minCol=0;
		if (computeActionNum(board, n, colour, oppoColour)>0){
		for (rowIndex=0; rowIndex<n; rowIndex++){
			for (colIndex=0; colIndex<n; colIndex++){
				if (checkLegalityOfPosition (board, n, rowIndex, colIndex, oppoColour)){
					char newBoard[26][26]={{0}};
					copyBoard(board, n, newBoard);
					flipTiles(newBoard, n, oppoColour, rowIndex, colIndex);
					int thisVal = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour, depth-1, true);
					//printf("ThisMinVal: %d\n", thisVal);
					if (thisVal<minVal){
						minVal = thisVal;
						minRow = rowIndex;
						minCol = colIndex;
				
					}			
				}
			}
		}
		return minVal;
		}else{
		//return 0;
		
		}
		//
		printf("Depth: %d. MinVal: %d. The board is shown below\n", depth, minVal);
		//For testing: printBoard(board, n);
		//flipTiles(board, n, oppoColour, minRow, minCol);
	}
	
}
