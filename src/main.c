#include <ti/screen.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <fileioc.h>
#include "graphics.h"

// Settings
#define BOARD_SIZE 4
#define TILE_SIZE 40
#define TILE_PADDING 5
#define BOARD_X 30
#define BOARD_Y 30

// Tile colors
#define HIGHERTILE 14790

// Font colors
#define BLACK_FORE 8452
#define WHITE_FORE 63421

int are2DArraysEqual(int arr1[BOARD_SIZE][BOARD_SIZE], int arr2[BOARD_SIZE][BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            if (arr1[i][j] != arr2[i][j])
            {
                return 0; // Arrays are not equal
            }
        }
    }
    return 1; // Arrays are equal
}

void copy2DArray(int source[BOARD_SIZE][BOARD_SIZE], int destination[BOARD_SIZE][BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            destination[i][j] = source[i][j];
        }
    }
}

typedef struct
{
    int board[BOARD_SIZE][BOARD_SIZE];
    int score;
    int best_score;
    int file_handle;
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
    game->score = 0;

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

    int handle = ti_Open("data2048", "r+");
    int data[1] = {0};

    if (handle)
    {
        ti_Read(data, sizeof(int), 1, handle);
        game->file_handle = ti_Open("data2048", "w+");
    }
    else
    {
        game->file_handle = ti_Open("data2048", "w+");
        ti_Write(data, sizeof(int), 1, game->file_handle);
    }

    game->best_score = data[0];
}

void save_game(Game *game)
{
    int data[1] = {game->best_score};
    ti_Write(data, sizeof(int), 1, game->file_handle);
}

void clear()
{
    int size = BOARD_SIZE * (TILE_SIZE + TILE_PADDING) + 10 + TILE_PADDING;
    gfx_SetColor(15);
    RoundFillRectangle(
        BOARD_X - 10,
        BOARD_Y - 10,
        size,
        size,
        7);
}

void render(Game *game)
{
    int size = BOARD_SIZE * (TILE_SIZE + TILE_PADDING) + 10 + TILE_PADDING;
    gfx_SetColor(15);
    RoundFillRectangle(size + 40, 75, 60, 30, 6);
    RoundFillRectangle(size + 40, 125, 60, 30, 6);
    gfx_SetTextFGColor(0);
    gfx_PrintStringXY("Score", size + 50, 80);
    gfx_PrintStringXY("Best", size + 55, 130);

    char score[10];
    sprintf(score, "%d", game->score);

    char best_score[10];
    sprintf(best_score, "%d", game->best_score);
    gfx_SetTextFGColor(13);
    gfx_PrintStringXY(score, size + 70 - gfx_GetStringWidth(score) / 2, 95);
    gfx_PrintStringXY(best_score, size + 70 - gfx_GetStringWidth(best_score) / 2, 145);

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            int tile = game->board[i][j];
            int x = j * (TILE_SIZE + TILE_PADDING) + BOARD_X;
            int y = i * (TILE_SIZE + TILE_PADDING) + BOARD_Y;
            int cx = x + TILE_SIZE / 2;
            int cy = y + TILE_SIZE / 2;

            if (tile == 0)
                gfx_SetColor(0);
            else if (tile == 2)
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
            else
                gfx_SetColor(12);

            RoundFillRectangle(x, y, TILE_SIZE, TILE_SIZE, 7);
            if (tile != 0)
            {
                // Set the correct font color
                if (tile == 2 || tile == 4)
                {
                    gfx_SetTextFGColor(14);
                }
                else
                {
                    gfx_SetTextFGColor(13);
                }

                // Convert the tile to a string
                char str[6];
                sprintf(str, "%d", tile);

                // Calculate location
                int sx = cx - gfx_GetStringWidth(str) / 2;
                int sy = cy - 2;
                gfx_PrintStringXY(str, sx, sy);
            }
        }
    }
}

void move_left(Game *game)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        // Remove all zeros
        int j = 0;
        for (int k = 0; k < BOARD_SIZE; k++)
        {
            if (game->board[i][k] != 0)
            {
                game->board[i][j++] = game->board[i][k];
            }
        }
        for (; j < BOARD_SIZE; j++)
        {
            game->board[i][j] = 0;
        }

        // Merge adjacent identical tiles
        for (int k = 0; k < BOARD_SIZE - 1; k++)
        {
            if (game->board[i][k] == game->board[i][k + 1])
            {
                game->board[i][k] *= 2;
                game->board[i][k + 1] = 0;
                game->score += game->board[i][k];
                save_game(game);
                k++;
            }
        }

        // Remove all zeros again
        j = 0;
        for (int k = 0; k < BOARD_SIZE; k++)
        {
            if (game->board[i][k] != 0)
            {
                game->board[i][j++] = game->board[i][k];
            }
        }
        for (; j < BOARD_SIZE; j++)
        {
            game->board[i][j] = 0;
        }
    }
}

void flip_horizontal(int board[BOARD_SIZE][BOARD_SIZE])
{
    int temp[BOARD_SIZE];

    for (int j = 0; j < BOARD_SIZE / 2; j++)
    {
        for (int i = 0; i < BOARD_SIZE; i++)
        {
            temp[i] = board[i][j];
            board[i][j] = board[i][BOARD_SIZE - j - 1];
            board[i][BOARD_SIZE - j - 1] = temp[i];
        }
    }
}

void transpose(int board[BOARD_SIZE][BOARD_SIZE])
{
    int temp[BOARD_SIZE][BOARD_SIZE];

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            temp[j][i] = board[i][j];
        }
    }

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            board[i][j] = temp[i][j];
        }
    }
}

int move(Game *game, int move)
{
    // Store the current state of the board to check for changes
    int original_state[BOARD_SIZE][BOARD_SIZE];
    copy2DArray(game->board, original_state);

    switch (move)
    {
    case sk_Up:
        transpose(game->board);
        move_left(game);
        transpose(game->board);
        break;
    case sk_Down:
        transpose(game->board);
        flip_horizontal(game->board);
        move_left(game);
        flip_horizontal(game->board);
        transpose(game->board);
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

    if (moved)
    {
        // If the board has changed, add a random tile and update the display
        add_random_tile(game);
        render(game);
    }

    return moved;
}

int main(void)
{
    gfx_Begin();

    // Set the color palette
    gfx_palette[0] = gfx_RGBTo1555(214, 203, 189); // 0
    gfx_palette[1] = gfx_RGBTo1555(231, 227, 214); // 2
    gfx_palette[2] = gfx_RGBTo1555(231, 223, 198); // 4
    gfx_palette[3] = gfx_RGBTo1555(239, 174, 123); // 8
    gfx_palette[4] = gfx_RGBTo1555(247, 146, 99);  // 16
    gfx_palette[5] = gfx_RGBTo1555(247, 125, 97);  // 32
    gfx_palette[6] = gfx_RGBTo1555(247, 97, 66);   // 64

    gfx_palette[7] = gfx_RGBTo1555(239, 203, 121); // 128
    gfx_palette[8] = gfx_RGBTo1555(239, 199, 105); // 256
    gfx_palette[9] = 30506;                        // 512
    gfx_palette[10] = 30471;                       // 1024
    gfx_palette[11] = 30469;                       // 2048
    gfx_palette[12] = 7398;                        // > 2048

    gfx_palette[13] = gfx_RGBTo1555(247, 243, 239); // White Fore
    gfx_palette[14] = gfx_RGBTo1555(115, 109, 99);  // Black Fore
    gfx_palette[15] = gfx_RGBTo1555(182, 170, 158); // Board Back

    srand(time(NULL));
    Game game;
    init_game(&game);
    clear();
    render(&game);

    int running = 1;
    while (running)
    {
        int keycode = os_GetCSC();
        if (keycode <= 0x04 && keycode >= 0x01)
        {
            move(&game, keycode);
        }
        else if (keycode == sk_Clear)
        {
            running = 0;
        }
    }

    // Exit
    gfx_End();
    ti_Close(game.file_handle);
    return 0;
}
