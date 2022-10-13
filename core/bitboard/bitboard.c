//
// Created by govei on 23/09/2022.
//

#include "bitboard.h"

uint8_t DEBRUIJ_T[] = {
        0, 47, 1, 56, 48, 27, 2, 60,
        57, 49, 41, 37, 28, 16, 3, 61,
        54, 58, 35, 52, 50, 42, 21, 44,
        38, 32, 29, 23, 17, 11, 4, 62,
        46, 55, 26, 59, 40, 36, 15, 53,
        34, 51, 20, 43, 31, 22, 10, 45,
        25, 39, 14, 33, 19, 30, 9, 24,
        13, 18, 8, 12, 7, 6, 5, 63
};

const uint64_t DEBRUIJ_M = 0x03f79d71b4cb0a89;

void bitboard_print(Bitboard bitboard) {
    printf("   %s", "+___+___+___+___+___+___+___+___+\n");

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            bool occupied = (bitboard & (1ULL << square)) > 0;

            if (file == 0) printf(" %d |", rank + 1);

            if (occupied)
                printf(" O |");
            else
                printf("   |");
        }

        printf("\n   %s\n", "+___+___+___+___+___+___+___+___+");
    }
    printf("   %s", "  a   b   c   d   e   f   g   h  \n");
    printf("\n");
}

uint8_t bitboard_scan_forward(Bitboard bb) {
    return DEBRUIJ_T[((bb ^ (bb - 1)) * DEBRUIJ_M) >> 58];
}

uint8_t bitboard_pop_lsb(Bitboard *bb) {
    uint8_t index = bitboard_scan_forward(*bb);
    *bb &= *bb - 1;
    return index;
}

uint8_t bitboard_popcnt(Bitboard bb) {
    uint8_t count = 0;
    while (bb) {
        bb &= (bb - 1);
        count++;
    }
    return count;
}

Bitboard bitboard_flip_vert(Bitboard bb) {
    const Bitboard k1 = 0x00FF00FF00FF00FFULL;
    const Bitboard k2 = 0x0000FFFF0000FFFFULL;
    bb = ((bb >>  8) & k1) | ((bb & k1) <<  8);
    bb = ((bb >> 16) & k2) | ((bb & k2) << 16);
    bb = ( bb >> 32)       | ( bb       << 32);
    return bb;
}

Bitboard bitboard_flip_horinz(Bitboard bb) {
    const Bitboard k1 = 0x5555555555555555ULL;
    const Bitboard k2 = 0x3333333333333333ULL;
    const Bitboard k4 = 0x0f0f0f0f0f0f0f0fULL;
    bb = ((bb >> 1) & k1) | ((bb & k1) << 1);
    bb = ((bb >> 2) & k2) | ((bb & k2) << 2);
    bb = ((bb >> 4) & k4) | ((bb & k4) << 4);
    return bb;
}