#include <stdio.h>

# define MAP_HEIGHT 9
# define MAP_WIDTH 19
# define PELLET_POINTS 10

typedef struct {
	int row;
	int column;
} Position;

static const char map[MAP_HEIGHT][MAP_WIDTH + 1] = {
	"###################",
	"#.................#",
	"#.###.#####.###.#.#",
	"#.................#",
	"#.###.#...#.#.###.#",
	"#.....#...#.......#", 
	"#.###.#...#.###.#.#",
	"#.................#",
	"###################"
};

static char board[MAP_HEIGHT][MAP_WIDTH + 1];

static int score = 0; 
static int pelletsLeft = 0;
static void initBoard(void) {
	int row;
	int column;
	pelletsLeft = 0;
	for(row=0; row < MAP_HEIGHT; row++) {
		strcpy(board[row], map[row]);
		for(column = 0; column < MAP_WIDTH; column++) {
			if(board[row][column] = '.') {
				pelletsLeft++;
			}
		}
	}
}

static void pelletEaten(Position pacman) {
	if (board[pacman.row][pacman.column] == '.') {
		board[pacman.row][pacman.colum] = ' ';
		score += PELLET_POINTS;
		pelletsLeft--;

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
	} else {
		printf("Pac-Man cannot move through a wall.\n");
	}
}

static void drawMap(Position pacman) {
	int row;
	int column;

	for (row = 0; row < MAP_HEIGHT; row++) {
		for (column = 0; column < MAP_WIDTH; column++) {
			if (row == pacman.row && column == pacman.column) {
				putchar('C');
			} else {
				putchar(map[row][column]);
			}
		}

		putchar('\n');
	}
}

int main(void) {
	Position pacman = {1, 1};
	char direction;

	initBoard()'
	pelletEaten(pacman);

	do {
		drawMap(pacman);
		printf("Move (w/a/s/d), or q to quit: ");

		if (scanf(" %c", &direction) != 1) {
			break;
		}

		if (direction != 'q') {
			movePacman(&pacman, direction);
		}
	} while (direction != 'q');

	return 0; 
}
