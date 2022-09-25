//
// Created by govei on 24/09/2022.
//

#include "movegen.h"

const DeltaInfo DELTAS[8] = {
        {LEFT_RIGHT_MAKS, 1},
        {CORNER_MASK, 9},
        {TOP_BOTTOM_MASK, 8},
        {CORNER_MASK, 7},
        {LEFT_RIGHT_MAKS, -1},
        {CORNER_MASK, -9},
        {TOP_BOTTOM_MASK, -8},
        {CORNER_MASK, -7},
};

static inline Bitboard apply_delta_shift(Bitboard bb, int delta_offset) {
    return delta_offset >= 0
        ? bb >> delta_offset
        : bb << (delta_offset * (-1));
}

Bitboard search_contiguous(DeltaInfo delta, Bitboard us, Bitboard them, Bitboard empty) {
    Bitboard w = them & delta.mask;
    Bitboard t = w & apply_delta_shift(us, delta.delta);
    t |= w & apply_delta_shift(t, delta.delta);
    t |= w & apply_delta_shift(t, delta.delta);
    t |= w & apply_delta_shift(t, delta.delta);
    t |= w & apply_delta_shift(t, delta.delta);
    t |= w & apply_delta_shift(t, delta.delta);
    return empty & apply_delta_shift(t, delta.delta);
}

MoveList *movegen_gen_legal(Board *board) {
    Bitboard moves_bb = 0ULL;

    Bitboard us = board_us_bb(board);
    Bitboard them = board_them_bb(board);
    Bitboard empty = board_empty_bb(board);

    for(int i=0; i < 8; i++) {
        moves_bb |= search_contiguous(DELTAS[i], us, them, empty);
    }

    uint8_t movecnt = bitboard_popcnt(moves_bb);
    Move* moves = (Move*) malloc(sizeof(Move) * movecnt);

    int move_index = 0;
    while(moves_bb != 0) {
        uint8_t sq = bitboard_pop_lsb(&moves_bb);
        Move move = {.sq = sq};
        moves[move_index] = move;

        move_index++;
    }

    MoveList  *mv_list = malloc(sizeof(MoveList));
    mv_list->size=  movecnt;
    mv_list->moves = moves;

    return mv_list;
}