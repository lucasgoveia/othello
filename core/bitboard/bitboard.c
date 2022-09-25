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