//
// Created by govei on 23/09/2022.
//

#ifndef OTHELLO_BITBOARD_H
#define OTHELLO_BITBOARD_H



#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t Bitboard;

void bitboard_print(Bitboard bitboard);

uint8_t bitboard_scan_forward(Bitboard bb);

uint8_t bitboard_pop_lsb(Bitboard *bb);

uint8_t bitboard_popcnt(Bitboard bb);

Bitboard bitboard_flip_vert(Bitboard bb);
Bitboard bitboard_flip_horinz(Bitboard bb);

#endif //OTHELLO_BITBOARD_H
