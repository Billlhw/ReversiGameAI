/*
 * Added Alpha Beta Pruning on Minimax Algorithm in the makeMove2 function
 * Added timer and displays time for each move, ensures each of the AI's move is done within one second. 
 */
 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <lab8part2lib.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>

#define MOBILITY_WEIGHT 125

bool positionInBounds(int n, int row, int col);
bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol);
bool checkLegalityOfPosition(char board[][26], int n, int row, int col, char colour);
void printBoard(char board[][26], int n);
void initializeBoard(char board[][26], int n);

void flipTiles(char board[][26], int n, char colour, int row, int col);

void makeMove2(char board[][26], int n, char colour, char oppoColour);
bool checkPositionAvailable(char board[][26], int n, char colour);
void checkWhoWon(char board[][26], int n, char computerColour, char profAIColour);
int computeActionNum(char board[][26], int n, char colour, char oppoColour);
int evalPiecesValue(char board[][26], int n, char colour, char oppoColour);
int moveHelper(char board[][26], int row, int col, int n, char colour, char oppoColour, int depth, int alpha, int beta, bool maximizingMove);
// void makeMoveHelper(char board[][26],int n, int stageNum, char colour, char oppoColour, int *bestRowPtr, int
// *bestColPtr);

void copyBoard(char board1[][26], int n, char board2[][26]);

int main(void)
{
    int boardSize = 0;
    char computerColour, profAIColour, colourOfTurn = 'B';
    printf("Enter the board dimension: ");
    scanf("%d", &boardSize); // Request for board size
    printf("Computer plays (B/W): ");
    getchar();
    scanf("%c", &computerColour);
    if (computerColour == 'B') {
        profAIColour = 'W';
    } else {
        profAIColour = 'B';
    } // Set the colour of human player to be opposite from computer's colour.
    char board[26][26] = { { 0 } }; // Declares the 2D array board
    int row = 0, col = 0; // Initialize the variable containing user input
    // int *rowPtr = &row, *colPtr = &col;
    initializeBoard(board, boardSize);
    printBoard(board, boardSize);
    bool continuePlaying = true;
    while (continuePlaying) {
        // char needCheckStatus;//If colour of turn is assigned as this, will bring to the case that
        // there might have been people won as no position is available
        if (colourOfTurn == computerColour) {
            // testing: printf("Num of Available moves for computer is: %d\n", computeActionNum(board,boardSize,
            // computerColour, humanColour));
            makeMove2(board, boardSize, computerColour, profAIColour);
            printBoard(board, boardSize);
            // break;//For testing
            if (checkPositionAvailable(board, boardSize, profAIColour)) {
                colourOfTurn = profAIColour; // Switch turn to human player if human has a move available.
            } else if (checkPositionAvailable(board, boardSize, computerColour)) {
                // Case that the human player has no move available but computer still has.
                printf("%c player has no valid move.\n", profAIColour);
                colourOfTurn = computerColour; // Still computer's turn
            } else { // No one has move availble, game ends.
                checkWhoWon(board, boardSize, computerColour, profAIColour);
                break;
            }
        } else { // Turn of the human player
            // Professor AI Input:
          // findSmartestMove(board, boardSize, profAIColour, &row, &col);
          //  printf("Testing AI move (row, col): %c%c\n", row +'a', col+'a');
            // Human Input:
            printf("Enter move for colour %c (RowCol): ", profAIColour);
            char inputRow, inputCol;
            getchar();
           scanf("%c%c",&inputRow, &inputCol);
           row = (int)(inputRow - 'a');
           col = (int)(inputCol - 'a'); // Request and define user input

            if (positionInBounds(boardSize, row, col) &&
                checkLegalityOfPosition(board, boardSize, row, col, profAIColour)) { // Case that the position entered
                                                                                     // by human player is valid
                flipTiles(board, boardSize, profAIColour, row, col);
                printBoard(board, boardSize);
            } else {
                printf("Invalid move.\n");
                printf("%c player wins.\n",
                       computerColour); // Case that the compter has won because human player inputted a invalid input
                break;
            }
            if (checkPositionAvailable(board, boardSize, computerColour)) {
                colourOfTurn =
                    computerColour; // Case that the computer has a position available and switch the turn to computer
            } else if (checkPositionAvailable(board, boardSize, profAIColour)) {
                // Case that the human player has no move available but computer still has.
                printf("%c player has no valid move.\n", computerColour);
                colourOfTurn = profAIColour; // Still human player's turn
            } else { // No one has move available, so check who has won.
                checkWhoWon(board, boardSize, computerColour, profAIColour);
                break;
            }
        }
    }
    return 0;
}

/*
 * Assign letters to the pieces on the board
 */
void initializeBoard(char board[][26], int n)
{
    int middleIndex = n / 2 - 1;
    int rowInd, colInd;
    for (rowInd = 0; rowInd < n; rowInd++) {
        for (colInd = 0; colInd < n; colInd++) {
            if (rowInd == middleIndex &&
                colInd == middleIndex) { // Initializes the center of the board to the configuration given
                board[rowInd][colInd] = 'W';
            } else if (rowInd == middleIndex + 1 && colInd == middleIndex) {
                board[rowInd][colInd] = 'B';
            } else if (rowInd == middleIndex && colInd == middleIndex + 1) {
                board[rowInd][colInd] = 'B';
            } else if (rowInd == middleIndex + 1 && colInd == middleIndex + 1) {
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
bool positionInBounds(int n, int row, int col)
{
    if (row >= n || col >= n) {
        return false;
    } else {
        return true;
    }
}

/*
 * The function that checks for the legality of a piece to be a candidate, given the direction checking
 */
bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol)
{
    int currentRow = row + deltaRow, currentCol = col + deltaCol;
    char oppoColour;
    if (colour == 'W') { // Defines the char containing opponent's colour
        oppoColour = 'B';
    } else {
        oppoColour = 'W';
    }
    bool existOppoPieceBetween = false;
    while (currentRow <= n && currentCol <= n) {
        // Check the pieces in the indicated direction iteratively until reaches boundary or the two conditions for
        // candidate piece are met,
        // namely at least one opponent's piece and one of the player's own piece exist in the indicated direction
        if (board[currentRow][currentCol] ==
            oppoColour) { // Case that there is an opponent's piece in the specified direction
            existOppoPieceBetween = true;
            // printf("exists OppoPiece (%c) at: %d%d\n",oppoColour, currentRow,currentCol); //testing
            currentRow += deltaRow;
            currentCol += deltaCol; // Set the current index checking to be the next space
        } else if (board[currentRow][currentCol] == colour) {
            if (existOppoPieceBetween) {
                // printf("exists myPiece (%c) at: %d%d\n",colour, currentRow, currentCol);//testing
                // printf("Position %d%d is available for colour %c\n",row,col, colour);
                return true;
                // The following piece exists and there is at least one opponent's piece in between
            } else {
                return false; // A piece of the same colour is directly near it
            }
        } else { // Case that there is no piece on the indicated direction
            break; // Condition is not met since line of pieces must be continuous
        }
    }
    return false;
}

/*
 * Initiates the checking process of a position for legality of being a candidate
 * Calls the function checkLegalInDirection
 */
bool checkLegalityOfPosition(char board[][26], int n, int row, int col, char colour)
{
    if (board[row][col] == 'B' || board[row][col] == 'W') { // Will not proceed if this place has been occupied
        return false;
    }
    int rowIndex, colIndex; // Indices used for specifying the direction to check
    for (rowIndex = -1; rowIndex <= 1; rowIndex++) {
        for (colIndex = -1; colIndex <= 1; colIndex++) {
            if (!(rowIndex == 0 &&
                  colIndex == 0)) { // Do not proceed to check-in-direction when the direction in both ways are zero
                if (positionInBounds(n, row + rowIndex, col + colIndex) &&
                    checkLegalInDirection(board, n, row, col, colour, rowIndex, colIndex))
                    return true; // The conditions are met by the pieces on the current direction
            }
        }
    }
    return false; // Condition not met after checking all eight directions
}

/*
 * Prints the board, the board array and size of board are the parameter
 */
void printBoard(char board[][26], int n)
{

    printf("  "); // align the row indicators (a,b,c,d,etc.) with the chess board
    int indicatorIndex;
    for (indicatorIndex = 0; indicatorIndex < n; indicatorIndex++) { // Prints the row indicators on the top
        printf("%c", 'a' + indicatorIndex);
    }
    printf("\n");
    int rowInd, colInd;
    for (rowInd = 0; rowInd < n; rowInd++) { // Prints the rest of the board
        printf("%c ", 'a' + rowInd); // prints the column indicators
        for (colInd = 0; colInd < n; colInd++) {
            printf("%c", board[rowInd][colInd]);
        }
        printf("\n");
    }
}

/*
 * Row and col in the parameter is the location of the place user inputted, after verification
 * This function will flip the pieces caused by the piece placed.
 */
void flipTiles(char board[][26], int n, char colour, int row, int col)
{
    int rowIndex, colIndex; // Indices used for specifying the direction to check
    board[row][col] = colour;
    for (rowIndex = -1; rowIndex <= 1; rowIndex++) {
        for (colIndex = -1; colIndex <= 1; colIndex++) {
            if (!(rowIndex == 0 &&
                  colIndex == 0)) { // Do not proceed to check-in-direction when the direction in both ways are zero
                if (checkLegalInDirection(board, n, row, col, colour, rowIndex, colIndex)) {
                    // The case that there tiles in this direction can be flipped
                    int currentRow = row + rowIndex, currentCol = col + colIndex;
                    while (board[currentRow][currentCol] != colour) {
                        board[currentRow][currentCol] =
                            colour; // Set the pieces in between the given colour to the colour.
                        currentRow += rowIndex;
                        currentCol += colIndex;
                    }
                }
            }
        }
    }
}



/*
 * Iterates through the board to count the number of moves available
 *
 */
int computeActionNum(char board[][26], int n, char colour, char oppoColour)
{
    int rowIndex, colIndex;
    int actionNum = 0;
    for (rowIndex = 0; rowIndex < n; rowIndex++) {
        for (colIndex = 0; colIndex < n; colIndex++) { // Iterates through the board
            if (checkLegalityOfPosition(board, n, rowIndex, colIndex, colour)) {
                actionNum++;
            }
        }
    }
    return actionNum;
}

int evalPiecesValue(char board[][26], int n, char colour, char oppoColour)
{
    /*int **valueArray = (int**)malloc (sizeof(int*)*n); //create a 2D array of size of board dynamically to contain the
    value of pieces on each position.
    int initalizeIdx;
    for (initalizeIdx=0;initalizeIdx<n;initalizeIdx++){
    valueArray[initalizeIdx]= (int*)malloc (sizeof(int)*n); //initialize each row of the array
    }
    free(valueArray);//Avoid overflow
    */
    int valueArray[26][26] = {{0}};
    int totalPieceValue = 0;
    int rowIdx, colIdx;
    for (rowIdx = 0; rowIdx < n; rowIdx++) { // Assign this array to their respective values
        for (colIdx = 0; colIdx < n; colIdx++) {
            if ((rowIdx == 1 && colIdx == 1) || (rowIdx == n - 2 && colIdx == n - 2) ||
                (rowIdx == 1 && colIdx == n - 2) || (rowIdx == n - 2 && colIdx == 1)) {
                valueArray[rowIdx][colIdx] = -90; // These are the corner pieces inside the broadest corner pieces
            } else if ((rowIdx == 0 && colIdx == 0) || (rowIdx == n - 1 && colIdx == n - 1) ||
                       (rowIdx == 0 && colIdx == n - 1) || (rowIdx == n - 1 && colIdx == 0)) {
                valueArray[rowIdx][colIdx] = 150; // These are the corner pieces, most valuable because these are impossible to be flipped
            } else if ((rowIdx == 0 && colIdx == 1) || (rowIdx == 0 && colIdx == n - 2) ||
                       (rowIdx == n - 1 && colIdx == 1) || (rowIdx == n - 1 && colIdx == n - 2)) {
                valueArray[rowIdx][colIdx] = -70; // These are the pieces beside the corner, undesirable because they
                                                  // allow the opponent to take the corner
            } else if ((rowIdx == 1 && colIdx == 0) || (rowIdx == 1 && colIdx == n - 1) ||
                       (rowIdx == n - 2 && colIdx == 0) || (rowIdx == n - 2 && colIdx == n - 1)) {
                valueArray[rowIdx][colIdx] = -70; // These are also the pieces beside the corner, undesirable because
                                                  // they allow the opponent to take the corner
            } else if (rowIdx == 0 || colIdx == 0 || rowIdx == n - 1 || colIdx == n - 1) {
                valueArray[rowIdx][colIdx] =
                    20; // These are the edge pieces, stable because it has limited freedom for flipping
            } else if (rowIdx == 1 || colIdx == 1 || rowIdx == n - 2 || colIdx == n - 2) {
                valueArray[rowIdx][colIdx] =
                    6; // These are the pieces inside the edge piece, stable to a limited extend.
            } else {
                valueArray[rowIdx][colIdx] = 1; // These are the rest of the pieces in the center.
            }
        }
    }
    for (rowIdx = 0; rowIdx < n; rowIdx++) {
        for (colIdx = 0; colIdx < n; colIdx++) {
            if (board[rowIdx][colIdx] == colour) {
                totalPieceValue += valueArray[rowIdx][colIdx];
            }  else if (board[rowIdx][colIdx]==oppoColour){
            totalPieceValue-=(int)(0.8*valueArray[rowIdx][colIdx]);
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

bool checkPositionAvailable(char board[][26], int n, char colour)
{
    int rowInd, colInd;
    // Checks if move is available for a given colour
    for (rowInd = 0; rowInd < n; rowInd++) {
        for (colInd = 0; colInd < n; colInd++) {
            if (checkLegalityOfPosition(board, n, rowInd, colInd, colour))
                return true;
        }
    }
    return false;
}

void checkWhoWon(char board[][26], int n, char computerColour, char profAIColour)
{
    int computerTileNum = 0, humanTileNum = 0;
    int rowNum, colNum;
    for (rowNum = 0; rowNum < n; rowNum++) {
        for (colNum = 0; colNum < n; colNum++) {
            if (board[rowNum][colNum] == computerColour) {
                computerTileNum++;
            } else if (board[rowNum][colNum] == profAIColour) {
                humanTileNum++;
            }
        }
    }
    if (computerTileNum > humanTileNum) {
        printf("%c player wins.\n", computerColour);
    } else if (computerTileNum < humanTileNum) {
        printf("%c player wins.\n", profAIColour);
    } else {
        printf("Draw!");
    }
}

void copyBoard(char board1[][26], int n, char board2[][26])
{
    int row, col;
    for (row = 0; row < n; row++) {
        for (col = 0; col < n; col++) {
            board2[row][col] =
                board1[row][col]; // Assign each value in the second board to be the same value as on the first board.
        }
    }
}

void makeMove2(char board[][26], int n, char colour, char oppoColour)
{
    struct rusage usage; // a structure to hold "resource usage"
    struct timeval start, end; // will hold the start and end times
    getrusage(RUSAGE_SELF, &usage);
    start = usage.ru_utime;
    double timeStart = start.tv_sec + start.tv_usec / 1000000.0; // in seconds
    // Code to calculate the move is below.
    bool stop = false;
    int maxScore = -20000; // Assign as a very small negative number so that the score larger than this can be stored.
    int maxRowInd = -1, maxColInd = -1;
    bool existLarger = false;
    int rowIndex, colIndex;
	double totalTime;
    for (rowIndex = 0; rowIndex < n && !stop;
         rowIndex++) { // The boolean stop terminates this loop when time limit is reached.
        for (colIndex = 0; colIndex < n && !stop; colIndex++) {
            if (checkLegalityOfPosition(board, n, rowIndex, colIndex, colour)) {
				
                char newBoard[26][26] = { { 0 } };
                copyBoard(board, n, newBoard);
                flipTiles(newBoard, n, colour, rowIndex, colIndex);
				
                int thisScore = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour,6, -50000000, 5000000, true);
                // For testing: printf("Piece's Position: %d%d; Score: %d.\n", rowIndex, colIndex, thisScore);
                if (thisScore > maxScore) {
                    // For testing:
                    existLarger = true;
                    // For testing: printf("Exist a score larger than %d. This score is: %d\n", maxScore, thisScore);
                    maxScore = thisScore;
                    maxRowInd = rowIndex;
                    maxColInd = colIndex;
                }
                getrusage(RUSAGE_SELF, &usage);
                end = usage.ru_utime;
                 double timeEnd = end.tv_sec + end.tv_usec / 1000000.0; // in seconds
                 totalTime = timeEnd - timeStart;
                // Total time now holds time in seconds.
                // For testing to check the time for move: printf("Total Time: %lf\n", totalTime);
                if (totalTime > 0.78) { // The time approaches 1s, to prevent deduction in mark, end it about a tenth of
                                        // a second before.
                    // For testing: check if program is stopped when time limit is reached. printf("BREAK\n");
					//printf("Time is now: %lf\n", totalTime);
					//printf("STOP!\n");
                    stop = true;
                }
            }
        }
    }
   // if (existLarger == false) {
        // Testing
      //  printf("ERROR OCCURED\n");
       // exit(0);
	   
   // }
   printf("Time: %lf\n", totalTime);
    printf("Computer places %c at %c%c.\n", colour, (char)('a' + maxRowInd), (char)('a' + maxColInd));
    flipTiles(board, n, colour, maxRowInd, maxColInd);
}

int moveHelper(char board[][26], int row, int col, int n, char colour, char oppoColour, int depth, int alpha, int beta, bool maximizingMove)
{ 
    if (depth == 0) { // Reach the bottom of evaluation tree or the case that no move is available
        int thisScore =
            (computeActionNum(board, n, colour, oppoColour) * MOBILITY_WEIGHT * 0.4 -
             computeActionNum(board, n, oppoColour, colour) * MOBILITY_WEIGHT +
             evalPiecesValue(board, n, colour, oppoColour) * 1.2 - evalPiecesValue(board, n, oppoColour, colour));
        flipTiles(board, n, colour, row, col);
        // For testing: printf("Bottom Node Reached. The score of the board below is: %d\n", thisScore);
        // For testing: printBoard(board, n);
        return thisScore;
    } else if (maximizingMove) { // Trying to find the maximum of moves among moves available for mycolour
        // int maxVal = -(int)INFINITY; //Ensures however small the range of scores are, they will always be larger than
        // this value to be stored
        int maxVal = -300000;
        if (computeActionNum(board, n, colour, oppoColour) > 0) {
            int rowIndex, colIndex;
            int maxRow = 0, maxCol = 0;
			bool stop = false; //To break out of multople for loops.
            for (rowIndex = 0; rowIndex < n &&!stop; rowIndex++) {
                for (colIndex = 0; colIndex < n&&!stop; colIndex++) {
                    if (checkLegalityOfPosition(board, n, rowIndex, colIndex, colour)) {
                        // existMyMove = true;
                        char newBoard[26][26] = { { 0 } };
                        copyBoard(board, n, newBoard);
                        flipTiles(newBoard, n, colour, rowIndex, colIndex);
                        int thisVal = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour, depth - 1, alpha, beta,  false);
                        // printf("ThisMaxVal: %d\n", thisVal);
		
                        if (thisVal > maxVal) {
                            maxVal = thisVal;
                            maxRow = rowIndex;
                            maxCol = colIndex;
                        }
						if (thisVal>alpha){
							alpha=thisVal;
						}
							
						if (beta <=alpha){
							stop = true;
						}
                    }
                }
            }
            return maxVal;
			
        } else {
            // if (depth==3){ //Case that when the board was passed, there was only one possible move.
            return 0;
        }
        // flipTiles(board, n, colour, maxRow, maxCol);
        // printf("Depth: %d. MaxValue: %d. The board is shown below\n", depth, maxVal);
        // For testing: printBoard(board, n);
    } else {
        // int minVal = (int)INFINITY;
        int minVal = 400000;
        int rowIndex, colIndex;
        int minRow = 0, minCol = 0;
		bool stop = false;
        if (computeActionNum(board, n, colour, oppoColour) > 0) {
            for (rowIndex = 0; rowIndex < n && !stop; rowIndex++) {
                for (colIndex = 0; colIndex < n && !stop; colIndex++) {
                    if (checkLegalityOfPosition(board, n, rowIndex, colIndex, oppoColour)) {
                        char newBoard[26][26] = { { 0 } };
                        copyBoard(board, n, newBoard);
                        flipTiles(newBoard, n, oppoColour, rowIndex, colIndex);
                        int thisVal = moveHelper(newBoard, rowIndex, colIndex, n, colour, oppoColour, depth - 1, alpha, beta, true);
                        // printf("ThisMinVal: %d\n", thisVal);
                        if (thisVal < minVal) {
                            minVal = thisVal;
                            minRow = rowIndex;
                            minCol = colIndex;
                        }
						if (thisVal<beta){
							beta=thisVal;
						}
						if (beta<=alpha){
						stop = true; //Since the opponent is searching for a move the best for them, they will not choose this branch. 
                    }
                }
            }  
        } 
		return minVal;
		//else {
          //  return 0;
       // }
        //
        // printf("Depth: %d. MinVal: %d. The board is shown below\n", depth, minVal);
        // For testing: printBoard(board, n);
        // flipTiles(board, n, oppoColour, minRow, minCol);
    }
    return 0; // Avoids the warning of "reach the end of non-void function".
}
}
