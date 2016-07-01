/**
 * drop.c
 */

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>


// constants
#define DIM 7
typedef struct {
    int x;
    int y;
} hit;
typedef char* string;
// board
int board[DIM][DIM]; //7 x 7 board
int rowCount[DIM]; //tracking tiles per row
int colCount[DIM]; //tracking tiles per column
int score = 0, moveCount = 0, level = 1; //initial global variables
bool levelEnd = false;


// prototypes
string GetString(void);
int GetInt(void);
void clear(void); //wipe board
void greet(void); //welcome
void init(void); //create board blank, seed tiles
void draw(void); //draw current board
void drop(int tile, int column); //add a tile
void count(void); //generates row and column counts; must be done before and after checkMatch;
int newTile(void); //generates random new tile 1-7 or o
void userPrompt(int tile); //uses while loop to GetInt and asks which column to drop tile in;
int checkMatch(hit hits[]); //successfully checks for matches and moves tiles down
void popTiles(hit hits[], int hitCount); //pop o tiles
void clearHits(hit hits[], int hitCount); //clear cleared tiles
void levelUp(void);
bool alive(void); //works!!

int main(int argc, string argv[])
{

    srand(time(NULL)); //seed for tile generation
    int hitCount = 0;
    hit hits[DIM * DIM];
    // greet user with instructions
    greet(); //works

    // initialize the board
    init(); //works w random placed tiles
    clear();
    draw();
    hitCount = checkMatch(hits);
    while (hitCount > 0) {
        clearHits(hits, hitCount);
        popTiles(hits, hitCount);
        hitCount = checkMatch(hits);
    }

    while(alive() && !levelEnd) {
        int tile = newTile();
        userPrompt(tile);
        count();
        hitCount = checkMatch(hits);
        while (hitCount > 0) {
            clearHits(hits, hitCount);
            popTiles(hits, hitCount);
            hitCount = checkMatch(hits);
        }
        if (moveCount == (29 - (level*level))) {
            levelUp();
            moveCount = 0;
        }
    }
    printf("\033[31;40mGame Over!\n\033[0mFinal Score: %i on Level %i\n", score, level);
    return 0;
}

void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

void greet(void)
{
    clear();
    printf("WELCOME TO DROP\n");
    usleep(250000);
}

void init(void)
{
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            board[i][j] = 0;
        }
    }
    int start = (rand() % 8);
    for (int k = 0; k < 5 + start; k++) {
        int tile = (rand() % 8) + 1;
        drop(tile, (rand() % 7));
    }
    count();
}

void drop(int tile, int column) {
    if (board[0][column] == 0 && tile != 0) {
        for (int i = 6; i >= 0 ; i--) {
            if (board[i][column] == 0) {
            board[i][column] = tile;
            break;
            }
        }
    } else {
        printf("Full column!\n");
        return;
    }
}

void count(void) {
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
                rowCount[i] = 0;
                colCount[j] = 0;
            }
        }

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (board[i][j] != 0) {
                rowCount[i]++;
                colCount[j]++;
            }
        }
    }
}

int newTile(void) {
    int tile = (rand() % 8) + 1;
    return tile;
}

void userPrompt(int tile) {
    int col;
    clear();
    printf("\033[30;47mYour score is %i\nMoves: %i\nLevel:%i\n", score, moveCount, level);
    do {
        if (tile == 8) {
            printf("\033[0mNew tile is o.\nWhich row?\n 1 2 3 4 5 6 7\n");
        } else {
            printf("\033[0mNew tile is %i.\nWhich row?\n 1 2 3 4 5 6 7\n", tile);
        }
        draw();
        col = GetInt();
    } while (col > 7 || col < 1 || board[0][col - 1] != 0);
    col--;
    moveCount++;
    drop(tile, col);
}

int checkMatch(hit hits[]) {
    int hitCount = 0;
    int hitCurrent = 0;
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (board[i][j] > 0 && board[i][j] < 8) {
                int up = 0, down = 0, left = 0, right = 0;
                for (int k = j - 1; k >= 0; k--) {
                    if (board[i][k] != 0) {
                        left++;
                    } else {
                        break;
                    }
                }
                for (int l = j + 1; l < DIM; l++) {
                    if (board[i][l] != 0) {
                        right++;
                    } else {
                        break;
                    }
                }
                for (int m = i - 1; m >= 0; m--) {
                    if (board[m][j] != 0) {
                        up++;
                    } else {
                        break;
                    }
                }
                for (int n = i + 1; n < DIM; n++) {
                    if (board[n][j] != 0) {
                        down++;
                    } else {
                        break;
                    }
                }
                if (left + right + 1 == board[i][j] || up + down + 1 == board[i][j]) {
                    hitCount++;
                    hits[hitCurrent].x = i;
                    hits[hitCurrent].y = j;
                    hitCurrent++;
                }
            }
        }
    }
    score += hitCount;
    return hitCount;
}

void clearHits(hit hits[], int hitCount) {
    int colStore[DIM];
    for (int i = 0; i < hitCount; i++) {
        for (int o = 0; o < DIM; o++) {
            colStore[o] = board[o][hits[i].y];
        }
        for (int p = hits[i].x; p > 0; p--) {
            board[p][hits[i].y] = colStore[p - 1];
        }
        board[0][hits[i].y] = 0;
        clear();
        draw();
    }
    count();
}

void popTiles(hit hits[], int hitCount) {
    for (int i = 0; i < hitCount; i++) {
        //NINES
        if (board[hits[i].x + 1][hits[i].y] == 9 && hits[i].x != (DIM - 1)) {
            board[hits[i].x + 1][hits[i].y] = newTile();
        }
        if (board[hits[i].x - 1][hits[i].y] == 9 && hits[i].x != 0) {
            board[hits[i].x - 1][hits[i].y] = newTile();
        }
        if (board[hits[i].x][hits[i].y + 1] == 9 && hits[i].y != (DIM - 1)) {
            board[hits[i].x][hits[i].y + 1] = newTile();
        }
        if (board[hits[i].x][hits[i].y - 1] == 9 && hits[i].y != 0) {
            board[hits[i].x][hits[i].y - 1] = newTile();
        }
        if (board[hits[i].x + 1][hits[i].y] == 8 && hits[i].x != (DIM - 1)) {
            board[hits[i].x + 1][hits[i].y] = 9;
        }
        if (board[hits[i].x - 1][hits[i].y] == 8 && hits[i].x != 0) {
            board[hits[i].x - 1][hits[i].y] = 9;
        }
        if (board[hits[i].x][hits[i].y + 1] == 8 && hits[i].y != (DIM - 1)) {
            board[hits[i].x][hits[i].y + 1] = 9;
        }
        if (board[hits[i].x][hits[i].y - 1] == 8 && hits[i].y != 0) {
            board[hits[i].x][hits[i].y - 1] = 9;
        }

    }
}

void levelUp(void) {
    int colStore[DIM];
    for (int w = 0; w < DIM; w++) {
        if (board[0][w] != 0) {
            levelEnd = true;
        }
    }
    for (int j = 0; j < DIM; j++) {
        for (int k = 0; k < DIM; k++) {
            colStore[k] = board[k][j];
        }
        for (int m = 0; m < 6; m++) {
            board[m][j] = colStore[m + 1];
        }
    }
    for (int n = 0; n < DIM; n++) {
        board[6][n] = 8;
    }
    level++;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (j == 0) {
                printf("\033[32;40;4m|");
            }
            if (board[i][j] == 0) {
                printf("\033[32;40;4m |");
            } else if (board[i][j] == 8) {
                printf("\033[32;40;4mo|");
            } else if (board[i][j] == 9) {
                printf("\033[31;40;4mo\033[32;40m|");
            } else {
            printf("\033[32;40;4m%i|", board[i][j]);
            }
        }
        printf("\n");
    }
}

bool alive(void)
{
    int full = 0;
    for (int i = 0; i < DIM; i++) {
        if (colCount[i] == 7) {
            full++;
        } else {
            return true;
        }
    }

    if (full == 7) {
    return false;
    } else {
        return true;
    }
}

int GetInt(void)
{
    // try to get an int from user
    while (true)
    {
        // get line of text, returning INT_MAX on failure
        string line = GetString();
        if (line == NULL)
        {
            return INT_MAX;
        }

        // return an int if only an int (possibly with
        // leading and/or trailing whitespace) was provided
        int n; char c;
        if (sscanf(line, " %d %c", &n, &c) == 1)
        {
            free(line);
            return n;
        }
        else
        {
            free(line);
            printf("Retry: ");
        }
    }
}

string GetString(void)
{
    // growable buffer for chars
    string buffer = NULL;

    // capacity of buffer
    unsigned int capacity = 0;

    // number of chars actually in buffer
    unsigned int n = 0;

    // character read or EOF
    int c;

    // iteratively get chars from standard input
    while ((c = fgetc(stdin)) != '\n' && c != EOF)
    {
        // grow buffer if necessary
        if (n + 1 > capacity)
        {
            // determine new capacity: start at 32 then double
            if (capacity == 0)
            {
                capacity = 32;
            }
            else if (capacity <= (UINT_MAX / 2))
            {
                capacity *= 2;
            }
            else
            {
                free(buffer);
                return NULL;
            }

            // extend buffer's capacity
            string temp = realloc(buffer, capacity * sizeof(char));
            if (temp == NULL)
            {
                free(buffer);
                return NULL;
            }
            buffer = temp;
        }

        // append current character to buffer
        buffer[n++] = c;
    }

    // return NULL if user provided no input
    if (n == 0 && c == EOF)
    {
        return NULL;
    }

    // minimize buffer
    string minimal = malloc((n + 1) * sizeof(char));
    strncpy(minimal, buffer, n);
    free(buffer);

    // terminate string
    minimal[n] = '\0';

    // return string
    return minimal;
}
