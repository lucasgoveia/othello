//
// Created by govei on 25/09/2022.
//

#ifndef OTHELLO_EVAL_H
#define OTHELLO_EVAL_H

#include "../../core/board/board.h"

#define CORNERS_BB 0x8100000000000081ULL
#define TOP_RIGHT_CORNER 0x80ULL
#define TOP_LEFT_CORNER 0x1ULL
#define BOTTOM_RIGHT_CORNER 0x8000000000000000ULL
#define BOTTOM_LEFT_CORNER 0x100000000000000
#define FRONTIER_BB 0x3c7e7e7e7e3c00ULL

#define STONE_SCORE 100
#define WIN 12800
#define WIN_SCORE 12000
#define INFINITY 13800
#define DRAW 0

int eval(Board *board);
int eval_pos(Board *board);
int eval_plain(Board *board);

#endif //OTHELLO_EVAL_H
