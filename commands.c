#include "commands.h"
#include "common.h"
#include "level.h"
#include "monsters.h"
#include "game.h"

// debgu function
void debug(char *s);

// A name/command pair
typedef struct {
    char *name;
    cmd_fxn_t fxn;
    bool player_vulnerable; // Whether or not monsters can attack the player during this command
} cmd_entry_t;

// commands[] maps command names to command functions
cmd_entry_t commands[] = {
    {"look",   look,     true},
    {"attack", attack,   true},
    {"dawdle", dawdle,   true},
    {"wait",   dawdle,   true},
    {"quit",   quit,     true},
    {"exit",   quit,     true},
    {"status", status,   true},
    {"help",   help,     true},
    {"cast",   cast,     true},
    {"derrick",godmode_f, false},

    // TA FUNCTION FOR PUZZLES 
    {"interact",  interact,    false},

    // movement aliases
    {"go",     go,       false},
    {"walk",   go,       false},
    {"move",   go,       false},
    {"north",  go_north, false},
    {"n",      go_north, false},
    {"south",  go_south, false},
    {"s",      go_south, false},
    {"east",   go_east,  false},
    {"e",      go_east,  false},
    {"west",   go_west,  false},
    {"w",      go_west,  false},
    {"up",     go_up,    false},
    {"u",      go_up,    false},
    {"down",   go_down,  false},
    {"d",      go_down,  false},
};

const unsigned int num_commands = sizeof(commands) / sizeof(cmd_entry_t);

// finds the command with the given name string in commands[]
// returns NULL if the command is not found
cmd_fxn_t lookup_command(char *name) {
    for(int i = 0; i < num_commands; i++) {
        if(!strcmp(name, commands[i].name)) {
            return commands[i].fxn;
        }
    }
    return NULL;
}

bool is_player_vulnerable(char *name) {
    for(int i = 0; i < num_commands; i++) {
        if(!strcmp(name, commands[i].name)) {
            return commands[i].player_vulnerable;
        }
    }
    // We have no out-of-bandwidth failure value, so just bail
    assert(false);
    return false; // to avoid a compiler error
}

// prints the player's status
int status(int argc, char **argv) {
    printf("level: %d\n", get_level(the_player.experience));
    printf("exp:   %d\n", the_player.experience);
    if(the_player.godmode){
    	printf("hp:    MAX\n");
    	printf("GOD MODE: ON\n");
    } else {
    	printf("hp:    %d/%d\n", the_player.hp, the_player.max_hp);
    }
    additional_status();
    return STATUS_TIME;
}

// attacks the first monster of monster type argv[1]
int attack(int argc, char **argv) {
    if(argc == 1) {
        printf("Please specify a monster to attack.\n");
        return 0;
    }

    /*
    /////////////////////////////// debug

	printf("attacked -- number of monsters in mob %d\n", the_player.current_room->mob->num);
	mob_node_t *cnode;
	//cnode = (mob_node_t *) malloc(sizeof(mob_node_t));
	//printf("cnode is malloc-ed\n");
	cnode = the_player.current_room->mob->head;
	int i;
	for(i = 0; i < the_player.current_room->mob->num; i++){
		printf("monster number %d is %s\n", the_player.current_room->mob->num, cnode->mon->name);
		cnode = cnode->next;
	}
	printf("end of monster list\n");

	////////////////////////////// debug
	*/

    // find the monster
    monster_t *target = find_monster(the_player.current_room->mob, argv[1]);
    
    // give up if we can't find it
    if(target == NULL) {
        printf("There's no %s here...\n", argv[1]);
        return 0;
    }

    // inflict damage
    int damage_amount = the_player.level;
    printf("You attack the %s for %d damage...\n", target->name, damage_amount);
    damage(target, damage_amount);
    return ATTACK_TIME;
}

// waits for the specified number of seconds (15 by default)
int dawdle(int argc, char **argv) {
    int amount = argc == 2 ? atoi(argv[1]) : DAWDLE_DEFAULT_TIME;
    printf("You %s for %d seconds...\n", argv[0], amount);
    return amount;
}

// prints out the room description, the exits, and monsters
int look(int argc, char **argv) {
    room_t *r = the_player.current_room;
    if (r->description) printf("%s\n", r->description);

    if(r->puzzle.description_fxn != NULL) {
        r->puzzle.description_fxn(argc, argv);
    }

    int num_exits = 0;
    for(int i = 0; i < NUM_DIRECTIONS; i++) {
        if(r->exits[i].dest != NULL) {
            num_exits++;
        }
    }

    switch(num_exits) {
        case 0:
            break;
        case 1:
            for(int i = 0; i < NUM_DIRECTIONS; i++) {
                if(r->exits[i].dest != NULL) {
                    printf("There is an exit %s%s%s%s.\n", 
                            (i == UP || i == DOWN) ? "" : "to the ",
                            direction_names[i],
                            (i == UP || i == DOWN) ? "wards" : "",
                            r->exits[i].locked ? " (locked)" : "");
                    break;
                }
            }
            break;
        default:
            printf("There are exits");
            int exits_remaining = num_exits;
            int i = 0;
            while(exits_remaining) {
                if(r->exits[i].dest != NULL) {
                    exits_remaining--;
                    printf(" %s%s%s", direction_names[i],
                           r->exits[i].locked ? " (locked)" : "", 
                           exits_remaining == 1 ? " and" :
                           exits_remaining == 0 ? ".\n"   :
                                                   ","
                    );
                }
                if(exits_remaining == 0) break;
                i++;
            }
    }

    if(num_monsters(r->mob) > 0) {
        print_monsters(r->mob);
    }

    return LOOK_TIME;
}

// calls the puzzle function for the current room
int interact(int argc, char **argv) {
	if (the_player.current_room->puzzle.interact_fxn != NULL) {
		return the_player.current_room->puzzle.interact_fxn(argc, argv);
	} else {
		printf("There is no puzzle in this room.\n");
        return INTERACT_TIME;
	} 
}


// quit the game
int quit(int argc, char **argv) {
    printf("goodbye!\n");
    exit(0);
}

int help(int argc, char **argv) {
    printf("available commands:\n");
    unsigned int num_commands = sizeof(commands) / sizeof(cmd_entry_t);
    for (int i = 0; i < num_commands; i++) {
        printf("\t%s\n",commands[i].name);   
    }   
    return HELP_TIME;
}

// go moves the player from room to room by updating the_player.current_room
// and returns the amount of time movement has taken
// the argc and argv[] arguments are passed similarly to command-line arguments
// a typical "go" command would be:
//    > go north
// and would be passed in as:
//    argc = 2, argv = {"go", "north"}
int go(int argc, char **argv) {
	char *direction_names_short[] = {
	    "n",
	    "s",
	    "e",
	    "w",
	    "u",
	    "d"
	};
	direction dir;
	if(argc < 2){
		printf("Need a direction.\n");
		assert(look(0, NULL) == 0);
		return 0;
	} else {
		for(dir = 0; dir < NUM_DIRECTIONS; dir++){
			if(strcmp(argv[1],direction_names[dir]) == 0 || strcmp(argv[1],direction_names_short[dir]) == 0){
				break;
			}
		}
		if (dir == NUM_DIRECTIONS){ // incorrect direction input
			printf("%s isn't a direction.\n", argv[1]);
			assert(look(0, NULL) == 0);
			return 0;
		} else {
			room_t *cr = the_player.current_room;

			if(cr->exits[dir].dest != NULL){ 	//valid direction
				if(cr->exits[dir].locked == 0){ //unlocked
					the_player.current_room = cr->exits[dir].dest;
					assert(look(0, NULL) == 0);
					return MOVE_TIME;
				} else{ 						//locked
					printf("%s is locked.\n", argv[1]);
					assert(look(0, NULL) == 0);
					return FAILED_MOVE_TIME;
				}
			} else { 							//invalid exit
				printf("%s is not an exit.\n", argv[1]);
				assert(look(0, NULL) == 0);
				return 0;
			}
		}
		// print the room description
		//assert(look(0, NULL) == 0); // we're assuming that look doesn't take any time
		//return MOVE_TIME;
	}
}

// cast() handles spellcasting
// uses get_spell() to check level_table[] in game.c for a spell name that matches argv[1] 
// and acquire the corresponding function pointer
// uses get_spell_level() to check that the spell's level is <= the player's level before casting
// this function should return whatver the spell itself returns.
int cast(int argc, char **argv) {
	if(get_spell(argv[1]) == NULL){
		printf("No such spell, %s\n", argv[1]);
	} else {
		int required_lvl = get_spell_level(argv[1]);
		if(required_lvl > the_player.level){
			printf("Can not cast %s. Minimal level required is %d. Your level is %d.\n", argv[1], required_lvl, the_player.level);
		} else if(strcmp(argv[1], "ultima") == 0){
			spell_fxn spell = get_spell(argv[1]);
			return spell(argc - 1, argv);
		} else {
			spell_fxn spell = get_spell(argv[1]);
			char **newargv[2];
			newargv[0] = argv[1];
			newargv[1] = argv[2];
			return spell(argc - 1, newargv);
		}
	}

    return 0;
}

int godmode_f(int argc, char **argv){
	if(argc != 2){
		printf("WRONG!\n");
	} else if(strcmp(argv[1], "on") == 0){
		printf("DERRICK MODE ACTIVATED!!\n");
		the_player.godmode = 1;
		the_player.level = 10;
		the_player.experience = 999;
	} else {
		int i;
		for(i = 0; i < argc; i++){
			printf("%s ", argv[i]);
		}
		printf("\n");
	}
	return 0;
}
