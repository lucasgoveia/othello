
#ifndef OTHELLO_GEN_DATA_H
#define OTHELLO_GEN_DATA_H

#include <stdint.h>
#include <stdio.h>
#include <pthread.h>

#include "../core/board/board.h"
#include "../core/player/player.h"
#include "../engine/engine.h"
#include "../engine/tt/tt.h"

typedef struct {
    int games_cnt;
    int32_t positions_cnt;
    char *games_dump_file_name;
} GenDataInfo;

typedef enum {
    RANDOM,
    ALPHA_BETA_1,
    ALPHA_BETA_3,
    ALPHA_BETA_5,
    ALPHA_BETA_7,
    ALPHA_BETA_9,
} PlayerType;

typedef struct {
    Board *board;
    Move mv;
} BoardAndMove;

void gen_data();


#endif //OTHELLO_GEN_DATA_H