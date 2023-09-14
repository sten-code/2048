# Standard pc version

import random
import os
from enum import Enum
import copy
import keyboard
import json
import math


def get_tile_color(value: int) -> tuple[str, str]:  # fore, back
    color_map = {
        0: ("#CCC0B2", "#CCC0B2"),
        2: ("#222222", "#EEE4DA"),
        4: ("#222222", "#EDE0C8"),
        8: ("#F9F6F2", "#F2B179"),
        16: ("#F9F6F2", "#F59563"),
        32: ("#F9F6F2", "#F67C5F"),
        64: ("#F9F6F2", "#F65E3B"),
        128: ("#F9F6F2", "#EDCF72"),
        256: ("#F9F6F2", "#EDCC61"),
        512: ("#F9F6F2", "#EDC850"),
        1024: ("#F9F6F2", "#EDC53F"),
        2048: ("#F9F6F2", "#EDC22E"),
    }

    return color_map.get(value, ("#F9F6F2", "#3C3A32"))


class Move(Enum):
    up = "up"
    left = "left"
    down = "down"
    right = "right"


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


def color_text(font_color_hex, background_color_hex):
    # Convert hex colors to RGB
    fore_r = int(font_color_hex[1:3], 16)
    fore_g = int(font_color_hex[3:5], 16)
    fore_b = int(font_color_hex[5:7], 16)
    back_r = int(background_color_hex[1:3], 16)
    back_g = int(background_color_hex[3:5], 16)
    back_b = int(background_color_hex[5:7], 16)

    # Create ANSI escape codes for setting font and background colors
    fore_colorcode = f"\x1b[38;2;{fore_r};{fore_g};{fore_b}m"
    back_colorcode = f"\x1b[48;2;{back_r};{back_g};{back_b}m"

    return f"{back_colorcode}{fore_colorcode}"


class Game:
    def __init__(self, size: int = 4):
        self.out = ""
        self.board = [[0] * size for _ in range(size)]
        self.add_random_tile()
        self.add_random_tile()

    def render(self):
        clear_screen()
        border = color_text("#BBADA0", "#BBADA0")
        print(border + "                                  ")
        for row in self.board:
            row1 = ""
            row2 = ""
            row3 = ""

            reset = "\x1b[0m"
            bold = "\x1b[1m"
            for tile in row:
                tile_str = str(tile) if tile > 0 else ""
                fore, back = get_tile_color(tile)
                c = color_text(fore, back)
                row1 += f"{border}  {c}      {reset}"
                row2 += f"{border}  {c}{bold}{tile_str.center(6)}{reset}"
                row3 += f"{border}  {c}      {reset}"

            print(row1 + border + "  ")
            print(row2 + border + "  ")
            print(row3 + border + "  ")
            print(border + "                                  " + reset)
        print(self.out)
        self.out = ""
        if self.is_game_over():
            print("Game Over!")
            os.remove("save.json")
            exit()

    def get_empty_cells(self) -> list[tuple[int, int]]:
        return [(i, j) for i in range(len(self.board)) for j in range(len(self.board[i])) if self.board[i][j] == 0]

    def add_random_tile(self):
        empty_cells = self.get_empty_cells()
        if empty_cells:
            # choose a random empty cell to put the new one
            i, j = random.choice(empty_cells)

            # 90% chance for a 2, 10% chance for a 4
            self.board[i][j] = 2 if random.random() < 0.9 else 4

    def _move_left(self):
        for row in self.board:
            # Remove all zeros
            while 0 in row:
                row.remove(0)

            # Merge adjacent identical tiles
            for i in range(len(row) - 1):
                if row[i] == row[i + 1]:
                    row[i] *= 2
                    row[i + 1] = 0

            # Fill the row with zeros again
            while 0 in row:
                row.remove(0)
            row.extend([0] * (len(self.board) - len(row)))

    def move(self, move: Move) -> bool:
        # deep copy the 2d list because otherwise the 2nd dimension won't be copied with the normal .copy function
        start = copy.deepcopy(self.board)

        # rotate the board so its easier to calculate moves with 1 function
        match move:
            case Move.right:
                self.flip_horizontal()
                self._move_left()
                self.flip_horizontal()
            case Move.left:
                self._move_left()
            case Move.up:
                self.flip_vertical()
                self._move_left()
                self.flip_vertical()
            case Move.down:
                self.flip_vertical()
                self.flip_horizontal()
                self._move_left()
                self.flip_horizontal()
                self.flip_vertical()

        # Check if the move actually did something to check if the move was valid
        success = start != self.board
        if success:
            self.add_random_tile()
            with open("save.json", "w") as f:
                json.dump(self.board, f)

        return success

    def flip_horizontal(self):
        self.board = [row[::-1] for row in self.board]

    def flip_vertical(self):
        self.board = [[row[i] for row in self.board]
                      for i in range(len(self.board[0]))]

    def is_game_over(self):
        for i in range(len(self.board)):
            for j in range(len(self.board[i])):
                if self.board[i][j] == 0:
                    return False

                if i > 0 and self.board[i][j] == self.board[i - 1][j]:
                    return False

                if i < len(self.board) - 1 and self.board[i][j] == self.board[i + 1][j]:
                    return False

                if j > 0 and self.board[i][j] == self.board[i][j - 1]:
                    return False

                if j < len(self.board[i]) - 1 and self.board[i][j] == self.board[i][j + 1]:
                    return False

        return True


class Bot:
    def __init__(self, game: Game) -> None:
        self.game = game
    
    def best_move(self, depth=4):
        def minimax(board, depth, alpha, beta, maximizing_player):
            if depth == 0 or not any(self.game.get_empty_cells()):
                return self.evaluate_board(board)

            if maximizing_player:
                max_eval = float('-inf')
                for move in [Move.up, Move.down, Move.left, Move.right]:
                    new_board = copy.deepcopy(board)
                    if self.game.move( move):
                        eval = minimax(new_board, depth - 1, alpha, beta, False)
                        max_eval = max(max_eval, eval)
                        alpha = max(alpha, eval)
                        if beta <= alpha:
                            break
                return max_eval
            else:
                min_eval = float('inf')
                empty_cells = self.game.get_empty_cells()
                for (i, j) in empty_cells:
                    for value in [2, 4]:
                        new_board = copy.deepcopy(board)
                        new_board[i][j] = value
                        eval = minimax(new_board, depth - 1, alpha, beta, True)
                        min_eval = min(min_eval, eval)
                        beta = min(beta, eval)
                        if beta <= alpha:
                            break
                return min_eval

        best_score = float('-inf')
        best_move = None
        alpha = float('-inf')
        beta = float('inf')

        for move in [Move.up, Move.down, Move.left, Move.right]:
            new_board = copy.deepcopy(self.game.board)
            if self.game.move(move):
                score = minimax(new_board, depth - 1, alpha, beta, False)
                if score > best_score:
                    best_score = score
                    best_move = move
                alpha = max(alpha, score)

        return best_move

    def evaluate_board(self, board):
        # Weight matrix to evaluate the board
        weight_matrix = [
            [32768, 16384, 8192, 4096],
            [2048, 1024, 512, 256],
            [128, 64, 32, 16],
            [8, 4, 2, 1]
        ]

        # Monotonicity heuristic
        monotonicity_score = 0
        for i in range(3):
            for j in range(3):
                if board[i][j] >= board[i + 1][j]:
                    monotonicity_score += math.log2(board[i][j] + 1) - math.log2(board[i + 1][j] + 1)
                if board[j][i] >= board[j][i + 1]:
                    monotonicity_score += math.log2(board[j][i] + 1) - math.log2(board[j][i + 1] + 1)

        # Smoothness heuristic
        smoothness_score = 0
        for i in range(len(board)):
            for j in range(len(board[i])):
                if board[i][j] > 0:
                    value = math.log2(board[i][j])
                    neighbors = []

                    if i > 0:
                        neighbors.append(board[i - 1][j])
                    if i < len(board) - 1:
                        neighbors.append(board[i + 1][j])
                    if j > 0:
                        neighbors.append(board[i][j - 1])
                    if j < len(board[i]) - 1:
                        neighbors.append(board[i][j + 1])

                    for neighbor in neighbors:
                        smoothness_score -= abs(math.log2(neighbor) - value)

        score = 0

        for i in range(len(board)):
            for j in range(len(board[i])):
                tile = board[i][j]
                if tile == 0:
                    continue

                # Calculate the score based on the value of the tile and its position
                score += tile * weight_matrix[i][j]

        return score + monotonicity_score + smoothness_score

def main():
    game = Game()
    bot = Bot(game)
    try:
        with open("save.json", "r") as f:
            game.board = json.load(f)
    except FileNotFoundError:
        pass
    game.render()

    running = True
    while running:
        # event = keyboard.read_event()
        # if event.event_type == keyboard.KEY_DOWN:
        #     if event.name in Move.__members__:
        #         game.move(Move(event.name))
        #         game.render()
        #     elif event.name == "esc":
        #         running = False
        #     elif event.name == "r":
        #         game = Game()
        #         game.render()
        # elif event.event_type == keyboard.KEY_UP:
        #     pass
        move = bot.best_move()
        game.move(move)
        game.render()
        game.out += str(move)
        input()


if __name__ == "__main__":
    main()
