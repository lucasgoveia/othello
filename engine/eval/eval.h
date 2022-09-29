//
// Created by govei on 25/09/2022.
//

#ifndef OTHELLO_EVAL_H
#define OTHELLO_EVAL_H

#include "../../core/board/board.h"

#define CORNERS_BB 0x8100000000000081ULL
#define ACCESS_CORNER_BB 0x42c300000000c342
#define STONE_SCORE 100
#define WIN_SCORE 6400
#define INFINITY 7400
#define DRAW 0
#define CORNER_BONUS 800
#define PENALTY_ACCESS_CORNER 200

int eval_pos(Board *board);
int eval_plain(Board *board);

#endif //OTHELLO_EVAL_H
