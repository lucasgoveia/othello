//
// Created by govei on 24/09/2022.
//

#ifndef OTHELLO_PLAYER_H
#define OTHELLO_PLAYER_H

typedef enum {
    WHITE,
    BLACK
} Player;

extern const char PLAYER_DISPLAY[2];

Player player_other(Player player);

#endif //OTHELLO_PLAYER_H
