//
// Created by govei on 24/09/2022.
//

#include "board.h"

Board *board_default() {
    Board *board = (Board*) malloc(sizeof(Board));

    board->bbs[WHITE] = BB_WHITE_START;
    board->bbs[BLACK] = BB_BLACK_START;
    board->turn = BLACK;

    return board;
}

void board_print(Board *board) {
    printf("   %s", "+___+___+___+___+___+___+___+___+\n");

    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            bool white_occupied = (board->bbs[WHITE] & (1ULL << square)) > 0;
            bool black_occupied = (board->bbs[BLACK] & (1ULL << square)) > 0;

            if (file == 0) printf(" %d |", rank + 1);

            printf(" ");

            if (white_occupied)
                printf("%c", PLAYER_DISPLAY[WHITE]);
            else if (black_occupied)
                printf("%c", PLAYER_DISPLAY[BLACK]);
            else
                printf(" ");

            printf(" |");
        }

        printf("\n   %s\n", "+___+___+___+___+___+___+___+___+");
    }
    printf("   %s", "  a   b   c   d   e   f   g   h  \n");
    printf("\n");
}

Bitboard board_us_bb(Board *board) {
    return board->bbs[board->turn];
}

Bitboard board_them_bb(Board *board) {
    return board->bbs[board->turn ^ 1];
}

Bitboard board_empty_bb(Board *board) {
    return ~(board->bbs[WHITE] | board->bbs[BLACK]);
}