//
// Created by govei on 26/09/2022.
//

#include "engine.h"


typedef struct {
    Board *root;
    int ply;
    uint32_t nodes;
    Move best_move;
} SearchInfo;

int alphabeta(SearchInfo *sinfo, int alpha, int beta, int depth) {
    if (board_empty_bb(sinfo->root) == 0 || sinfo->root->pass_move_count >= 2) {
        if (eval_plain(sinfo->root) > 0)
            return WIN_SCORE - sinfo->ply;
        else if (eval_plain(sinfo->root) < 0)
            return -WIN_SCORE + sinfo->ply;

        return DRAW;
    }

    if (depth == 0) {
        return eval_pos(sinfo->root);
    }

    sinfo->nodes++;

    MoveList *mv_list = board_legal_moves(sinfo->root);

    for (int i=0; i < mv_list->size; i++) {
        board_apply_move(sinfo->root, mv_list->moves[i]);
        sinfo->ply++;

        int score = -alphabeta(sinfo, -beta, -alpha, depth - 1);

        board_undo_move(sinfo->root);
        sinfo->ply--;

        if (score > alpha) {
            alpha = score;

            if (sinfo->ply == 0) {
                sinfo->best_move = mv_list->moves[i];
            }

            if (score >= beta) {
                return beta;
            }
        }
    }

    free(mv_list->moves);
    free(mv_list);

    return alpha;
}

Move engine_search(Board *board, int depth) {
    int alpha = -INFINITY;
    int beta = INFINITY;

    SearchInfo sinfo = {
            .root = board,
            .ply = 0,
            .nodes = 0,
            .best_move = {.code = PASS_MOVE_CODE}
    };

    alphabeta(&sinfo, alpha, beta, depth);

    return sinfo.best_move;
}

