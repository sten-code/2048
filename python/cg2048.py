# Casio fx-CG50

import os
import random


TILE_WIDTH = 5


def center_string(input_string: str, width: int, space: str = " "):
    # Calculate the number of spaces needed
    spacing = max(0, width - len(input_string))
    
    # Calculate padding for both sides
    left_padding = spacing // 2
    right_padding = spacing - left_padding
    
    # Center the string within the field
    centered_string = space * left_padding + input_string + space * right_padding
    
    return centered_string


def deep_copy_2d_list(lst):
    if not isinstance(lst, list):
        return lst
    return [deep_copy_2d_list(item) for item in lst]

class Game:
    def __init__(self, size: int = 4):
        self.out = ""
        self.board = [[0] * size for _ in range(size)]
        self.add_random_tile()
        self.add_random_tile()

    def render(self):
        os.system('cls(0)')

        print()
        for row in self.board:
            line = ""
            for tile in row:
                # If its 0 then the tile is empty so don't render a number
                tile_str = str(tile) if tile > 0 else "."
                line += " " + center_string(tile_str, TILE_WIDTH)

            # Render all the rows with a spacing below
            print(line)
            print()

        # Render additional information
        print(self.out)
        self.out = ""
        if self.is_game_over():
            print("Game Over!")
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

    def move(self, move: str) -> bool:
        # deep copy the 2d list because otherwise the 2nd dimension won't be copied with the normal .copy function
        start = deep_copy_2d_list(self.board)

        # rotate the board so its easier to calculate moves with 1 function
        if move == "right":
            self.flip_horizontal()
            self._move_left()
            self.flip_horizontal()
        elif move == "left":
            self._move_left()
        elif move == "up":
            self.flip_vertical()
            self._move_left()
            self.flip_vertical()
        elif move == "down":
            self.flip_vertical()
            self.flip_horizontal()
            self._move_left()
            self.flip_horizontal()
            self.flip_vertical()

        # Check if the move actually did something to check if the move was valid
        success = start != self.board
        if success:
            self.add_random_tile()

        return success

    def flip_horizontal(self):
        self.board = [row[::-1] for row in self.board]

    def flip_vertical(self):
        self.board = [[row[i] for row in self.board] for i in range(len(self.board[0]))]

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


game = Game()
game.render()

running = True
while running:
    inp = input()
    if inp == "8":
        game.move("up")
    elif inp == "4":
        game.move("left")
    elif inp == "5":
        game.move("down")
    elif inp == "6":
        game.move("right")
    elif inp == "0":
        game = Game()
    elif inp == "1":
        running = False
        break
    game.render()
