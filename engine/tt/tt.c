//
// Created by govei on 29/09/2022.
//

#include "tt.h"


TT *tt_init(uint16_t mb_size) {
    uint32_t num_entries = (mb_size * (1024 * 1024)) / sizeof(TTEntry);
    TTEntry *entries = (TTEntry *) malloc(num_entries * sizeof(TTEntry));

    TT *new_tt = (TT*) malloc(sizeof(TT));
    new_tt->size = num_entries;
    new_tt->entries = entries;

    return new_tt;
}

void tt_place_entry(TT *tt, uint64_t key, NodeType node_type, int score, Move best_move, int depth) {
    TTEntry *entry = &tt->entries[key % tt->size];

    entry->key = key;
    entry->score = score;
    entry->node_type = node_type;
    entry->best_move = best_move;
    entry->depth = depth;
}

TTEntry *tt_read_entry(TT *tt, uint64_t key) {
    return &tt->entries[key % tt->size];
}