// Members: Alina Avramenko, Angelina Nyguyen, Becca Penno, Danica Jacutin, Shuvashree Basnet
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

# define MAP_HEIGHT 15
# define MAP_WIDTH 29
# define PELLET_POINTS 10
# define GHOST_COUNT 4
# define STARTING_LIVES	3
# define PACMAN_YELLOW "\033[1;93m"
# define ORANGE "\033[1;33m"
# define RED "\033[1;31m"
# define PINK "\033[1;35m"
# define CYAN "\033[1;36m"
# define BLUE "\033[1;34m"
# define POWER_PELLET_POINTS 30
# define GHOST_EAT_POINTS 200
# define POWER_DURATION 15
# define RESET "\033[0m"

typedef struct {
	int row;
	int column;
} Position;

typedef struct {
	int row;
	int column;
} Ghost;

typedef enum {
	UP,
	DOWN,
	LEFT,
	RIGHT
} Direction;

static const char map[MAP_HEIGHT][MAP_WIDTH + 1] = {
    "#############################",
    "#o......#...........#......o#",
    "#.#####.#####.#.#####.#####.#",
    "#.#.....#.....#.....#.....#.#",
    "#.#####.#####.#.#####.#####.#",
    "#.#.......................#.#",
    "#.#.######.##.####....#...#.#",
    "#......#...#.....#....#.....#",
    "#.#....#...#######.######.#.#",
    "#.#.......................#.#",
    "#.#####.#####.#.#####.#####.#",
    "#.#.....#.....#...........#.#",
    "#.#####.#####.#.#####.#####.#",
    "#o............#.....#......o#",
    "#############################"
};

static const char *ghostColors[GHOST_COUNT] = {
	RED, PINK, CYAN, ORANGE
};

static char board[MAP_HEIGHT][MAP_WIDTH + 1];

static int score = 0; 
static int pelletsLeft = 0;
static int lives = STARTING_LIVES;
static int powerTimer = 0;

static void initBoard(void) {
	int row;
	int column;
	pelletsLeft = 0;
	for(row=0; row < MAP_HEIGHT; row++) {
		strcpy(board[row], map[row]);
		for(column = 0; column < MAP_WIDTH; column++) {
			if(board[row][column] == '.' || board[row][column] == 'o') {
				pelletsLeft++;
			}
		}
	}
}

static void pelletEaten(Position pacman) {
	if (board[pacman.row][pacman.column] == '.') {
		board[pacman.row][pacman.column] = ' ';
		score += PELLET_POINTS;
		pelletsLeft--;
	} else if (board[pacman.row][pacman.column] == 'o') {
		board[pacman.row][pacman.column] = ' ';
		score += POWER_PELLET_POINTS;
		pelletsLeft--;
		powerTimer = POWER_DURATION;
	}
}

static int isWalkable(int row, int column) {
	return row >= 0 && row < MAP_HEIGHT &&
		column >= 0 && column < MAP_WIDTH &&
		map[row][column] != '#';
}

static void movePacman(Position *pacman, char direction) {
	int newRow = pacman -> row;
	int newColumn = pacman -> column;
	switch (direction) {
		case 'w':
			newRow--;
			break;
		case 's':
			newRow++;
			break;
		case 'a':
			newColumn--;
			break;
		case 'd':
			newColumn++;
			break;
		default:
			printf("Use w, a, s, d, or q.\n");
			return;
	}

	if (isWalkable(newRow, newColumn)) {
		pacman -> row = newRow;
		pacman -> column = newColumn;
		pelletEaten(*pacman);

	} else {
		printf("Pac-Man cannot move through a wall.\n");
	}
}

static void clearScreen(void) {
    printf("\033[2J\033[H");
}

static void showStartScreen(void){
	clearScreen();

	printf(PACMAN_YELLOW);
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf("                   PAC - MAN                 \n");
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf(RESET);


	 printf("\nWelcome to Pac-Man!\n\n");
    printf("HOW TO PLAY:\n");
    printf("  W - Move Up\n");
    printf("  A - Move Left\n");
    printf("  S - Move Down\n");
    printf("  D - Move Right\n");
    printf("  Q - Quit the game\n\n");

    printf("Collect all the pellets to win!\n");
    printf("Avoid the ghosts! You have 3 lives.\n\n");

    printf(PACMAN_YELLOW);
    printf("Press P to Play or Q to Quit\n");
    printf(RESET);

}

static void showWinScreen(int finalScore){
	clearScreen();

	printf(PACMAN_YELLOW);
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf("                                             \n");
	printf("                 YOU WIN!                    \n");
	printf("                                             \n");
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf(RESET);

	printf("\nCongratulations!\n");
	printf("You collected all the pellets!\n");
	printf("Final Score: %d\n", finalScore);
}

static void showGameOverScreen(int finalScore){
	clearScreen();

	printf(PACMAN_YELLOW);
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf("                                             \n");
	printf("                 GAME OVER                   \n");
	printf("                                             \n");
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf(RESET);

	printf("\nGame over!\n");
	printf("The ghosts caught pac-man!\n");
	printf("Final Score: %d\n", finalScore);

}

static void showQuitScreen(int finalScore){
	clearScreen();

	printf(PACMAN_YELLOW);
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf("              THANKS FOR PLAYING             \n");
	printf(" - - - - - - - - - - - - - - - - - - - - - - \n");
	printf(RESET);

	printf("\nFinal Score: %d\n", finalScore);
	printf("See you next time!\n");
}

static char askPlayAgain(void){
	char choice;
	
	while(1){
		printf("Press P to Play again or Q to Quit: ");
		if (scanf(" %c", &choice) != 1){
			return 'q';
		}

		switch (choice){
			case 'p':
			case 'P':
				return 'p';

			case 'q':
			case 'Q':
				return 'q';

			default:
				printf("Invalid choice. Please enter P or Q.\n");
		}
	}
}

static void resetGame(Position *pacman, Ghost ghosts[]) {
	score = 0;
	lives = STARTING_LIVES;
	powerTimer = 0;

	initBoard();

	pacman -> row = 7;
	pacman -> column = 14;

	ghosts[0] = (Ghost){5, 17};
	ghosts[1] = (Ghost){3, 13};
	ghosts[2] = (Ghost){5, 1};
	ghosts[3] = (Ghost){7, 10};

	pelletEaten(*pacman);
}
static void moveGhost(Ghost *ghost) {
	int newRow = ghost->row;
	int newColumn = ghost->column;
	Direction direction = rand() % 4;

	switch (direction) {
		case UP: newRow--; break;
		case DOWN: newRow++; break;
		case LEFT: newColumn--; break;
		case RIGHT: newColumn++; break;
	}

	if (isWalkable(newRow, newColumn)) {
		ghost->row = newRow;
		ghost->column = newColumn;
	}
}

static int hasCollision(Position pacman, Ghost ghosts[]) {
	int i;

	for (i = 0; i < GHOST_COUNT; i++) {
		if (pacman.row == ghosts[i].row &&
			pacman.column == ghosts[i].column) {
			return i;
		}
	}

	return -1;
}

static void drawMap(Position pacman, Ghost ghosts[]) {
	int row;
	int column;
	int i;
	
	for (row = 0; row < MAP_HEIGHT; row++) {
		for (column = 0; column < MAP_WIDTH; column++) {
			if (row == pacman.row && column == pacman.column) {
				printf(PACMAN_YELLOW "C" RESET);
		       	} else {
				int ghostFound = 0;
				for (i = 0; i < GHOST_COUNT; i++) {
					if (row == ghosts[i].row && column == ghosts[i].column) {
						printf("%sG" RESET, powerTimer > 0 ? BLUE : ghostColors[i]);
						ghostFound = 1;
						break;
					}
				}
				
				if (!ghostFound) {
					putchar(board[row][column]);
				}
			}
		}
		putchar('\n');
	}
}

int main(void) {
	Position pacman = {7, 14};
	const Ghost ghostStarts[GHOST_COUNT] = {
		{5, 17},
		{3, 13},
		{5,1},
		{7,10}
	};
	Ghost ghosts[GHOST_COUNT];
	memcpy(ghosts, ghostStarts, sizeof(ghosts));
	char direction;
	char choice;
	int gameRunning;

	srand((unsigned int) time(NULL));

	showStartScreen();
	
	do {
		printf("Enter your choice: ");

		if (scanf(" %c", &choice) != 1) {
			return 0;
		}

		if (choice == 'q' || choice == 'Q') {
			showQuitScreen(0);
			return 0;
		}

		if (choice != 'p' && choice != 'P') {
			printf("Please enter P to play or Q to quit.\n");
		}
	} while (choice != 'p' && choice != 'P');

	do {
		resetGame(&pacman, ghosts);
		gameRunning = 1;

		while (gameRunning && lives > 0 && pelletsLeft > 0) {
			clearScreen();
			printf("Score: %d Lives: %d Pellets left: %d\n\n", score, lives, pelletsLeft);

			drawMap(pacman, ghosts);
			printf("Move (w/a/s/d), or q to quit: ");

			if (scanf(" %c", &direction) != 1) {
				direction = 'q';
			}

			if (direction =='q' || direction == 'Q') { 
				showQuitScreen(score);
				return 0;
			}

			movePacman(&pacman, direction);

			for (int i = 0; i < GHOST_COUNT; i++) {
		    	moveGhost(&ghosts[i]);
	    	}

	    	int hit = hasCollision(pacman, ghosts);
			if (hit >= 0 && powerTimer > 0) {
				score += GHOST_EAT_POINTS;
				ghosts[hit] = ghostStarts[hit];
			}
			else if (hit >= 0) {
				lives--;
		    	if (lives > 0) {
			    	pacman = (Position){7, 13};
					memcpy(ghosts, ghostStarts, sizeof(ghosts));
		    	}
		}
		if (powerTimer > 0) {
			powerTimer--;
		}
		}
        
		if (lives == 0) {
			showGameOverScreen(score);
		}
		else if (pelletsLeft == 0) {
			showWinScreen(score);
		}

		choice = askPlayAgain();

    } while ( choice == 'p' || choice == 'P');

	showQuitScreen(score);
	return 0; 
}
