//
// Created by govei on 25/09/2022.
//

#include "eval.h"

int eval_stones(Player player, Board *board) {
    Bitboard player_bb = board->bbs[player];
    return bitboard_popcnt(player_bb) * STONE_SCORE;
}

int eval_plain(Board *board) {
    return (eval_stones(WHITE, board) - eval_stones(BLACK, board))
           * (board->turn == WHITE ? 1 : -1);
}

int count_frontiers_stones(Bitboard us, Bitboard occ) {
    Bitboard frontiers_bb = 0ULL;
    Bitboard frontier_us = FRONTIER_BB & us;
    Bitboard empty = ~occ;

    Bitboard mask = (frontier_us << 8) & empty;
    frontiers_bb |= mask >> 8;

    mask = (frontier_us >> 8) & empty;
    frontiers_bb |= mask << 8;

    mask = (frontier_us << 1) & empty;
    frontiers_bb |= mask >> 1;

    mask = (frontier_us >> 1) & empty;
    frontiers_bb |= mask << 1;

    return bitboard_popcnt(frontiers_bb);
}

int count_stables_stones(Bitboard bb) {
    if ((CORNERS_BB & bb) == 0) {
        return 0;
    }

    Bitboard mask = 0ULL;
    if ((TOP_RIGHT_CORNER & bb) != 0) {
        mask |= TOP_RIGHT_CORNER;

        Bitboard single_bit_mask = TOP_RIGHT_CORNER >> 1; // Left 1st row
        while ((single_bit_mask & bb) != 0 && single_bit_mask != TOP_LEFT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask >>= 1;
        }

        single_bit_mask = TOP_RIGHT_CORNER << 8; // Bottom 1st column
        while ((single_bit_mask & bb) != 0 && single_bit_mask != BOTTOM_RIGHT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask <<= 8;
        }

        single_bit_mask = 0x80c0ULL; // Left 2nd row
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x101ULL) {
            mask |= single_bit_mask;
            single_bit_mask >>= 1;
        }

        single_bit_mask = 0x80c0ULL; // Bottom 2nd column
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0xc000000000000000ULL) {
            mask |= single_bit_mask;
            single_bit_mask <<= 8;
        }

        // 3rd stone diagonally
        if ((0xe0f0f8ULL & bb) == 0xe0f0f8ULL) {
            mask |= 0xe0f0f8ULL;
        } else if ((0x80c0e0e0e0 & bb) == 0x80c0e0e0e0) {
            mask |= 0x80c0e0e0e0;
        }
    }
    if ((TOP_LEFT_CORNER & bb) != 0) {
        mask |= TOP_LEFT_CORNER;

        Bitboard single_bit_mask = TOP_LEFT_CORNER << 1; // Right 1st row
        while ((single_bit_mask & bb) != 0 && single_bit_mask != TOP_RIGHT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask <<= 1;
        }

        single_bit_mask = TOP_LEFT_CORNER << 8; // Bottom 1st column
        while ((single_bit_mask & bb) != 0 && single_bit_mask != BOTTOM_LEFT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask <<= 8;
        }

        single_bit_mask = 0x103ULL; // Right 2nd row
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x8080ULL) {
            mask |= single_bit_mask;
            single_bit_mask <<= 1;
        }

        single_bit_mask = 0x103ULL; // Bottom 2nd column
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x300000000000000ULL) {
            mask |= single_bit_mask;
            single_bit_mask <<= 8;
        }

        // 3rd stone diagonally
        if ((0x70f1fULL & bb) == 0x70f1fULL) {
            mask |= 0x70f1fULL;
        } else if ((0x103070707ULL & bb) == 0x103070707ULL) {
            mask |= 0x103070707ULL;
        }
    }
    if ((BOTTOM_RIGHT_CORNER & bb) != 0) {
        mask |= BOTTOM_RIGHT_CORNER;

        Bitboard single_bit_mask = BOTTOM_RIGHT_CORNER >> 1; // Left 1st row
        while ((single_bit_mask & bb) != 0 && single_bit_mask != BOTTOM_LEFT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask >>= 1;
        }

        single_bit_mask = BOTTOM_RIGHT_CORNER >> 8; // Top 1st column
        while ((single_bit_mask & bb) != 0 && single_bit_mask != TOP_RIGHT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask >>= 8;
        }

        single_bit_mask = 0xc080000000000000ULL; // Left 2nd row
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x101000000000000ULL) {
            mask |= single_bit_mask;
            single_bit_mask >>= 1;
        }

        single_bit_mask = 0xc080000000000000ULL; // Top 2nd column
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0xc0ULL) {
            mask |= single_bit_mask;
            single_bit_mask >>= 8;
        }

        // 3rd stone diagonally
        if ((0xf8f0e00000000000ULL & bb) == 0xf8f0e00000000000ULL) {
            mask |= 0xf8f0e00000000000ULL;
        } else if ((0xe0e0e0c080000000ULL & bb) == 0xe0e0e0c080000000ULL) {
            mask |= 0xe0e0e0c080000000ULL;
        }
    }
    if ((BOTTOM_LEFT_CORNER & bb) != 0) {
        mask |= BOTTOM_LEFT_CORNER;

        Bitboard single_bit_mask = BOTTOM_LEFT_CORNER << 1; // Right 1st row
        while ((single_bit_mask & bb) != 0 && single_bit_mask != BOTTOM_RIGHT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask <<= 1;
        }

        single_bit_mask = BOTTOM_LEFT_CORNER >> 8; // Bottom 1st column
        while ((single_bit_mask & bb) != 0 && single_bit_mask != TOP_LEFT_CORNER) {
            mask |= single_bit_mask;
            single_bit_mask >>= 8;
        }

        single_bit_mask = 0x301000000000000ULL; // Right 2nd row
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x8080000000000000ULL) {
            mask |= single_bit_mask;
            single_bit_mask <<= 1;
        }

        single_bit_mask = 0x301000000000000ULL; // Bottom 2nd column
        while ((single_bit_mask & bb) == single_bit_mask && single_bit_mask != 0x3ULL) {
            mask |= single_bit_mask;
            single_bit_mask >>= 8;
        }

        // 3rd stone diagonally
        if ((0x1f0f070000000000ULL & bb) == 0x1f0f070000000000ULL) {
            mask |= 0x1f0f070000000000ULL;
        } else if ((0x707070301000000ULL & bb) == 0x707070301000000ULL) {
            mask |= 0x707070301000000ULL;
        }
    }

    return bitboard_popcnt(mask);
}

int eval(Board *board) {
    Bitboard us = board->bbs[board->turn];
    Bitboard them = board->bbs[player_other(board->turn)];

    int us_mobility = board_legal_moves_count(board);

    board->turn = player_other(board->turn);
    int them_mobility = board_legal_moves_count(board);
    board->turn = player_other(board->turn);

    int us_frontiers = count_frontiers_stones(us, us | them);
    int them_frontiers = count_frontiers_stones(them, us | them);

    int us_stables = count_stables_stones(us);
    int them_stables = count_stables_stones(them);

    return 73 * (us_mobility - them_mobility)
        - 57  * (us_frontiers - them_frontiers)
        + 141 * (us_stables - them_stables);
}

