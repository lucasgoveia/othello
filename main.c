#include <time.h>
#include <string.h>

#include "core/bitboard/bitboard.h"
#include "core/board/board.h"
#include "engine/engine.h"
#include "engine/tt/tt.h"
#include "gen_data/gen_data.h"


// void apply_random_mv(Board *board) {
//     MoveList *mv_list = board_legal_moves(board);
//     int mv_index = rand() % mv_list->size;
//     board_apply_move(board, mv_list->moves[mv_index]);
//     free(mv_list->moves);
//     free(mv_list);
// }

// void apply_bot_move(TT *tt, Board *board) {
//     Move mv = engine_search(tt, board, 14);
//     board_apply_move(board, mv);
// }

// void apply_user_move(Board *board) {
//     MoveList *mv_list = board_legal_moves(board);
//     Move mv = ILLEGAL_MOVE;

//     for(int i=0; i<mv_list->size; i++)
//         printf("%s ", MOVE_DISPLAY[mv_list->moves[i]]);

//     char move[5];

//     do {
//         printf("\nInput your move: ");
//         scanf("%s", move);

//         for(int i=0; i<mv_list->size; i++) {
//             if (strcmp(move, MOVE_DISPLAY[mv_list->moves[i]]) == 0) {
//                 mv = mv_list->moves[i];
//                 break;
//             }
//         }
//     } while (mv == ILLEGAL_MOVE);

//     board_apply_move(board, mv);
//     free(mv_list->moves);
//     free(mv_list);
// }


int main() {
    srand(time(NULL));

    gen_data();

    return 0;
}