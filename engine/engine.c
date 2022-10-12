//
// Created by govei on 26/09/2022.
//

#include "engine.h"

#define MAX_PLY 64
#define FULL_DEPTH_MOVES 4
#define REDUCTION_LIMIT 3

typedef struct {
    Board *root;
    TT *tt;
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
        if (sinfo->pv_table[sinfo->ply][0] == mv) {
            sinfo->follow_pv = true;
            sinfo->score_pv = true;
        }
    }
}

int move_score(SearchInfo *sinfo, Move mv) {
    if (sinfo->score_pv && sinfo->pv_table[sinfo->ply][0] == mv) {
        sinfo->score_pv = false;
        return 20000;
    }

    if (sinfo->killers_mvs[sinfo->ply][0] == mv) {
        return 9000;
    }

    if (sinfo->killers_mvs[sinfo->ply][1] == mv) {
        return 8000;
    }

    return sinfo->history_mvs[mv][sinfo->root->turn];
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

int score_to_tt(int score, int ply) {
    if (score > WIN_SCORE) {
        return score + ply;
    } else if (score < (-WIN_SCORE)) {
        return score - ply;
    } else {
        return score;
    }
}

int score_from__tt(int score, int ply) {
    if (score > WIN_SCORE) {
        return score - ply;
    } else if (score < (-WIN_SCORE)) {
        return score + ply;
    } else {
        return score;
    }
}

int alphabeta(SearchInfo *sinfo, int alpha, int beta, int depth) {
    bool pv_node = (beta - alpha) > 1;

    sinfo->pv_length[sinfo->ply] = sinfo->ply;

    TTEntry *tt_entry = tt_read_entry(sinfo->tt, sinfo->root->hash);

    if (!pv_node && tt_entry->key == sinfo->root->hash && sinfo->ply > 0) {
        if (tt_entry->node_type != NO_BOUND && tt_entry->depth >= depth) {
            int score = score_from__tt(tt_entry->score, sinfo->ply);

            if (tt_entry->node_type == EXACT) {
                return score;
            }
            if (tt_entry->node_type == ALPHA && score <= alpha) {
                return alpha;
            }
            if (tt_entry->node_type == BETA && score >= beta) {
                return beta;
            }
        }
    }

    if (board_empty_bb(sinfo->root) == 0 || sinfo->root->pass_move_count >= 2) {
        if (eval_plain(sinfo->root) > 0)
            return WIN - sinfo->ply;
        else if (eval_plain(sinfo->root) < 0)
            return -WIN + sinfo->ply;

        return DRAW;
    }

    if (depth == 0) {
        return eval(sinfo->root);
    }

    sinfo->nodes++;

    MoveList *mv_list = board_legal_moves(sinfo->root);

    if (sinfo->follow_pv) {
        enable_pv_scoring(sinfo, mv_list);
    }

    sort_moves(sinfo, mv_list);

    int moves_searched = 0;
    int score;
    Move best_move = ILLEGAL_MOVE;
    NodeType node_type = ALPHA;

    for (int i = 0; i < mv_list->size; i++) {
        board_apply_move(sinfo->root, mv_list->moves[i]);
        sinfo->ply++;

        // PV Search with LMR (Late Move Reduction)
        if (moves_searched == 0) {
            score = -alphabeta(sinfo, -beta, -alpha, depth - 1);
        } else {
            if (moves_searched >= FULL_DEPTH_MOVES && depth >= REDUCTION_LIMIT && !pv_node) {
                // Search with reduced depth
                score = -alphabeta(sinfo, -alpha - 1, -alpha, depth - 2);
            } else {
                // Ensure pv search
                score = alpha + 1;
            }

            if (score > alpha) {
                // Found PV node

                // Try to prove that the move is bad
                score = -alphabeta(sinfo, -alpha - 1, -alpha, depth - 1);

                if (score > alpha && score < beta) {
                    // If it fails redo the search with full window
                    score = -alphabeta(sinfo, -beta, -alpha, depth - 1);
                }
            }
        }

        board_undo_move(sinfo->root);
        sinfo->ply--;
        moves_searched++;

        if (score > alpha) {
            node_type = EXACT;
            best_move = mv_list->moves[i];
            
            sinfo->history_mvs[mv_list->moves[i]][sinfo->root->turn] += depth;

            alpha = score;

            sinfo->pv_table[sinfo->ply][sinfo->ply] = mv_list->moves[i];

            for (int next_ply = sinfo->ply + 1; next_ply < sinfo->pv_length[sinfo->ply + 1]; next_ply++)
                sinfo->pv_table[sinfo->ply][next_ply] = sinfo->pv_table[sinfo->ply + 1][next_ply];

            sinfo->pv_length[sinfo->ply] = sinfo->pv_length[sinfo->ply + 1];

            if (score >= beta) {
                tt_place_entry(sinfo->tt, sinfo->root->hash, score_to_tt(BETA, sinfo->ply), beta, best_move, depth);

                sinfo->killers_mvs[sinfo->ply][1] = sinfo->killers_mvs[sinfo->ply][0];
                sinfo->killers_mvs[sinfo->ply][0] = mv_list->moves[i];

                free(mv_list->moves);
                free(mv_list);

                return beta;
            }
        }
    }

    tt_place_entry(sinfo->tt, sinfo->root->hash, node_type, score_to_tt(alpha, sinfo->ply), best_move, depth);

    free(mv_list->moves);
    free(mv_list);

    return alpha;
}

Move engine_search(TT* tt, Board *board, int depth) {
    if (board_empty_bb(board) == 0 || board->pass_move_count >= 2) {
        Move mv = PASS_MOVE;
        return mv;
    }

    int alpha = -INFINITY;
    int beta = INFINITY;

    SearchInfo sinfo = {
            .root = board,
            .ply = 0,
            .nodes = 0,
            .tt = tt
    };

    clock_t t = clock();

    for (int d = 1; d <= depth; d++) {
        sinfo.follow_pv = true;
        int score = alphabeta(&sinfo, alpha, beta, d);

        double s = ((double) (clock() - t)) / CLOCKS_PER_SEC;
        printf("Score: %d. Search %u nodes in %lfs %i n/s. depth - %d", score, sinfo.nodes, s,
               (int32_t) round(sinfo.nodes / s), d);

        printf("\nPV: ");
        for (int count = 0; count < sinfo.pv_length[0]; count++) {
            // print PV move
            printf("%s ", MOVE_DISPLAY[sinfo.pv_table[0][count]]);
        }

        printf("\n");
    }

    return sinfo.pv_table[0][0];
}

