#include "level.h"
#include "common.h"
#include "util.h"

// array of direction names. must be consistent with the order of the enum
// in level.h
char *direction_names[] = {
    "north",
    "south",
    "east",
    "west",
    "up",
    "down"
};

// loads a level from a config file and returns a pointer to the starting room
room_t *load_level(char *filename) {
    char buf[2048];
    char *whitespace = " \t\n";
    FILE *levelfile = fopen(filename,"r");

    if(levelfile == NULL) {
        printf("Could not open %s\n", filename);
        exit(1);
    }

    skip_characters(levelfile, whitespace);
    
    // get the total number of rooms
    fgets(buf,256,levelfile);
    num_rooms = atoi(buf);


    skip_characters(levelfile, whitespace);

    // allocate an array for all of the room structs to be stored
	// store the pointer in the global room_array variable
	
	/*** YOUR CODE HERE ***/

    // Initialize room_array

    room_array = (room_t *) malloc(sizeof(room_t) * num_rooms);
    int i,j;
    for(i = 0; i < num_rooms; i++){
    	room_array[i].room_id = i;
    	room_array[i].description = NULL;
    	// Initialize exit array
    	for(j = 0; j < NUM_DIRECTIONS; j++){
    		room_array[i].exits[j].dest = NULL;
    		room_array[i].exits[j].locked = 0;
    	}
    	room_array[i].puzzle.description_fxn = NULL;
    	room_array[i].puzzle.interact_fxn = NULL;
    	// Initialize mob_t
    	mob_t *initialmob;
    	initialmob = (mob_t *) malloc(sizeof(mob_t));
    	initialmob->num = 0;
    	mob_node_t *initialhead;
    	initialhead = (mob_node_t *) malloc(sizeof(mob_node_t));
    	initialmob->head = initialhead;
    	room_array[i].mob = initialmob;
    }

	/*** YOUR CODE HERE ***/

    skip_characters(levelfile, whitespace);

	// one line at a time, read in room description strings and set 
	// the appropriate field in each string's corresponding room struct

    char *endptr;
    char *descrip;
    char c;
    int len;
    fgets(buf, 256, levelfile);
    while(strcmp(buf,"\n") != 0) {
    	len = strlen(buf);
    	descrip = (char *) malloc(sizeof(char) * len);

    	i = (int) strtol(buf, &endptr, 10);

    	while(*endptr == ' ' || *endptr == '\t'){
    		endptr++;
    	}
    	j = 0;
    	c = endptr[j];
    	while(c != '\n'){
    		c = endptr[j];
    		j++;
    	}
    	endptr[j-1] = '\0';

    	strcpy(descrip, endptr);
    	room_array[i].description = descrip;
    	fgets(buf, 256, levelfile);
    }

    skip_characters(levelfile, whitespace);

	// hook up rooms so that exits point to each other appropriately.
	// exits should also be set to locked or unlocked as necessary

    while(fgets(buf, 256, levelfile), buf[0] != '\n' && !feof(levelfile)) {
        char *words[32];
        tokenizer(buf, words, " \t\n");
        
		assert(!strcmp("can-go", words[0]) || !strcmp("cant-go", words[0]));
        
		direction dir;
        switch(words[1][0]) {
            case 'n':
                dir = NORTH;
                break;
            case 's':
                dir = SOUTH;
                break;
            case 'e':
                dir = EAST;
                break;
            case 'w':
                dir = WEST;
                break;
            case 'u':
                dir = UP;
                break;
            case 'd':
                dir = DOWN;
                break;
            default:
                printf("%s isn't a direction\n", words[1]);
                assert(false);
        }
		
        if(words[0][3] == 't'){
        	room_array[atoi(words[2])].exits[dir].locked = 1;
        }
        room_array[atoi(words[2])].exits[dir].dest = room_array + atoi(words[3]);
        room_array[atoi(words[3])].exits[flip_direction(dir)].dest = room_array + atoi(words[2]);
    }


    return room_array; // equivalent to a pointer to the first element of the array
}

