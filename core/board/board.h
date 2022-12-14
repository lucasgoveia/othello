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

#define PASS_MOVE 64
#define ILLEGAL_MOVE 65

typedef struct board {
    Bitboard bbs[2];
    Player turn;
    int pass_move_count;
    int moves_count;
    uint64_t hash;
    struct board *prev;
} Board;

typedef enum {
    SQ_OCC_WHITE,
    SQ_OCC_BLACK,
    SQ_OCC_EMPTY
} SquareOccupancyType;

typedef struct {
    Bitboard mask;
    int offset;
} DeltaInfo;

typedef uint8_t Move;

typedef struct {
    Move *moves;
    uint8_t size;
} MoveList;

extern const DeltaInfo DELTAS[8];

extern const char *MOVE_DISPLAY[65];

Board *board_default();

void board_print(Board *board);

Bitboard board_us_bb(Board *board);
Bitboard board_them_bb(Board *board);
Bitboard board_empty_bb(Board *board);

uint8_t board_legal_moves_count(Board *board);
MoveList *board_legal_moves(Board *board);

void board_apply_move(Board *board, Move mv);
void board_undo_move(Board *board);

Board *board_copy(Board *board);

void board_free(Board *board);

extern const uint64_t RAND_HASHES[192];
extern const uint64_t PLAYER_HASH;

#endif //OTHELLO_BOARD_H
