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
    short column;
    long long score;
};

typedef struct MIN_ARGS{
    short** board;
    short depth;
    long long alpha;
    long long beta;
    short player;
    long long score;
}MIN_ARGS;

//function prototypes

int bestmove(char* board_state);

short** create_board();

short play_move(short** board);

void flush_stdin();

long long evaluate_window(short window[4], short player);

short is_legal_move(short** board, short column);

short** place_chip(short** board, short column, short player);

short isdraw(short** board);

short iswin(short** board);

short** reset_board(short** board);

long long evaluate_window(short window[4], short player);

long long evaluate_board(short** board, short player);

short** copy_board(short** board);

void free_board(short** board);

struct minimax_return minimax(short** board, short depth, long long alpha, long long beta, short player);

void* minimax_mt(void* minimax_args);

#endif //CONNECT4_ENGINE_H
