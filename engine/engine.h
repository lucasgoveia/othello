//
// Created by govei on 26/09/2022.
//

#ifndef OTHELLO_ENGINE_H
#define OTHELLO_ENGINE_H

#include <time.h>
#include <math.h>

#include "../core/board/board.h"
#include "eval/eval.h"
#include "tt/tt.h"

Move engine_search(Board *board, int depth);

#endif //OTHELLO_ENGINE_H
