//
// Created by govei on 24/09/2022.
//

#ifndef OTHELLO_BOARD_H
#define OTHELLO_BOARD_H

#include <stdlib.h>

#include "../bitboard/bitboard.h"
#include "../player/player.h"
#include "../defs.h"

#define LEFT_RIGHT_MAKS 0x7e7e7e7e7e7e7e7eULL
#define TOP_BOTTOM_MASK 0x00ffffffffffff00ULL
#define CORNER_MASK (LEFT_RIGHT_MAKS & TOP_BOTTOM_MASK)

#define PASS_MOVE_CODE 64

typedef struct board {
    Bitboard bbs[2];
    Player turn;
    int pass_move_count;
    struct board *prev;
} Board;

typedef struct {
    Bitboard mask;
    int offset;
} DeltaInfo;

typedef struct {
    uint8_t code;
    Player player;
} Move;

typedef struct {
    Move *moves;
    uint8_t size;
} MoveList;

const DeltaInfo DELTAS[8];

Board *board_default();

void board_print(Board *board);

Bitboard board_us_bb(Board *board);
Bitboard board_them_bb(Board *board);
Bitboard board_empty_bb(Board *board);

MoveList *board_legal_moves(Board *board);
void board_apply_move(Board *board, Move mv);
void board_undo_move(Board *board);

Board *board_copy(Board *board);

#endif //OTHELLO_BOARD_H
