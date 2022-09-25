#include "core/bitboard/bitboard.h"
#include "core/board/board.h"
#include "core/movegen/movegen.h"

int main() {
    Board *board = board_default();

    MoveList *moves = movegen_gen_legal(board);

    for (int i = 0; i < moves->size; ++i) {
        printf("%d, ", moves->moves[i].sq);
    }
    printf("\n");

    return 0;
}
