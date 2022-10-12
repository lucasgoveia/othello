#include <time.h>
#include <string.h>

#include "core/bitboard/bitboard.h"
#include "core/board/board.h"
#include "engine/engine.h"
#include "engine/tt/tt.h"


void apply_random_mv(Board *board) {
    MoveList *mv_list = board_legal_moves(board);
    int mv_index = rand() % mv_list->size;
    board_apply_move(board, mv_list->moves[mv_index]);
    free(mv_list->moves);
    free(mv_list);
}

void apply_bot_move(TT *tt, Board *board) {
    Move mv = engine_search(tt, board, 12);
    board_apply_move(board, mv);
}

void apply_user_move(Board *board) {
    MoveList *mv_list = board_legal_moves(board);
    Move mv = {.stone_pos = 65};

    for(int i=0; i<mv_list->size; i++)
        printf("%s ", MOVE_DISPLAY[mv_list->moves[i].stone_pos]);

    char move[5];

    do {
        printf("\nInput your move: ");
        scanf("%s", move);

        for(int i=0; i<mv_list->size; i++) {
            if (strcmp(move, MOVE_DISPLAY[mv_list->moves[i].stone_pos]) == 0) {
                mv = mv_list->moves[i];
                break;
            }
        }
    } while (mv.stone_pos == 65);

    board_apply_move(board, mv);
    free(mv_list->moves);
    free(mv_list);
}


int main() {
    tt_init(80);
    srand(time(NULL));

    TT *tt = tt_init(80);
    Board *board = board_default();

    while (board->pass_move_count < 2) {
        board_print(board);
        if (board->turn == WHITE) {
            apply_random_mv(board);
        }
        else {
            apply_bot_move(tt, board);
        }
    }

    board_print(board);

    int white_count = bitboard_popcnt(board->bbs[WHITE]);
    int black_count = bitboard_popcnt(board->bbs[BLACK]);

    printf("Black: %d, White: %d", black_count, white_count);

    return 0;
}