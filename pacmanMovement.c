#include <stdio.h>

# define MAP_HEIGHT 9
# define MAP_WIDTH 19

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
