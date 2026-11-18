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
    "#.......#...........#.......#",
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
    "#.............#.....#.......#",
    "#############################"
};

static const char *ghostColors[GHOST_COUNT] = {
	RED, PINK, CYAN, ORANGE
};

static char board[MAP_HEIGHT][MAP_WIDTH + 1];

static int score = 0; 
static int pelletsLeft = 0;
static int lives = STARTING_LIVES;

static void initBoard(void) {
	int row;
	int column;
	pelletsLeft = 0;
	for(row=0; row < MAP_HEIGHT; row++) {
		strcpy(board[row], map[row]);
		for(column = 0; column < MAP_WIDTH; column++) {
			if(board[row][column] == '.') {
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
	pelletEaten(*pacman);

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
	} else {
		printf("Pac-Man cannot move through a wall.\n");
	}
}

static void clearScreen(void) {
    printf("\033[2J\033[H");
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
			return 1;
		}
	}

	return 0;
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
						printf("%sG" RESET, ghostColors[i]);
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
	Ghost ghosts[GHOST_COUNT] = {
		{5, 17},
		{3, 14},
		{5, 1},
		{7, 10}
	};
	char direction;

	srand((unsigned int) time(NULL));

	initBoard();

	printf("Score: %d   Pellets left: %d\n\n", score, pelletsLeft);

	pelletEaten(pacman);

	do {
        clearScreen();
	printf("Score: %d   Lives: %d   Pellets left: %d\n\n", score, lives, pelletsLeft);
        drawMap(pacman, ghosts);

        printf("Move (w/a/s/d), or q to quit: ");

        if (scanf(" %c", &direction) != 1) {
            break;
        }

        if (direction != 'q') {
            movePacman(&pacman, direction);
	    for (int i = 0; i < GHOST_COUNT; i++) {
		    moveGhost(&ghosts[i]);
	    }

	    if (hasCollision(pacman, ghosts)) {
		    lives--;
		    printf("Pac-Man was caught! Lives remaining: %d\n", lives);
		    
		    if (lives > 0) {
			    pacman = (Position){1, 1};
			    ghosts[0] = (Ghost){5, 17};
			    ghosts[1] = (Ghost){3, 14};
			    ghosts[2] = (Ghost){5, 1};
			    ghosts[3] = (Ghost){7, 10};
		    }
	    }
        }

    } while (direction != 'q' && lives > 0 && pelletsLeft > 0);
	
	clearScreen();
	if (lives == 0) {
		printf("Game over! Final score: %d\n", score);
	} else if (pelletsLeft == 0) {
		printf("You win! Final score: %d\n", score);
	}

	return 0; 
}
