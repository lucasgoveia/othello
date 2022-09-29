//
// Created by govei on 24/09/2022.
//

#include "board.h"

const DeltaInfo DELTAS[8] = {
        {LEFT_RIGHT_MAKS, 1},
        {CORNER_MASK,     9},
        {TOP_BOTTOM_MASK, 8},
        {CORNER_MASK,     7},
        {LEFT_RIGHT_MAKS, -1},
        {CORNER_MASK,     -9},
        {TOP_BOTTOM_MASK, -8},
        {CORNER_MASK,     -7},
};

Board *board_default() {
    Board *board = (Board *) malloc(sizeof(Board));

    board->bbs[WHITE] = BB_WHITE_START;
    board->bbs[BLACK] = BB_BLACK_START;
    board->turn = BLACK;
    board->pass_move_count = 0;
    board->prev = NULL;

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

static inline Bitboard apply_delta_shift(Bitboard bb, int delta_offset) {
    return delta_offset >= 0
           ? bb >> delta_offset
           : bb << (delta_offset * (-1));
}

Bitboard search_contiguous(DeltaInfo delta, Bitboard us, Bitboard them) {
    Bitboard w = them & delta.mask;
    Bitboard t = w & apply_delta_shift(us, delta.offset);
    t |= w & apply_delta_shift(t, delta.offset);
    t |= w & apply_delta_shift(t, delta.offset);
    t |= w & apply_delta_shift(t, delta.offset);
    t |= w & apply_delta_shift(t, delta.offset);
    t |= w & apply_delta_shift(t, delta.offset);
    return t;
}

MoveList *board_legal_moves(Board *board) {
    Bitboard moves_bb = 0ULL;

    Bitboard us = board_us_bb(board);
    Bitboard them = board_them_bb(board);
    Bitboard empty = board_empty_bb(board);

    for (int i = 0; i < 8; i++) {
        DeltaInfo delta = DELTAS[i];
        moves_bb |= empty & apply_delta_shift(search_contiguous(delta, us, them), delta.offset);
    }

    uint8_t movecnt = bitboard_popcnt(moves_bb);
    Move *moves = (Move *) malloc(sizeof(Move) * movecnt);

    int move_index = 0;
    while (moves_bb != 0) {
        uint8_t sq = bitboard_pop_lsb(&moves_bb);
        Move move = {.code = sq, .player = board->turn};
        moves[move_index] = move;

        move_index++;
    }

    if (move_index == 0) {
        Move move = {.code = PASS_MOVE_CODE, .player = board->turn};
        moves = (Move *) malloc(sizeof(Move));
        moves[0] = move;
        movecnt++;
    }

    MoveList *mv_list = malloc(sizeof(MoveList));
    mv_list->size = movecnt;
    mv_list->moves = moves;

    return mv_list;
}

Board *board_copy(Board *board) {
    Board *new_board = (Board *) malloc(sizeof(Board));

    new_board->prev = board->prev;
    new_board->turn = board->turn;
    new_board->bbs[WHITE] = board->bbs[WHITE];
    new_board->bbs[BLACK] = board->bbs[BLACK];
    new_board->pass_move_count = board->pass_move_count;

    return new_board;
}

void board_apply_move(Board *board, Move mv) {
    if (mv.code == PASS_MOVE_CODE) {
        Board *copy = board_copy(board);

        board->prev = copy;
        board->turn = player_other(board->turn);
        board->bbs[WHITE] = board->bbs[WHITE];
        board->bbs[BLACK] = board->bbs[BLACK];
        board->pass_move_count = board->pass_move_count + 1;
        return;
    }

    Bitboard us = board_us_bb(board);
    Bitboard them = board_them_bb(board);

    Bitboard bitmove = 1ULL << mv.code;
    Bitboard flipped_stones = 0ULL;

    for (int i = 0; i < 8; i++) {
        DeltaInfo delta = DELTAS[i];
        Bitboard s = search_contiguous(delta, bitmove, them);
        flipped_stones |= (us & apply_delta_shift(s, delta.offset)) == 0ULL
                          ? 0ULL
                          : s;
    }

    us |= flipped_stones | bitmove;
    them &= ~flipped_stones;

    Board *copy = board_copy(board);

    board->prev = copy;
    board->pass_move_count = 0;
    board->turn = player_other(copy->turn);
    board->bbs[WHITE] = copy->turn == WHITE
        ? us
        : them;
    board->bbs[BLACK] = copy->turn == BLACK
        ? us
        : them;
}

void board_undo_move(Board *board) {
    board->bbs[0] = board->prev->bbs[0];
    board->bbs[1] = board->prev->bbs[1];
    board->turn = board->prev->turn;
    board->pass_move_count = board->prev->pass_move_count;
    Board *prev = board->prev;
    board->prev = prev->prev;
    free(prev);
}