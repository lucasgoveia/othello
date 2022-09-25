//
// Created by govei on 24/09/2022.
//

#include "player.h"

const char PLAYER_DISPLAY[2] = {
        'O',
        'X'
};

Player player_other(Player player) {
    return player ^ 1;
}
