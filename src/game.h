#ifndef GAME
#define GAME

#include "defines.h"

typedef struct
{
    int board[BOARD_SIZE][BOARD_SIZE];
    int score;
    int best_score;
} Game;

void add_random_tile(Game *game);
void reset_game(Game *game);
void init_game(Game *game);
void save_game(Game *game);
int is_game_over(Game *game);
void render_background();
void render(Game *game);
int move(Game *game, int move);

#endif