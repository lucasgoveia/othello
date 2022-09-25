#include <time.h>

#include "core/bitboard/bitboard.h"
#include "core/board/board.h"


int main() {
    srand(time(NULL));
    Board *board = board_default();

    while (board->pass_move_count < 2) {
        MoveList *mv_list = board_legal_moves(board);
        int mv_index = rand() % mv_list->size;
        board = board_apply_move(board, mv_list->moves[mv_index]);
        board_print(board);
        free(mv_list->moves);
        free(mv_list);
    }

    return 0;
}
