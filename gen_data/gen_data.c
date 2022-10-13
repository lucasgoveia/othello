#include "gen_data.h"

Move get_random_mv(Board *board) {
    MoveList *mv_list = board_legal_moves(board);
    int mv_index = rand() % mv_list->size;
    Move mv = mv_list->moves[mv_index];

    free(mv_list->moves);
    free(mv_list);

    return mv;
}

Move get_bot_move(TT *tt, Board *board, int depth) {
    Board *copy = board_copy(board);
    Move mv = engine_search(tt, copy, depth);
    free(copy);
    return mv;
}


Move get_player_mv(TT *tt, Board *board, PlayerType pt) {
    switch (pt) {
        case RANDOM:
            return get_random_mv(board);
        case ALPHA_BETA_1:
            return get_bot_move(tt, board, 1);
        case ALPHA_BETA_3:
            return get_bot_move(tt, board, 3);
        case ALPHA_BETA_5:
            return get_bot_move(tt, board, 5);
        case ALPHA_BETA_7:
            return get_bot_move(tt, board, 7);
        case ALPHA_BETA_9:
            return get_bot_move(tt, board, 9);
    }
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *play_game(void *vargp) {
    TT *tt = tt_init(16);
    Board *board = board_default();

    Player main_player = (Player) rand() % 2;
    PlayerType other_player_type = (PlayerType) rand() % 6;

    BoardAndMove data[80];
    int data_cnt = 0;

    for (int i = 0; i < 7; i++) {
        Move mv = get_random_mv(board);
        board_apply_move(board, mv);
    }

    if (board->pass_move_count >= 2) {
        free(tt->entries);
        free(tt);
        board_free(board);
        return NULL;
    }


    while (board->pass_move_count < 2) {
        if (board->turn == main_player) {
            Move mv = get_bot_move(tt, board, 9);
            data[data_cnt].board = board_copy(board);
            data[data_cnt].mv = mv;
            board_apply_move(board, mv);
        } else {
            Move mv = get_player_mv(tt, board, other_player_type);
            data[data_cnt].board = board_copy(board);
            data[data_cnt].mv = mv;
            board_apply_move(board, mv);
        }
        data_cnt++;
    }

    int white_count = bitboard_popcnt(board->bbs[WHITE]);
    int black_count = bitboard_popcnt(board->bbs[BLACK]);

    printf("Main as %s, Black: %d, White: %d\n", main_player == WHITE ? "white" : "black", black_count, white_count);

    GenDataInfo *info = (GenDataInfo *) vargp;
    pthread_mutex_lock(&mutex);

    info->games_cnt++;

    FILE *f = fopen(info->games_dump_file_name, "ab");

    for (int i = 0; i < data_cnt; i++) {
        Move mv = data[i].mv;
        info->positions_cnt += 3;

        int winner = 0;

        if (white_count > black_count) {
            winner = 1;
        } else if (black_count > white_count) {
            winner = -1;
        }

        // Save normal board
        Board *current = data[i].board;
        for (int j = 0; j < 6 && current != NULL; j++) {
            Bitboard white = current->bbs[WHITE];
            Bitboard black = current->bbs[BLACK];

            fwrite(&white, sizeof white, 1, f);
            fwrite(&black, sizeof black, 1, f);
            current = current->prev;
        }

        fwrite(&mv, sizeof mv, 1, f);
        fwrite(&winner, sizeof winner, 1, f);

        // Save flipped vert board
        current = data[i].board;
        for (int j = 0; j < 6 && current != NULL; j++) {
            Bitboard white = bitboard_flip_vert(current->bbs[WHITE]);
            Bitboard black = bitboard_flip_vert(current->bbs[BLACK]);

            fwrite(&white, sizeof white, 1, f);
            fwrite(&black, sizeof black, 1, f);
            current = current->prev;
        }

        Move vmv = mv == PASS_MOVE ? mv : mv ^ 56;

        fwrite(&vmv, sizeof vmv, 1, f);
        fwrite(&winner, sizeof winner, 1, f);

        // Save flipped horiz board
        current = data[i].board;
        for (int j = 0; j < 6 && current != NULL; j++) {
            Bitboard white = bitboard_flip_horinz(current->bbs[WHITE]);
            Bitboard black = bitboard_flip_horinz(current->bbs[BLACK]);

            fwrite(&white, sizeof white, 1, f);
            fwrite(&black, sizeof black, 1, f);
            current = current->prev;
        }

        Move hmv = mv == PASS_MOVE ? mv : mv ^ 7;

        fwrite(&hmv, sizeof hmv, 1, f);
        fwrite(&winner, sizeof winner, 1, f);

        free(data[i].board);
    }

    fclose(f);

    pthread_mutex_unlock(&mutex);

    free(tt->entries);
    free(tt);
    board_free(board);

    return NULL;
}

void gen_data() {
    clock_t t = clock();

    GenDataInfo info;
    info.games_cnt = 0;
    info.positions_cnt = 0;
    info.games_dump_file_name = "/home/lucas/dev/tcc/othello/data/games.dat";


    FILE *f = fopen(info.games_dump_file_name, "wb");
    fclose(f);

    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);

    const int total_games = 30000;

    while (info.games_cnt < total_games) {
        pthread_t threads_ids[12];

        for (int i = 0; i < 12; i++) {
            pthread_create(&threads_ids[i], NULL, play_game, &info);
        }

        for (int i = 0; i < 12; i++) {
            pthread_join(threads_ids[i], NULL);
        }

        float s = ((float) (clock() - t)) / CLOCKS_PER_SEC;
        float completion = (float) info.games_cnt / (float) total_games;
        float eta = s / completion - s;
        printf("Played %d games so far %d positions. %.2f%% completed. ETA: %.2f minutes\n", info.games_cnt,
               info.positions_cnt, completion * 100.0, eta / 60.0);
    }
}