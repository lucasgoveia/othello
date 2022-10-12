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

const char *MOVE_DISPLAY[65] = {
        "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
        "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
        "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
        "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
        "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
        "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
        "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
        "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
        "pm"
};

static inline uint64_t get_pos_hash(uint8_t stone_pos, SquareOccupancyType occ) {
    return RAND_HASHES[stone_pos + occ * 64];
}

Board *board_default() {
    Board *board = (Board *) malloc(sizeof(Board));

    Bitboard white = BB_WHITE_START;
    Bitboard black = BB_BLACK_START;
    Bitboard empty = ~(white | black);

    board->bbs[WHITE] = white;
    board->bbs[BLACK] = black;
    board->turn = BLACK;
    board->pass_move_count = 0;
    board->prev = NULL;
    board->hash = 0ULL;

    while (white) {
        uint8_t pos = bitboard_pop_lsb(&white);
        board->hash ^= get_pos_hash(pos, SQ_OCC_WHITE);
    }

    while (black) {
        uint8_t pos = bitboard_pop_lsb(&black);
        board->hash ^= get_pos_hash(pos, SQ_OCC_BLACK);
    }

    while (empty) {
        uint8_t pos = bitboard_pop_lsb(&empty);
        board->hash ^= get_pos_hash(pos, SQ_OCC_EMPTY);
    }

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

static inline Bitboard board_legal_moves_bb(Board *board) {
    Bitboard moves_bb = 0ULL;

    Bitboard us = board_us_bb(board);
    Bitboard them = board_them_bb(board);
    Bitboard empty = board_empty_bb(board);

    for (int i = 0; i < 8; i++) {
        DeltaInfo delta = DELTAS[i];
        moves_bb |= empty & apply_delta_shift(search_contiguous(delta, us, them), delta.offset);
    }

    return moves_bb;
}

MoveList *board_legal_moves(Board *board) {
    Bitboard moves_bb = board_legal_moves_bb(board);

    uint8_t movecnt = bitboard_popcnt(moves_bb);
    Move *moves = (Move *) malloc(sizeof(Move) * movecnt);

    int move_index = 0;
    while (moves_bb != 0) {
        Move move = bitboard_pop_lsb(&moves_bb);
        moves[move_index] = move;

        move_index++;
    }

    if (move_index == 0) {
        Move move = PASS_MOVE;
        moves = (Move *) malloc(sizeof(Move));
        moves[0] = move;
        movecnt++;
    }

    MoveList *mv_list = malloc(sizeof(MoveList));
    mv_list->size = movecnt;
    mv_list->moves = moves;

    return mv_list;
}

uint8_t board_legal_moves_count(Board *board) {

    Bitboard moves_bb = board_legal_moves_bb(board);

    return bitboard_popcnt(moves_bb);
}

Board *board_copy(Board *board) {
    Board *new_board = (Board *) malloc(sizeof(Board));

    new_board->prev = board->prev;
    new_board->hash = board->hash;
    new_board->turn = board->turn;
    new_board->bbs[WHITE] = board->bbs[WHITE];
    new_board->bbs[BLACK] = board->bbs[BLACK];
    new_board->pass_move_count = board->pass_move_count;

    return new_board;
}


void board_apply_move(Board *board, Move mv) {
    if (mv == PASS_MOVE) {
        Board *copy = board_copy(board);

        board->prev = copy;
        board->hash ^= PLAYER_HASH;
        board->turn = player_other(board->turn);
        board->bbs[WHITE] = board->bbs[WHITE];
        board->bbs[BLACK] = board->bbs[BLACK];
        board->pass_move_count = board->pass_move_count + 1;
        return;
    }

    Bitboard us = board_us_bb(board);
    Bitboard them = board_them_bb(board);

    Bitboard bitmove = 1ULL << mv;
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

    Board *prev = board_copy(board);

    board->prev = prev;
    board->pass_move_count = 0;
    board->turn = player_other(prev->turn);
    board->bbs[WHITE] = prev->turn == WHITE
                        ? us
                        : them;
    board->bbs[BLACK] = prev->turn == BLACK
                        ? us
                        : them;

    board->hash ^= PLAYER_HASH;
    board->hash ^= get_pos_hash(mv, (SquareOccupancyType) prev->turn)
            ^ get_pos_hash(mv, SQ_OCC_EMPTY);

    while (flipped_stones) {
        uint8_t pos = bitboard_pop_lsb(&flipped_stones);
        board->hash ^= get_pos_hash(pos, (SquareOccupancyType) prev->turn)
                ^ get_pos_hash(pos, (SquareOccupancyType) player_other(prev->turn));
    }
}

void board_undo_move(Board *board) {
    board->bbs[0] = board->prev->bbs[0];
    board->bbs[1] = board->prev->bbs[1];
    board->turn = board->prev->turn;
    board->pass_move_count = board->prev->pass_move_count;
    board->hash = board->prev->hash;
    Board *prev = board->prev;
    board->prev = prev->prev;
    free(prev);
}