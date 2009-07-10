#ifndef PATCH_H
#define PATCH_H

#include <stdbool.h>

// acts on curent room
bool get_locked_status(int dir);

// acts on curent room
void set_locked_status(int dir, bool status);

int get_player_level(void);

// must check if player died
void reduce_player_hp(int amount);

#endif
