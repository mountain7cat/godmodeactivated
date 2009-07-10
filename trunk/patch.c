#include "patch.h"
#include "common.h"
#include "level.h"


bool get_locked_status(int dir) {
	return the_player.current_room->exits[dir].locked;
}

void set_locked_status(int dir, bool status) {
	the_player.current_room->exits[dir].locked = status;
}

int get_player_level(void) {
	return the_player.level;
}

void reduce_player_hp(int amount) {
	the_player.hp -= amount;
	if (the_player.hp <= 0) {
		printf("That last one was too much for you. You die.");
		exit(0);
	}
}
