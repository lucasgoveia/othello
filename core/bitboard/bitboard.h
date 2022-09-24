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

uint8_t bitboard_scan_forward(Bitboard bitboard);

#endif //OTHELLO_BITBOARD_H
