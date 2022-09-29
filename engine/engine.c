//
// Created by govei on 26/09/2022.
//

#include "engine.h"

#define MAX_PLY 64

typedef struct {
    Board *root;
    int ply;
    uint32_t nodes;
    Move pv_table[MAX_PLY][MAX_PLY];
    uint16_t pv_length[MAX_PLY];
    int history_mvs[MAX_PLY][2];
    Move killers_mvs[MAX_PLY][2];
    bool follow_pv;
    bool score_pv;
} SearchInfo;

void enable_pv_scoring(SearchInfo *sinfo, MoveList *mv_list) {
    sinfo->follow_pv = false;

    for (int i = 0; i < mv_list->size; i++) {
        Move mv = mv_list->moves[i];
        if (sinfo->pv_table[sinfo->ply][0].code == mv.code) {
            sinfo->follow_pv = true;
            sinfo->score_pv = true;
        }
    }
}

int move_score(SearchInfo *sinfo, Move mv) {
    if (sinfo->score_pv && sinfo->pv_table[sinfo->ply][0].code == mv.code) {
        sinfo->score_pv = false;
        return 20000;
    }

    if (sinfo->killers_mvs[sinfo->ply][0].code == mv.code) {
        return 9000;
    }

    if (sinfo->killers_mvs[sinfo->ply][1].code == mv.code) {
        return 8000;
    }

    return sinfo->history_mvs[mv.code][mv.player];
}

void sort_moves(SearchInfo *sinfo, MoveList *mv_list) {
    int *move_scores = (int *) malloc(sizeof(int) * mv_list->size);

    for (int i = 0; i < mv_list->size; i++) {
        move_scores[i] = move_score(sinfo, mv_list->moves[i]);
    }

    for (int i = 0; i < mv_list->size; i++) {
        for (int j = i + 1; j < mv_list->size; j++) {
            if (move_scores[i] < move_scores[j]) {

                int temp_score = move_scores[i];
                move_scores[i] = move_scores[j];
                move_scores[j] = temp_score;

                Move temp_move = mv_list->moves[i];
                mv_list->moves[i] = mv_list->moves[j];
                mv_list->moves[j] = temp_move;
            }
        }
    }

    free(move_scores);
}

int alphabeta(SearchInfo *sinfo, int alpha, int beta, int depth) {
    sinfo->pv_length[sinfo->ply] = sinfo->ply;

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

    if (sinfo->follow_pv) {
        enable_pv_scoring(sinfo, mv_list);
    }

    sort_moves(sinfo, mv_list);

    for (int i = 0; i < mv_list->size; i++) {
        board_apply_move(sinfo->root, mv_list->moves[i]);
        sinfo->ply++;

        int score = -alphabeta(sinfo, -beta, -alpha, depth - 1);

        board_undo_move(sinfo->root);
        sinfo->ply--;

        if (score > alpha) {
            sinfo->history_mvs[mv_list->moves[i].code][mv_list->moves[i].player] += depth;

            alpha = score;

            sinfo->pv_table[sinfo->ply][sinfo->ply] = mv_list->moves[i];

            for (int next_ply = sinfo->ply + 1; next_ply < sinfo->pv_length[sinfo->ply + 1]; next_ply++)
                sinfo->pv_table[sinfo->ply][next_ply] = sinfo->pv_table[sinfo->ply + 1][next_ply];

            sinfo->pv_length[sinfo->ply] = sinfo->pv_length[sinfo->ply + 1];

            if (score >= beta) {
                sinfo->killers_mvs[sinfo->ply][1] = sinfo->killers_mvs[sinfo->ply][0];
                sinfo->killers_mvs[sinfo->ply][0] = mv_list->moves[i];

                free(mv_list->moves);
                free(mv_list);

                return beta;
            }
        }
    }

    free(mv_list->moves);
    free(mv_list);

    return alpha;
}

Move engine_search(Board *board, int depth) {
    if (board_empty_bb(board) == 0 || board->pass_move_count >= 2) {
        Move mv = {.code = PASS_MOVE_CODE, .player = board->turn};
        return mv;
    }

    int alpha = -INFINITY;
    int beta = INFINITY;

    SearchInfo sinfo = {
            .root = board,
            .ply = 0,
            .nodes = 0,
    };

    clock_t t = clock();

    for (int d = 1; d <= depth; d++) {
        sinfo.follow_pv = true;
        int score = alphabeta(&sinfo, alpha, beta, d);

        double s = ((double) (clock() - t)) / CLOCKS_PER_SEC;
        printf("info score %d depth %d nodes %u in %lfs. %i n/s", score, d, sinfo.nodes, s,
               (int32_t) round(sinfo.nodes / s));

        // print new line
        printf("\n");
    }

    return sinfo.pv_table[0][0];
}

