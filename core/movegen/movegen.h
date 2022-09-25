//
// Created by govei on 24/09/2022.
//

#ifndef OTHELLO_MOVEGEN_H
#define OTHELLO_MOVEGEN_H

#include "../bitboard/bitboard.h"
#include "../board/board.h"

#define LEFT_RIGHT_MAKS 0x7e7e7e7e7e7e7e7eULL
#define TOP_BOTTOM_MASK 0x00ffffffffffff00ULL
#define CORNER_MASK (LEFT_RIGHT_MAKS & TOP_BOTTOM_MASK)

typedef struct {
    Bitboard mask;
    int delta;
} DeltaInfo;

typedef struct {
    uint8_t sq;
} Move;

typedef struct {
    Move *moves;
    uint8_t size;
} MoveList;


const DeltaInfo DELTAS[8];

MoveList *movegen_gen_legal(Board *board);

#endif //OTHELLO_MOVEGEN_H
