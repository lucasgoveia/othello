#include "core/bitboard/bitboard.h"
#include "core/board/board.h"

int main() {
    Board *board = board_default();

    board_print(board);

    return 0;
}
