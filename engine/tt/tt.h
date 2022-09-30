//
// Created by govei on 29/09/2022.
//

#ifndef OTHELLO_TT_H
#define OTHELLO_TT_H

#include <stdint.h>
#include "../../core/board/board.h"

typedef enum {
    NO_BOUND,
    EXACT,
    ALPHA,
    BETA
} NodeType;

typedef struct {
    uint64_t key;
    int depth;
    NodeType node_type;
    int score;
    Move best_move;
} TTEntry;

typedef struct {
    TTEntry *entries;
    uint32_t size;
} TT;

void tt_init(uint16_t mb_size);

void tt_place_entry(uint64_t key, NodeType node_type, int score, Move best_move, int depth);

TTEntry *tt_read_entry(uint64_t key);

#endif //OTHELLO_TT_H
