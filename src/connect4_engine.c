//
// Created by dalae on 2023-03-12.
//

#include "connect4_engine.h"
#include <pthread.h>
#include <stdlib.h>


int bestmove(char* board_state){
    //first 42 chars of board state are '0', '1', or '2', for human, ai, empty. 43rd char is player to move, either '1' or '2'

    int** board = create_board();
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            board[i][j] = (int)board_state[BOARD_WIDTH * i + j] - (int)'0';
        }
    }

    int to_play = (board_state[42] == '1' ? HUMAN : AI);
    int depth = (int)(board_state[43] - '0');
    int move;
    MIN_ARGS min_args[BOARD_WIDTH];
    pthread_t threads[BOARD_WIDTH];

    for (int i = 0; i < BOARD_WIDTH; ++i) {
        min_args[i] = (MIN_ARGS){copy_board(board), depth, -10000000000, 10000000000, 3-to_play, -10000000000};

        if(place_chip(min_args[i].board, i, to_play)){
            pthread_create(&threads[i], NULL, minimax_mt, (void*)&min_args[i]);

        }else threads[i] = 0;
    }

    for (int i = 0; i < BOARD_WIDTH; ++i) {
        if(threads[i] != 0)
            pthread_join(threads[i], NULL);
    }

    long long min_score = -10000000000;

    for (int i = 0; i < BOARD_WIDTH; ++i) {
        if(min_args[i].score > min_score){
            move = i;
            min_score = min_args[i].score;
        }
    }

    free_board(board);

    return (int)move;
}

int** create_board(){
    //allocate memory for the board and initialize values to 0
    int** board = malloc(BOARD_HEIGHT*sizeof(int*));
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        board[i] = malloc(BOARD_WIDTH * sizeof(int));
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            board[i][j] = EMPTY;
        }
    }
    return board;
}


int is_legal_move(int** board, int column){
    if(board[0][column] == 0 && (7 > column && column >= 0))
        return 1;
    else return 0;
}

void flush_stdin(){
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int play_move(int** board){
    int move;
    printf("Enter the index of the column in which you would like to play, in a range of 1-7\n");
    scanf("%d", &move);
    move--;
    if(is_legal_move(board, move) == 1) {
        flush_stdin();
        return move;
    }
    printf("That was not a legal move, please try again\n");
    flush_stdin();
    return play_move(board);
}

int** place_chip(int** board, int column, int player){
    if(!is_legal_move(board, column)){ //check if column is full
        return 0;
    }
    for (int i = 0; i < BOARD_HEIGHT; ++i) { //search for current height of chips in given column
        if(board[i][column] != 0){ //add chip above last added chip
            board[i-1][column] = player;
            return board;
        }
    }
    board[BOARD_HEIGHT-1][column] = player; //add chip at bottom if no chips in column
    return board;
}

int isdraw(int** board){
    int draw = 1;
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        if(board[0][i] == 0)
            return 0;
    }
    return draw;
}

int iswin(int** board){
    if(isdraw(board)){
        return 3;
    }
    for (int i = 0; i < BOARD_HEIGHT; ++i) { //check rows
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            if (board[i][j] == board[i][j + 1] && board[i][j + 1] == board[i][j + 2] && board[i][j + 2] == board[i][j + 3] && board[i][j] != 0) {
                return board[i][j];
            }
        }
    }
    for (int i = 0; i < BOARD_HEIGHT - 3; ++i) { //check columns
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            if (board[i][j] == board[i + 1][j] && board[i+1][j] == board[i + 2][j] && board[i + 2][j] == board[i + 3][j] && board[i][j] != 0) {
                return board[i][j];
            }
        }
    }
    for (int i = 0; i < BOARD_HEIGHT - 3; ++i) { //check downward diagonals
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            if(board[i][j] == board[i+1][j+1] && board[i+1][j+1] == board[i+2][j+2] && board[i+2][j+2] == board[i+3][j+3] && board[i][j] != 0){
                return board[i][j];
            }
        }
    }
    for (int i = 3; i < BOARD_HEIGHT; ++i) { //check upward diagonals
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            if(board[i][j] == board[i-1][j+1] && board[i-1][j+1] == board[i-2][j+2] && board[i-2][j+2] == board[i-3][j+3] && board[i][j] != 0){
                return board[i][j];
            }
        }
    }
    return 0;
}

int** reset_board(int** board){
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            board[i][j] = 0;
        }
    }
    return board;

}



long long evaluate_window(int window[4], int player) {
    int opponent = 3 - player;
    int player_count = 0;
    int opponent_count = 0;
    for (int i = 0; i < 4; ++i) { //count chips in window
        if (window[i] == player) {
            player_count++;
        } else if (window[i] == opponent) {
            opponent_count++;
        }
    }
    if (player_count == 4)
        return  10000000000;
    if (opponent_count == 4)
        return  -(10000000000);
    if (opponent_count == 0)
        return player_count * player_count * 100;
    if (player_count == 0)
        return -(opponent_count * opponent_count * 100);
    return player_count - opponent_count;
}

long long evaluate_board(int** board, int player){
    long long score = 0;
    int window[4];
    long long window_score;
    for (int i = 0; i < BOARD_HEIGHT; ++i) { //check rows
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                window[k] = board[i][j + k];
            }
            window_score = evaluate_window(window, player);
            if(llabs(window_score) >= 10000000000)
                return window_score;
            score += window_score;
        }
    }
    for (int i = 0; i < BOARD_HEIGHT - 3; ++i) { //check columns
        for (int j = 0; j < BOARD_WIDTH; ++j) {
            for (int k = 0; k < 4; ++k) {
                window[k] = board[i+k][j];
            }
            window_score = evaluate_window(window, player);
            if(llabs(window_score) >= 10000000000)
                return window_score;
            score += window_score;
        }
    }
    for (int i = 0; i < BOARD_HEIGHT - 3; ++i) { //check upward diagonals
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                window[k] = board[i+k][j+k];
            }
            window_score = evaluate_window(window, player);
            if(llabs(window_score) >= 10000000000)
                return window_score;
            score += window_score;
        }
    }
    for (int i = 3; i < BOARD_HEIGHT; ++i) { //check downward diagonals
        for (int j = 0; j < BOARD_WIDTH - 3; ++j) {
            for (int k = 0; k < 4; ++k) {
                window[k] = board[i-k][j+k];
            }
            window_score = evaluate_window(window, player);
            if(llabs(window_score) >= 10000000000)
                return window_score;
            score += window_score;
        }
    }
    return score;
}

int** copy_board(int** board){
    //returns pointer to a copy of values in board array
    int** new_board = malloc(sizeof(int*) * BOARD_HEIGHT); //allocate memory for board copy
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        new_board[i] = malloc(sizeof(int) * BOARD_WIDTH);
        memcpy(new_board[i], board[i], BOARD_WIDTH*sizeof(board[i][0]));
    }
    return new_board;
}

void free_board(int** board){
    for (int i = 0; i < BOARD_HEIGHT; ++i) {
        free(board[i]);
    }
    free(board);
}

struct minimax_return minimax(int** board, int depth, long long alpha, long long beta, int player){
    struct minimax_return best_move;
    best_move.column = -1;
    best_move.score = player == AI ? -10000000000 : 10000000000;
    int win_state = iswin(board);

    //stopping conditions
    if(win_state == 2){
        best_move.score =  (long long)round(pow(10, depth + 4));
        free_board(board);
        return best_move;
    }
    if(win_state == 1){
        best_move.score =  -10000000000;
        free_board(board);
        return best_move;
    }
    if(win_state == 3){
        best_move.score = 0;
        free_board(board);
        return best_move;
    }
    if(depth == 0){
        best_move.column = -1;
        best_move.score = evaluate_board(board, AI);
        free_board(board);
        return best_move;
    }

    long long score;
    int** new_board;

    //check for wins before calling minimax
    for (int i = 0; i < BOARD_WIDTH; ++i) {
        new_board = copy_board(board);
        if(place_chip(new_board, i, player) != 0) {
            if(iswin(new_board) == AI){
                best_move.score = (long long)round(pow(10, depth + 4));
                best_move.column = i;
                free_board(board);
                free_board(new_board);
                return best_move;
            }
            if(iswin(new_board) == HUMAN){
                best_move.score = -10000000000;
                best_move.column = i;
                free_board(board);
                free_board(new_board);
                return best_move;
            }
        }
        free_board(new_board);
    }
    //if no wins found at this depth continue searching

    for (int i = 0; i < BOARD_WIDTH; ++i) {
        new_board = copy_board(board);
        if(place_chip(new_board, i, player) != 0) {
            score = minimax(new_board, depth - 1, alpha, beta, 3 - player).score;
            if (player == AI) { //maximising
                if (score > best_move.score) {
                    best_move.score = score;
                    best_move.column = i;
                }
                alpha = max(alpha, best_move.score);
                if (alpha >= beta)
                    break;
            } else {
                if (score < best_move.score) {
                    best_move.score = score;
                    best_move.column = i;
                }
                beta = min(beta, best_move.score);
                if (alpha >= beta)
                    break;
            }
        }
    }
    free_board(board);
    return best_move;
}

void* minimax_mt(void* args){ //receive general args in separate execution thread
    MIN_ARGS* ret_args = (MIN_ARGS*)args;
    ret_args->score = minimax(ret_args->board, ret_args->depth, ret_args->alpha, ret_args->beta, ret_args->player).score;
    pthread_exit(NULL);
}
