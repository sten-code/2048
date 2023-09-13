import random
import os
from enum import Enum
from pynput import keyboard
import sys

class Move(Enum):
    up = "up"
    left = "left"
    down = "down"
    right = "right"


def clear_screen():
    os.system("cls" if os.name == "nt" else "clear")


class Game:
    def __init__(self, size: int = 4):
        self.board = [[0] * size for _ in range(size)]
        self.history = []
        self.add_random_tile()
        self.add_random_tile()

    def render(self):
        clear_screen()
        print("┌─────┬─────┬─────┬─────┐")
        for i, row in enumerate(self.board):
            text = ""
            for tile in row:
                text += "│" + (str(tile) if tile > 0 else "").center(5)
            print(text + "│")
            if i < len(self.board) - 1:
                print("├─────┼─────┼─────┼─────┤")
            else:
                print("└─────┴─────┴─────┴─────┘")
        print()
        if self.is_game_over():
            print("Game Over!")
            exit()

    def get_empty_cells(self) -> list[tuple[int, int]]:
        return [(i, j) for i in range(len(self.board)) for j in range(len(self.board[i])) if self.board[i][j] == 0]

    def add_random_tile(self):
        empty_cells = self.get_empty_cells()
        if empty_cells:
            i, j = random.choice(empty_cells)
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
        start = self.board
        
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

        success = start != self.board
        if success:
            self.add_random_tile()
            self.history.append(start)

        return success

    def undo(self):
        if len(self.history) > 0:
            self.board = self.history.pop()

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

class Bot:
    def __init__(self, game: Game) -> None:
        self.game = game
    
    def next(self) -> Move:
        return self.minimax(4, -999999, 999999)[1]

    def minimax(self, depth: int, alpha: int, beta: int) -> tuple[int, Move]:
        if self.game.is_game_over():
            return -999999, Move.up
        if depth == 0:
            return 0, Move.up
        
        best_eval = -999999
        best_move = Move.up
        for move in [Move.up, Move.left, Move.down, Move.right]:
            self.game.move(move)
            eval = self.minimax(depth - 1, alpha, beta)[0]
            self.game.undo()
            
            if eval > best_eval:
                best_eval = eval
                best_move = move

            alpha = max(alpha, eval)
            if beta <= alpha:
                break
        return best_eval, best_move

def main():
    game = Game()
    # bot = Bot(game)
    # while True:
    #     game.render()
    #     game.move(bot.next())
    #     input()
        
    game.render()

    def on_press(key):
        if key == keyboard.Key.esc:
            listener.stop()
            return

        if key == keyboard.Key.up:
            game.move(Move.up)
        if key == keyboard.Key.down:
            game.move(Move.down)
        if key == keyboard.Key.left:
            game.move(Move.left)
        if key == keyboard.Key.right:
            game.move(Move.right)
        if key == keyboard.Key.backspace:
            game.undo()
        game.render()
            

    with keyboard.Listener(on_press=on_press) as listener:
        listener.join()


if __name__ == "__main__":
    main()
