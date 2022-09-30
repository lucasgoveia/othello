cmake_minimum_required(VERSION 3.23)
project(othello C)

set(CMAKE_C_STANDARD 17)

add_executable(othello main.c core/bitboard/bitboard.h core/bitboard/bitboard.c core/board/board.c core/board/board.h core/player/player.h core/defs.h core/player/player.c engine/eval/eval.c engine/eval/eval.h engine/engine.c engine/engine.h engine/tt/tt.c engine/tt/tt.h core/board/hash.c)
