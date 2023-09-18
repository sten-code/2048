#include "game.h"
#include <graphx.h>
#include <ti/getcsc.h>
#include <string.h>
#include <fileioc.h>
#include "graphics.h"

// Private methods
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
                if (game->score > game->best_score)
                {
                    game->best_score = game->score;
                }
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

// Public methods
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

void reset_game(Game *game)
{
    game->score = 0;
    // for (size_t i = 0; i < BOARD_SIZE * BOARD_SIZE; i++)
    // {
    //     *((int*)game->board + i) = 0;
    // }
    memset(game->board, 0, sizeof(game->board));

    // Add two random tiles at the beginning
    add_random_tile(game);
    add_random_tile(game);
}

void init_game(Game *game)
{
    reset_game(game);

    int handle = ti_Open("data2048", "r");
    int data[18];

    if (handle)
    {
        ti_Read(data, sizeof(int), 18, handle);
    }
    else
    {
        memset(data, 0, sizeof(data));
        int handle = ti_Open("data2048", "w+");
        ti_Write(data, sizeof(int), 18, handle);
    }

    game->best_score = data[0];
    game->score = data[1];
    int allZero = 1;
    for (int i = 0; i < 16; i++)
    {
        game->board[i / 4][i % 4] = data[i + 2];
        if (data[i + 1] != 0)
        {
            allZero = 0;
        }
    }

    if (allZero)
    {
        // Add two random tiles at the beginning
        add_random_tile(game);
        add_random_tile(game);
    }
}

void save_game(Game *game)
{
    int handle = ti_Open("data2048", "w+");
    int data[18] = {game->best_score, game->score};

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            data[i * 4 + j + 2] = game->board[i][j];
    }

    ti_Write(data, sizeof(int), 18, handle);
    ti_Close(handle);
}

int is_game_over(Game *game)
{
    int i, j;

    // Check for empty cells and horizontal adjacent duplicates
    for (i = 0; i < BOARD_SIZE; i++)
    {
        j = 0;
        while (j < BOARD_SIZE - 1)
        {
            if (game->board[i][j] == 0 || game->board[i][j] == game->board[i][j + 1])
            {
                return false;
            }
            j += 2; // Unroll the loop by 2
        }
        if (j == BOARD_SIZE - 1 && game->board[i][j] == 0)
        {
            return false;
        }
    }

    // Check for vertical adjacent duplicates
    for (j = 0; j < BOARD_SIZE; j++)
    {
        i = 0;
        while (i < BOARD_SIZE - 1)
        {
            if (game->board[i][j] == game->board[i + 1][j])
            {
                return false;
            }
            i += 2; // Unroll the loop by 2
        }
    }

    return true;
}

void render_background()
{
    int size = BOARD_SIZE * (TILE_SIZE + TILE_PADDING) + TILE_PADDING;
    gfx_SetColor(15);
    gfx_RoundFillRectangle(
        BOARD_X - TILE_PADDING,
        BOARD_Y - TILE_PADDING,
        size,
        size,
        10);
}

void render(Game *game)
{
    int size = BOARD_SIZE * (TILE_SIZE + TILE_PADDING) + 10 + TILE_PADDING;
    gfx_SetColor(15);
    gfx_RoundFillRectangle(size + 40, 75, 60, 30, 6);
    gfx_RoundFillRectangle(size + 40, 125, 60, 30, 6);
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
            {
                gfx_SetColor(0);
            }
            else if (tile <= 2048)
            {
                int temp = tile;
                int position = 0;
                while ((temp & 1) == 0)
                {
                    temp >>= 1;
                    position++;
                }
                gfx_SetColor(position);
            }
            else
            {
                gfx_SetColor(12);
            }

            gfx_RoundFillRectangle(x, y, TILE_SIZE, TILE_SIZE, 7);
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

    if (is_game_over(game))
    {
        gfx_SetColor(0);
        int width = BOARD_SIZE * (TILE_SIZE + TILE_PADDING) - TILE_PADDING;
        int height = (BOARD_SIZE - 2) * (TILE_SIZE + TILE_PADDING) - TILE_PADDING;
        int y = BOARD_Y + TILE_SIZE + TILE_PADDING;
        gfx_RoundFillRectangle(
            BOARD_X,
            y,
            width,
            height,
            6);
        gfx_SetTextFGColor(13);
        gfx_SetTextScale(2, 2);
        gfx_PrintStringXY("Game Over!", BOARD_X + width / 2 - gfx_GetStringWidth("Game Over!") / 2, y + height / 2 - 7);
        gfx_SetTextScale(1, 1);
        while (!os_GetCSC())
            ;
        reset_game(game);
        render_background();
        render(game);
    }
}

int move(Game *game, int move)
{
    // Store the current state of the board to check for changes
    int original_state[BOARD_SIZE][BOARD_SIZE];
    memcpy(original_state, game->board, sizeof(game->board));

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
    int moved = memcmp(original_state, game->board, sizeof(game->board)) != 0;
    if (moved)
    {
        // If the board has changed, add a random tile and update the display
        add_random_tile(game);
        render(game);
    }

    return moved;
}
