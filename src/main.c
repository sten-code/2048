#include <ti/screen.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Settings
#define BOARD_SIZE 4
#define TILE_SIZE 40
#define TILE_PADDING 5
#define FONT_SIZE 10

// Tile colors
#define HIGHERTILE 14790

// Font colors
#define BLACK_FORE 8452
#define WHITE_FORE 63421

int are2DArraysEqual(int arr1[BOARD_SIZE][BOARD_SIZE], int arr2[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (arr1[i][j] != arr2[i][j]) {
                return 0; // Arrays are not equal
            }
        }
    }
    return 1; // Arrays are equal
}

void copy2DArray(int source[BOARD_SIZE][BOARD_SIZE], int destination[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            destination[i][j] = source[i][j];
        }
    }
}

typedef struct
{
    int board[BOARD_SIZE][BOARD_SIZE];
} Game;

void add_random_tile(Game *game)
{
    int empty_cells[BOARD_SIZE * BOARD_SIZE][2];
    int num_empty_cells = 0;

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (game->board[i][j] == 0)
            {
                empty_cells[num_empty_cells][0] = i;
                empty_cells[num_empty_cells][1] = j;
                num_empty_cells++;
            }
        }
    }

    if (num_empty_cells > 0)
    {
        int index = rand() % num_empty_cells;
        int value = (rand() < RAND_MAX * 0.9) ? 2 : 4;
        int i = empty_cells[index][0];
        int j = empty_cells[index][1];

        game->board[i][j] = value;
    }
}

void init_game(Game *game)
{
    // Ensure that the game contains all 0 tiles
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            game->board[i][j] = 0;
        }
    }

    // Add two random tiles at the beginning
    add_random_tile(game);
    add_random_tile(game);
}

void render(Game *game)
{
    gfx_FillScreen(255);
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            int tile = game->board[i][j];
            int x = j * (TILE_SIZE + TILE_PADDING) + TILE_PADDING;
            int y = i * (TILE_SIZE + TILE_PADDING) + TILE_PADDING;
            int cx = x + TILE_SIZE / 2;
            int cy = y + TILE_SIZE / 2;

            if (tile == 2)
                gfx_SetColor(1);
            else if (tile == 4)
                gfx_SetColor(2);
            else if (tile == 8)
                gfx_SetColor(3);
            else if (tile == 16)
                gfx_SetColor(4);
            else if (tile == 32)
                gfx_SetColor(5);
            else if (tile == 64)
                gfx_SetColor(6);
            else if (tile == 128)
                gfx_SetColor(7);
            else if (tile == 256)
                gfx_SetColor(8);
            else if (tile == 512)
                gfx_SetColor(9);
            else if (tile == 1024)
                gfx_SetColor(10);
            else if (tile == 2048)
                gfx_SetColor(11);
            else if (tile > 2048)
                gfx_SetColor(12);
            else
                gfx_SetColor(0);

            gfx_FillRectangle(x, y, TILE_SIZE, TILE_SIZE);
            if (tile != 0)
            {
                char str[6];
                sprintf(str, "%d", tile);
                int sx = cx - gfx_GetStringWidth(str) / 2;
                gfx_PrintStringXY(str, sx, cy);
            }
        }
    }
}

void move_left(int **board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        // Remove all zeros
        int j = 0;
        for (int k = 0; k < BOARD_SIZE; k++) {
            if (board[i][k] != 0) {
                board[i][j++] = board[i][k];
            }
        }
        for (; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }

        // Merge adjacent identical tiles
        for (int k = 0; k < BOARD_SIZE - 1; k++) {
            if (board[i][k] == board[i][k + 1]) {
                board[i][k] *= 2;
                board[i][k + 1] = 0;
                k++;
            }
        }

        // Remove all zeros again
        j = 0;
        for (int k = 0; k < BOARD_SIZE; k++) {
            if (board[i][k] != 0) {
                board[i][j++] = board[i][k];
            }
        }
        for (; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

void flip_horizontal(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE / 2; j++) {
            // Swap elements symmetrically around the middle column
            int temp = board[i][j];
            board[i][j] = board[i][BOARD_SIZE - 1 - j];
            board[i][BOARD_SIZE - 1 - j] = temp;
        }
    }
}

void flip_vertical(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE / 2; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Swap elements symmetrically around the middle row
            int temp = board[i][j];
            board[i][j] = board[BOARD_SIZE - 1 - i][j];
            board[BOARD_SIZE - 1 - i][j] = temp;
        }
    }
}

int move(Game* game, int move) {
    // Store the current state of the board to check for changes
    int original_state[BOARD_SIZE][BOARD_SIZE];
    copy2DArray(game->board, original_state);

    switch (move) {
        case sk_Up:
            flip_vertical(game->board);
            move_left(game);
            flip_vertical(game->board);
            break;
        case sk_Down:
            flip_vertical(game->board);
            flip_horizontal(game->board);
            move_left(game);
            flip_horizontal(game->board);
            flip_vertical(game->board);
            break;
        case sk_Left:
            move_left(game);
            break;
        case sk_Right:
            flip_horizontal(game->board);
            move_left(game);
            flip_horizontal(game->board);
            break;
        default:
            // Invalid move
            return false;
    }

    // Check if the board has changed after the move
    int moved = !are2DArraysEqual(original_state, game->board);

    if (moved) {
        // If the board has changed, add a random tile and update the display
        add_random_tile(game);
        render(game);
    }

    return moved;
}

int main(void)
{
    gfx_Begin();
    gfx_SetFontHeight(FONT_SIZE);

    // Set the correct palette
    gfx_palette[0] = 26390;  // 0
    gfx_palette[1] = 30619;  // 2
    gfx_palette[2] = 30617;  // 4
    gfx_palette[3] = 31439;  // 8
    gfx_palette[4] = 31308;  // 16
    gfx_palette[5] = 31211;  // 32
    gfx_palette[6] = 31079;  // 64
    gfx_palette[7] = 30510;  // 128
    gfx_palette[8] = 30508;  // 256
    gfx_palette[9] = 30506;  // 512
    gfx_palette[10] = 30471; // 1024
    gfx_palette[11] = 30469; // 2048
    gfx_palette[12] = 7398;  // > 2048
    gfx_palette[13] = 32734; // White Fore
    gfx_palette[14] = 4228;  // Black Fore

    srand(time(NULL));
    Game game;
    init_game(&game);
    render(&game);

    int running = 1;
    while (running) {
        int keycode = os_GetCSC();
        if (keycode <= 0x04 && keycode >= 0x01) {
            move(&game, keycode);
        } else if (keycode == sk_Clear) {
            running = 0;
        }
    }
    

    gfx_End();

    return 0;
}
