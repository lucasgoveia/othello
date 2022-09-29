//
// Created by govei on 25/09/2022.
//

#include "eval.h"

int eval_stones(Player player, Board *board) {
    Bitboard player_bb = board->bbs[player];
    return bitboard_popcnt(player_bb) * STONE_SCORE;
}

int eval_corners(Player player, Board *board) {
    Bitboard player_bb = board->bbs[player];
    Bitboard stones_corner_bb = player_bb & CORNERS_BB;
    Bitboard access_corner_bb = player_bb & ACCESS_CORNER_BB;

    return bitboard_popcnt(stones_corner_bb) * CORNER_BONUS
        + bitboard_popcnt(access_corner_bb) * (-PENALTY_ACCESS_CORNER);
}

int eval_pos(Board *board) {
    int white_score = eval_stones(WHITE, board) + eval_corners(WHITE, board);
    int black_score = eval_stones(BLACK, board) + eval_corners(BLACK, board);

    return (white_score - black_score) * (board->turn == WHITE ? 1 : - 1);
}

int eval_plain(Board *board) {
    return (eval_stones(WHITE, board) - eval_stones(BLACK, board))
           * (board->turn == WHITE ? 1 : - 1);
}

