//
// Created by govei on 24/09/2022.
//

#ifndef OTHELLO_BOARD_H
#define OTHELLO_BOARD_H

#include "../bitboard/bitboard.h"
#include "../player/player.h"
#include "../defs.h"
#include <stdlib.h>

typedef struct {
    Bitboard bbs[2];
    Player turn;
} Board;

Board *board_default();

void board_print(Board *board);

#endif //OTHELLO_BOARD_H
