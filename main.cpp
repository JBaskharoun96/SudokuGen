#include <iostream>
#include <vector>
#include <random>

using namespace std;

/*
    Recursive backtracking algorithm to solve a sudoku puzzle by DFS
*/


bool isPossible     (int row, int col, int num, vector< vector<int> > &board);
bool isUnique       (vector< vector<int> > &board);
void fillBoard      (vector< vector<int> > &board);
void generateBoard  (vector< vector<int> > &board);
void printBoard     (vector< vector<int> > &board);
void solve          (vector< vector<int> > &board);
void zeroBoard      (vector< vector<int> > &board);

// constants to set size of board, may allow dynamic board sizes later
const int BOARD_SIZE = 9;
const int BLOCK_SIZE = 3;

int calls;
int numFound;
int numSols;
bool abortChain;
bool oneSol;

int main()
{

    vector< vector<int> > board
    {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };
    // count how many times solve() is called and the number of solutions found


    int choice = 0;

    do {
        calls = 0;
        numFound = 0;
        numSols = 0;
        abortChain = false;

        cout << '\n';
        cout << "0. Quit" << '\n';
        cout << "1. Display current board" << '\n';
        cout << "2. Generate a new random sudoku" << '\n';
        cout << "3. Fill in a board with custom values" << '\n';
        cout << "4. Solve current sudoku" << '\n';

        cin >> choice;

        switch (choice){
            case 0:
                cout << "\nSee you later, space cowboy" << '\n';
                return 0;
            case 1:
                cout << "\nCurrent board:" << '\n';
                printBoard(board);
                break;
            case 2:
                generateBoard(board);
                break;
            case 3:
                fillBoard(board);
                break;
            case 4:
                cout << "What is the max number of solutions you want to see?" << '\n';
                cin >> numSols;
                solve(board);
                cout << numFound << (numFound == 1 ? " solution" : " solutions") << " found" << '\n';
                break;
        }
    } while (choice != 0);

    return 0;
}

void fillBoard(vector< vector<int> > &board) {

    cout << "Enter the values of the sudoku puzzle from left to right, top to bottom: " << '\n';
    cout << "Numbers can be separated by either a space or a newline" << '\n';
    int temp;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            cin >> temp;

            board[i][j] = temp;
        }
    }
    return;
}

void generateBoard (vector< vector<int> > &board){

    // approach: craft a full board, then work backwards until minimum uniquely solvable board is found

    // instead of going one at a time run something like a binary search:
    // 1. store previous state in a second board
    // 2. remove half of digits
    // 3. check if unique, and if not add half back in

    // takes at most 6 solves of the board to find unique solution

    random_device rd;
    mt19937 twist(rd());
    uniform_int_distribution<int> dist(0, 8);
    bool inserted;
    int row, col, num;

    zeroBoard(board);

    // for (int i = 0; i < 17; i++){
    while (!oneSol){
        inserted = false;
        while (!inserted) {
            row = dist(twist);
            col = dist(twist);
            num = dist(twist) + 1;

            if ( isPossible(row, col, num, board) ){
                board[row][col] = num;
                inserted = true;
            }
        }
        if (isUnique(board)){
            break;
        }
    }
    return;
}

bool isPossible(int row, int col, int num, vector< vector<int> > &board) {

    int blockRow = row - (row % 3); // sets up the row and column of the block which the square being checked belongs to
    int blockCol = col - (col % 3); // sets to upper left corner

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[row][i] == num  ){ // if number is found elsewhere in the same row, cannot place
            return false;
        }
        if (board[i][col] == num){ // if number is found elsewhere in the same column, cannot place
            return false;
        }
    }
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            if (board[blockRow + i][blockCol + j] == num){ // if number is  found within the block, cannot place
                // cout << "Value " << num << " found in 3x3 block" << '\n';
                return false;
            }
        }
    }
    return true; // number not found in row, column, or block, is valid
}

void printBoard(vector< vector<int> > &board) {

    for (int i = 0; i < BOARD_SIZE; i++) {

        cout << '\n';
        cout << "     ";
        for (int j = 0; j < BOARD_SIZE; j++) {

            cout << board[i][j] << "    ";
        }
        cout << '\n' << '\n';
    }
    cout << "____________________________________________________" << '\n';

    return;
}

bool isUnique(vector< vector<int> > &board){
    oneSol = false;
    numFound = 0;
    numSols = 2;

    solve(board);

    if (numFound == 1) {
        oneSol = true;
    }

    return oneSol;
}

void solve(vector< vector<int> > &board) {

    calls++;

    for (int i = 0; i < BOARD_SIZE; i++) {

        for (int j = 0; j < BOARD_SIZE; j++) {

            if (board[i][j] == 0) { // check each square for blanks

                for (int n = 1; n <= BOARD_SIZE; n++) { // for each blank square, check each possible number (1 - 9)

                    if (isPossible(i, j, n, board) ) {     // here is where recursive backtracking happens

                        board[i][j] = n;           // if the number can fit, put it in

                        solve(board);                    // now we run solve on a board which is one square more complete
                        board[i][j] = 0;           // if we arrive here it means the solution branch is not viable, make blank again and keep checking
                        if (abortChain) {
                            return;
                        }
                    }

                }

                return;     // this return only runs if we find a blank square which no number can fit into
            }
        }
    }

    printBoard(board); // in this case because of how the recursion is set up, must print right before all the returns happen
    numSols--;
    numFound++;

    system("pause");

    if (numSols == 0){
        abortChain = true;
    }

    return; // this return runs when no blanks are found, meaning solution  is complete
}

void zeroBoard (vector< vector<int> > &board) {

    vector<int> vtemp (BOARD_SIZE, 0);
    vector< vector<int> > fullBoard (BOARD_SIZE, vtemp);

    board = fullBoard;

    return;
}

