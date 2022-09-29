#include <time.h>

#include "core/bitboard/bitboard.h"
#include "core/board/board.h"
#include "engine/engine.h"


void apply_random_mv(Board *board) {
    MoveList *mv_list = board_legal_moves(board);
    int mv_index = rand() % mv_list->size;
    board_apply_move(board, mv_list->moves[mv_index]);
    free(mv_list->moves);
    free(mv_list);
}

void apply_bot_move(Board *board) {
    Move mv = engine_search(board, 7);
    board_apply_move(board, mv);
}


int main() {
    srand(time(NULL));
    Board *board = board_default();

    while (board->pass_move_count < 2) {
        board_print(board);
        if (board->turn == WHITE) {
            apply_bot_move(board);
        } else {
            apply_random_mv(board);
        }
    }

    board_print(board);

    int white_count = bitboard_popcnt(board->bbs[WHITE]);
    int black_count = bitboard_popcnt(board->bbs[BLACK]);

    printf("Black: %d, White: %d", black_count, white_count);

    return 0;
}