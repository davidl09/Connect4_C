from pygame import init, mouse, event, MOUSEMOTION, font, display, draw, MOUSEBUTTONDOWN, QUIT, time as ptime
import CONNECT4 as C4
from sys import argv
import ctypes
import pathlib


lib = pathlib.Path().absolute() / "lib/connect4.so"
c4_lib = ctypes.CDLL(lib)

c4_lib.bestmove.argtypes = [ctypes.c_char_p]
c4_lib.bestmove.restype = ctypes.c_int

try:
    depth = int(argv[1])
except:
    depth = 10


RED = (200, 10, 20)
BLUE = (20, 10, 220)
WHITE = (255, 255, 255)
GREEN = (11, 102, 35)
YELLOW = (180, 150, 0)
BLACK = (0, 0, 0)

EMPTY = 0
HUMAN_PLAYER = 1 #player
AI_PLAYER = 2 #AI

colours = [BLACK, RED, YELLOW]

SCREEN_WIDTH = 900
SCREEN_HEIGHT = 900


class Board:
    def __init__(self):
        self.height = 6
        self.width = 7
        self.values = [[0]*self.width for _ in range(self.height)]
        self.rad = min((SCREEN_HEIGHT-100)//6, (SCREEN_WIDTH-100)//7)//2 - 10
        self.x_edge = (SCREEN_WIDTH - self.width*self.rad)//2
        self.y_edge = (SCREEN_HEIGHT - self.height*self.rad)//2
        
        
    def get_draw_coord(self, i, j):
        row = range(0, SCREEN_HEIGHT, SCREEN_HEIGHT//8)[i+2]
        col = range(0, SCREEN_WIDTH, SCREEN_WIDTH//8)[j+1]
        return (col, row)
    
    
    def reset_board(self):
        for i in range(self.height):
            for j in range(self.width):
                self.values[i][j] = 0


    def add_chip(self, turn, col):
        for i in range(self.height):
            if self.values[i][col] != EMPTY:
                self.values[i - 1][col] = int(turn) + 1
                return True
            elif i == 5:
                self.values[i][col] = int(turn) + 1
                return True
            
    def encode_state(self, to_play):
        result = ""
        for i in range(self.height):
            for j in range(self.width):
                result = result + '1' if self.values[i][j] == HUMAN_PLAYER else result + '2' if self.values[i][j] == AI_PLAYER else result + '0'
        
        result = result + '1' if to_play == HUMAN_PLAYER else result + '2'

        result = result + chr(depth + 48)
        
        return result


class Game:
    def __init__(self):
        self.initialize_game()

    def initialize_game(self):
        init()
        self.myfont = font.SysFont("monospace", 75)
        self.screen = display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        display.set_caption('Connect4!')
        self.screen.fill(BLUE)
        display.flip()
        self.board = Board()
        self.ai_turn = False

    def draw_board(self, win):
        self.screen.fill(BLUE)
        for i in range(self.board.height):
            for j in range(self.board.width):
                draw.circle(self.screen, colours[self.board.values[i][j]], self.board.get_draw_coord(i, j), self.board.rad)
                draw.circle(self.screen, (1, 1, 1), self.board.get_draw_coord(i, j), self.board.rad, 3)
        if win and win != 'draw':
            label = self.myfont.render(str(f'Player {win} wins!'), 1, colours[win])
            self.screen.blit(label, (40, 10))
        if win == 'draw':
            label = self.myfont.render(str(f'It\'s a draw...'), 1, 'black')
            self.screen.blit(label, (40, 10))
        display.update()

    def handle_clicks(self, mouse):
        if self.screen.get_at(mouse) == BLACK:
            for i in range(self.board.width):
                if mouse[0] in range(self.board.get_draw_coord(0, i)[0] - self.board.rad, self.board.get_draw_coord(0, i)[0] + self.board.rad):
                    return self.board.add_chip(self.ai_turn, i)
                
    def flash_win(self, window, win):
        for i in range(4):
            for i, j in window:
                draw.circle(self.screen, 'white', self.board.get_draw_coord(i, j), self.board.rad)
            display.update()
            ptime.wait(500)
            for i, j in window:
                draw.circle(self.screen, colours[win], self.board.get_draw_coord(i, j), self.board.rad)
            display.update()
            ptime.wait(500)
                
        
    def run(self):
        clock = ptime.Clock()
        running = True
        while running == True:
            for thing in event.get():
                if thing.type == QUIT:
                    running = False
                if thing.type == MOUSEBUTTONDOWN:
                    if thing.button == 1 and self.handle_clicks(mouse.get_pos()):
                        win = C4.iswin(self.board.values, HUMAN_PLAYER)
                        if win:
                            if win == 'draw':
                                self.draw_board(win)
                                ptime.wait(1)
                            else:
                                self.draw_board(1)
                                ptime.wait(1)
                                self.flash_win(win, 2)
                            self.board.reset_board()
                        else: self.draw_board(False)
                        
                        board_str = self.board.encode_state(AI_PLAYER)
                        result = c4_lib.bestmove(board_str.encode())
                        self.board.add_chip(True, result)
            win = C4.iswin(self.board.values, AI_PLAYER)
            if win:
                if win == 'draw':
                    self.draw_board(win)
                    ptime.wait(1)
                else:
                    self.draw_board(2)
                    ptime.wait(1)
                    self.flash_win(win, 2)
                self.board.reset_board()
            self.draw_board(False)
            clock.tick(30)
      
game = Game()
game.run()