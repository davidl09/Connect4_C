//
// Created by dalae on 2023-03-12.
//

#ifndef CONNECT4_C4_GAME_ENGINE_H
#define CONNECT4_C4_GAME_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6

#define AI 2
#define HUMAN 1
#define EMPTY 0

#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)


struct minimax_return{
    int column;
    long long score;
};

typedef struct MIN_ARGS{
    int** board;
    int depth;
    long long alpha;
    long long beta;
    int player;
    long long score;
}MIN_ARGS;

//function prototypes

int bestmove(char* board_state);

int** create_board();

int play_move(int** board);

void flush_stdin();

long long evaluate_window(int window[4], int player);

int is_legal_move(int** board, int column);

int** place_chip(int** board, int column, int player);

int isdraw(int** board);

int iswin(int** board);

int** reset_board(int** board);

long long evaluate_window(int window[4], int player);

long long evaluate_board(int** board, int player);

int** copy_board(int** board);

void free_board(int** board);

struct minimax_return minimax(int** board, int depth, long long alpha, long long beta, int player);

void* minimax_mt(void* minimax_args);

#endif //CONNECT4_ENGINE_H
