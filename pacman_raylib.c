#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAP_HEIGHT 15
#define MAP_WIDTH 29
#define TILE_SIZE 24
#define HUD_HEIGHT 50
#define GHOST_COUNT 4
#define STARTING_LIVES 3

typedef struct {
    int row;
    int column;
} Position;

typedef struct {
    Position position;
    Color color;
} Ghost;

typedef enum {
    SCREEN_START,
    SCREEN_PLAYING,
    SCREEN_WIN,
    SCREEN_GAME_OVER
} GameState;

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

static char board[MAP_HEIGHT][MAP_WIDTH + 1];
static int score;
static int pelletsLeft;
static int lives;

static int isWalkable(int row, int column)
{
    return row >= 0 && row < MAP_HEIGHT &&
           column >= 0 && column < MAP_WIDTH &&
           map[row][column] != '#';
}

static Vector2 tileCenter(Position position)
{
    return (Vector2){
        position.column * TILE_SIZE + TILE_SIZE / 2.0f,
        HUD_HEIGHT + position.row * TILE_SIZE + TILE_SIZE / 2.0f
    };
}

static void resetGame(Position *pacman, Ghost ghosts[])
{
    for (int row = 0; row < MAP_HEIGHT; row++) {
        strcpy(board[row], map[row]);
    }

    score = 0;
    pelletsLeft = 0;
    lives = STARTING_LIVES;

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int column = 0; column < MAP_WIDTH; column++) {
            if (board[row][column] == '.' ||
                board[row][column] == 'o') {
                pelletsLeft++;
            }
        }
    }

    pacman->row = 7;
    pacman->column = 14;

    ghosts[0].position = (Position){5, 17};
    ghosts[1].position = (Position){3, 13};
    ghosts[2].position = (Position){5, 1};
    ghosts[3].position = (Position){7, 10};
}

static void eatPellet(Position pacman)
{
    if (board[pacman.row][pacman.column] == '.') {
        board[pacman.row][pacman.column] = ' ';
        score += 10;
        pelletsLeft--;
    } else if (board[pacman.row][pacman.column] == 'o') {
        board[pacman.row][pacman.column] = ' ';
        score += 30;
        pelletsLeft--;
    }
}

static int ghostCollision(Position pacman, Ghost ghosts[])
{
    for (int i = 0; i < GHOST_COUNT; i++) {
        if (pacman.row == ghosts[i].position.row &&
            pacman.column == ghosts[i].position.column) {
            return 1;
        }
    }

    return 0;
}

static void drawPacman(Vector2 center, int direction, int moving)
{
    float mouthAngle = moving
        ? 45.0f * fabsf(sinf((float)GetTime() * 8.0f))
        : 0.0f;

    float angle = (float)direction;

    DrawCircleV(center, TILE_SIZE * 0.42f, YELLOW);

    Vector2 tip = {
        center.x + cosf(angle * DEG2RAD) *
                    TILE_SIZE * 0.48f,
        center.y + sinf(angle * DEG2RAD) *
                    TILE_SIZE * 0.48f
    };

    Vector2 upper = {
        center.x + cosf((angle - mouthAngle) * DEG2RAD) *
                    TILE_SIZE * 0.5f,
        center.y + sinf((angle - mouthAngle) * DEG2RAD) *
                    TILE_SIZE * 0.5f
    };

    Vector2 lower = {
        center.x + cosf((angle + mouthAngle) * DEG2RAD) *
                    TILE_SIZE * 0.5f,
        center.y + sinf((angle + mouthAngle) * DEG2RAD) *
                    TILE_SIZE * 0.5f
    };

    DrawTriangle(tip, upper, lower, BLACK);
}

static void drawGhost(Ghost ghost)
{
    Vector2 center = tileCenter(ghost.position);
    float radius = TILE_SIZE * 0.38f;

    DrawCircle(center.x, center.y - 2, radius, ghost.color);

    DrawRectangle((int)(center.x - radius),
                  (int)(center.y - 2),
                  (int)(radius * 2),
                  (int)(radius + 3),
                  ghost.color);

    DrawCircle(center.x - 5, center.y - 4, 4, WHITE);
    DrawCircle(center.x + 5, center.y - 4, 4, WHITE);

    DrawCircle(center.x - 5, center.y - 4, 2, BLUE);
    DrawCircle(center.x + 5, center.y - 4, 2, BLUE);

    DrawCircle(center.x - 7, center.y + 9, 3, ghost.color);
    DrawCircle(center.x, center.y + 9, 3, ghost.color);
    DrawCircle(center.x + 7, center.y + 9, 3, ghost.color);
}

static void drawMaze(Ghost ghosts[])
{
    ClearBackground(BLACK);

    for (int row = 0; row < MAP_HEIGHT; row++) {
        for (int column = 0; column < MAP_WIDTH; column++) {
            Rectangle tile = {
                column * TILE_SIZE,
                HUD_HEIGHT + row * TILE_SIZE,
                TILE_SIZE,
                TILE_SIZE
            };

            if (board[row][column] == '#') {
                DrawRectangleLinesEx(tile, 2, BLUE);
            } else if (board[row][column] == '.' ||
                       board[row][column] == 'o') {
                Vector2 pellet = {
                    tile.x + TILE_SIZE / 2.0f,
                    tile.y + TILE_SIZE / 2.0f
                };

                float size = board[row][column] == 'o' ? 5.0f : 2.0f;
                Color color = board[row][column] == 'o'
                                  ? WHITE
                                  : LIGHTGRAY;

                DrawCircleV(pellet, size, color);
            }
        }
    }

    for (int i = 0; i < GHOST_COUNT; i++) {
        drawGhost(ghosts[i]);
    }
}

static void moveGhost(Ghost *ghost)
{
    int directions[4][2] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1}
    };

    int choice = GetRandomValue(0, 3);

    int newRow =
        ghost->position.row + directions[choice][0];

    int newColumn =
        ghost->position.column + directions[choice][1];

    if (isWalkable(newRow, newColumn)) {
        ghost->position.row = newRow;
        ghost->position.column = newColumn;
    }
}

int main(void)
{
    Position pacman;
    Position target;

    Ghost ghosts[GHOST_COUNT] = {
        {{5, 17}, RED},
        {{3, 13}, PINK},
        {{5, 1}, SKYBLUE},
        {{7, 10}, ORANGE}
    };

    GameState state = SCREEN_START;

    int pacmanDirection = 0;
    int requestedDirection = 0;
    int moveRequested = 0;
    int pacmanMoving = 0;

    Vector2 pacmanPixel;
    float ghostTimer = 0.0f;

    InitWindow(MAP_WIDTH * TILE_SIZE,
               HUD_HEIGHT + MAP_HEIGHT * TILE_SIZE,
               "Pac-Man");

    SetTargetFPS(60);
    SetRandomSeed((unsigned int)time(NULL));

    resetGame(&pacman, ghosts);
    target = pacman;
    pacmanPixel = tileCenter(pacman);

    while (!WindowShouldClose()) {
        float frameTime = GetFrameTime();

        if (state == SCREEN_START) {
            if (IsKeyPressed(KEY_P)) {
                resetGame(&pacman, ghosts);
                target = pacman;
                pacmanPixel = tileCenter(pacman);
                pacmanMoving = 0;
                moveRequested = 0;
                state = SCREEN_PLAYING;
            }

            if (IsKeyPressed(KEY_Q)) {
                break;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            DrawText("PAC-MAN", 260, 90, 42, YELLOW);
            DrawText("WASD or arrow keys to move",
                     205, 180, 20, WHITE);
            DrawText("P - Play", 280, 240, 24, GREEN);
            DrawText("Q - Quit", 280, 280, 24, RED);

            EndDrawing();
            continue;
        }

        if (state == SCREEN_PLAYING) {
            ghostTimer += frameTime;

            if (IsKeyPressed(KEY_W) ||
                IsKeyPressed(KEY_UP)) {
                requestedDirection = 1;
                moveRequested = 1;
            }

            if (IsKeyPressed(KEY_S) ||
                IsKeyPressed(KEY_DOWN)) {
                requestedDirection = 2;
                moveRequested = 1;
            }

            if (IsKeyPressed(KEY_A) ||
                IsKeyPressed(KEY_LEFT)) {
                requestedDirection = 3;
                moveRequested = 1;
            }

            if (IsKeyPressed(KEY_D) ||
                IsKeyPressed(KEY_RIGHT)) {
                requestedDirection = 0;
                moveRequested = 1;
            }

            if (IsKeyPressed(KEY_Q)) {
                break;
            }

            if (!pacmanMoving && moveRequested) {
                int rowChange[4] = {0, -1, 1, 0};
                int columnChange[4] = {1, 0, 0, -1};

                int newRow =
                    pacman.row +
                    rowChange[requestedDirection];

                int newColumn =
                    pacman.column +
                    columnChange[requestedDirection];

                if (isWalkable(newRow, newColumn)) {
                    target = (Position){newRow, newColumn};
                    pacmanMoving = 1;
                    moveRequested = 0;

                    pacmanDirection =
                        requestedDirection == 0 ? 0 :
                        requestedDirection == 1 ? 270 :
                        requestedDirection == 2 ? 90 : 180;
                } else {
                    moveRequested = 0;
                }
            }

            if (pacmanMoving) {
                Vector2 targetPixel = tileCenter(target);

                Vector2 difference = {
                    targetPixel.x - pacmanPixel.x,
                    targetPixel.y - pacmanPixel.y
                };

                float distance = sqrtf(
                    difference.x * difference.x +
                    difference.y * difference.y
                );

                float step = 120.0f * frameTime;

                if (distance <= step) {
                    pacmanPixel = targetPixel;
                    pacman = target;
                    eatPellet(pacman);
                    pacmanMoving = 0;
                } else {
                    pacmanPixel.x +=
                        difference.x / distance * step;

                    pacmanPixel.y +=
                        difference.y / distance * step;
                }
            }

            if (ghostTimer >= 0.25f) {
                ghostTimer = 0.0f;

                for (int i = 0; i < GHOST_COUNT; i++) {
                    moveGhost(&ghosts[i]);
                }
            }

            if (ghostCollision(pacman, ghosts)) {
                lives--;

                if (lives > 0) {
                    pacman = (Position){7, 14};
                    target = pacman;
                    pacmanPixel = tileCenter(pacman);
                    pacmanMoving = 0;
                    moveRequested = 0;

                    ghosts[0].position = (Position){5, 17};
                    ghosts[1].position = (Position){3, 13};
                    ghosts[2].position = (Position){5, 1};
                    ghosts[3].position = (Position){7, 10};
                } else {
                    state = SCREEN_GAME_OVER;
                }
            }

            if (pelletsLeft == 0) {
                state = SCREEN_WIN;
            }
        }

        BeginDrawing();

        if (state == SCREEN_PLAYING) {
            drawMaze(ghosts);
            drawPacman(pacmanPixel,
                       pacmanDirection,
                       pacmanMoving);

            DrawText(TextFormat("Score: %d   Lives: %d",
                               score, lives),
                     10, 12, 20, WHITE);

            DrawText("P: play  Q: quit",
                     500, 16, 16, WHITE);
        } else {
            ClearBackground(BLACK);

            DrawText(state == SCREEN_WIN
                         ? "YOU WIN!"
                         : "GAME OVER",
                     245, 100, 42,
                     state == SCREEN_WIN ? GREEN : RED);

            DrawText(TextFormat("Final score: %d",
                               score),
                     270, 180, 24, WHITE);

            DrawText("P - Play again", 260, 250, 22, GREEN);
            DrawText("Q - Quit", 290, 290, 22, RED);

            if (IsKeyPressed(KEY_P)) {
                resetGame(&pacman, ghosts);
                target = pacman;
                pacmanPixel = tileCenter(pacman);
                pacmanMoving = 0;
                moveRequested = 0;
                state = SCREEN_PLAYING;
            }

            if (IsKeyPressed(KEY_Q)) {
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
