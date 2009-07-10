#include "monsters.h"
#include "common.h"

void debug(char * s);

// monsters[] maps monster names to their attributes,
// and is used as a template to instantiate monsters.
// this array must be sorted by level (ascending).
// there must be at least one level 1 monster.
monster_t monsters[] = {
    // name     attack   level hp  damage  exp  frequency  description
    {"goblin",  "bite",  1,    2,  1,      4,   1,         "mean spirited goblin"},
    {"troll",   "whack", 1,    3,  1,      5,   1,         "giant troll"},
    {"ghost",   "lick",  1,    4,  1,      6,   1,         "scary ghost"},
    {"pig",		"headbutt",1,  4,  1,      6,   1,         "fat pig"},
    {"donkey",  "kick",  2,    5,  3,      15,  1,         "small brown donkey"},
    {"squirrel","gnaw",  3,    13, 5,      45,  1,         "grotesque, irradiated squirrel"},
    {"NULL",	"",	    0,		0,	0,		0,	0,			""} // not a real monster!!
};

// takes a mob pointer and prints the names of all monsters in the mob
// used for the "look" command
void print_monsters(mob_t *mob) {
    monster_t *monster;
    int num_monsters_here = num_monsters(mob);
    mob_iterator_t *iter = make_mob_iterator(mob);

    printf("There %s %d monster%s here:\n",
            num_monsters_here > 1 ? "are" : "is",
            num_monsters(mob),
            num_monsters_here > 1 ? "s" : ""
            );

    while((monster = next_monster(iter))) {
        printf("\ta %s\n", monster->description);
    }

    delete_mob_iterator(iter);
}

// takes a mob pointer and returns the number of monsters in the mob
int num_monsters(mob_t *mob) {
	//printf("DEBUG: >>>> Number of monsters - %d\n", mob->num);
    return mob->num;
}

// given a mob pointer and a name string, returns the monster pointer
// for the first monster in the mob that matches the given name string,
// or NULL if the name string is not matched in this mob
monster_t *find_monster(mob_t *mob, char *name) {
	debug("find monster!");
	int i;
	mob_node_t *cnode = mob->head;
	debug("pass");
	for(i = 0; i < mob->num; i++){
		debug("in loop");
		if(strcmp(name, cnode->mon->name) == 0){
			debug("found monster");
			monster_t *monster;
			monster = (monster_t *) malloc(sizeof(monster_t));
			monster = cnode->mon;
			debug("return monster");
			return monster;
		} else {
			debug("next monster");
			cnode = cnode->next;
		}
	}
	debug("can not find monster");
	return NULL;
}

// takes a mob pointer and adds a randomly generated monster to the mob.
// the random monster generation must generate monsters with relative
// frequencies proportional to the frequencies listed in monsters[] for each
// type. no monster with level greater than the player's current level should
// be generated.
void spawn_new_monster(mob_t **mob_handle) {
	debug("spawn new monster!");
	monster_t *potential;
	int total = 0;
	int i = 0;
	int m = 0;
	int numberofmonsters = 0;
	while(strcmp(monsters[i].name, "NULL") != 0){
		numberofmonsters++;
		i++;
	}
	i = 0;
	//debug("hello");
	potential = (monster_t *) malloc((sizeof(monster_t)*sizeof(mob_t))*numberofmonsters);
	monster_t *cmonster;
	cmonster = (monster_t *) malloc(sizeof(monster_t));
	while(cmonster = &monsters[i],strcmp(cmonster->name, "NULL") != 0){
		//debug("in the loop");
		if(cmonster->level <= the_player.level){
			//printf("\t %s is less than player's level\n", cmonster->name);
			potential[m] = *cmonster;
			//printf("\t\t %s frequency is %d. total is %d\n", cmonster->name, cmonster->frequency, total);
			total = total + cmonster->frequency;
			//printf("\t\t new total is %d\n", total);
			m++;
		}
		//debug("loop to front");
		i++;
	}
	//debug("storing potentials done");
	//printf(" -------------- total frequencies %d\n",total);
	total = rand() % total;
	//printf("new total after rand() %d\n", total);
	int j,k;
	for(j = 0, k = 0; j < m; j++){
		k = k + potential[j].frequency;
		//printf("print k=%d and freqency=%d\n", k, potential[j].frequency);
		//printf("name of monster - %s\n", potential[j].name);
		if(total < k){
			cmonster = &potential[j];
			//printf("lucky monster %s\n", cmonster->name);
			break;
		}
	}
	//debug("loop to pick monster done");
	append_monster(mob_handle, cmonster);
	//printf("adding monster (%s) to mob\n",cmonster->name);
	//free(cmonster);
	//free(potential);
	//printf("number of monsters in mob %d\n", (*mob_handle)->num);
	//mob_node_t *cnode = (*mob_handle)->head;
	//for(i = 0; i < (*mob_handle)->num; i++){
		//printf("monster number %d is %s\n", (*mob_handle)->num, cnode->mon->name);
	//	cnode = cnode->next;
	//}
	//printf("end of monster list\n");
	//debug("append monster done");
}

// given a mob pointer and a monster pointer, this function adds the monster
// to the mob. the monster can be added at any position in the list.
void append_monster(mob_t **mob_handle, monster_t *monster) {
	debug("appened monster!");
	//printf("current number of monsters in mob = %d\n", (*mob_handle)->num);
	mob_node_t *new;
	new = (mob_node_t *) malloc(sizeof(mob_node_t));
	new->mon = monster;
	new->next = (*mob_handle)->head;
	(*mob_handle)->head = new;
	(*mob_handle)->num = (*mob_handle)->num + 1;
}

// given a mob and a monster pointer, this function deletes the corresponding
// monster from the mob. all memory associated with that monster and any
// overhead memory in the mob should be deallocated. if the monster pointer
// does not exist in the mob, the function does nothing.
void delete_monster(mob_t **mob_handle, monster_t *monster) {
	debug("delete monster");
	int i;
	mob_t *deref = *mob_handle;
	if(deref->head->mon == monster){			// if delete first monster in list
		deref->head = NULL;
		deref->num = deref->num - 1;
	} else {
		mob_node_t *cnode = deref->head;
		mob_node_t *next = cnode->next;
		for(i = 1; i < deref->num; i++){
			if(next->mon == monster){
				if(i == deref->num-1){		// if delete last monster in list
					cnode->next = NULL;
				} else {
					cnode->next = next->next;
				}
				deref->num = deref->num--;
				free(next);
				break;
			} else {
				cnode = next;
				next = next->next;
			}
		}
	}
}

// this function creates and returns a mob iterator object from a mob pointer
mob_iterator_t *make_mob_iterator(mob_t *mob) {
	//debug("make mob iterator!");
	//printf("make mob iterator!");
	mob_iterator_t *iter;
	int i = mob->num;
	iter = (mob_iterator_t *) malloc(i*sizeof(monster_t)+sizeof(mob_iterator_t));
	iter->count = i;
	//printf("number of monsters in the iter mob %d\n", i);
	iter->node = mob->head;
	return iter;
}

// given a mob iterator object, returns the next monster in the iterator
// and changes the iterator's state to advance it to the next monster.
// if the iterator has exhausted the mob, NULL is always returned.
monster_t *next_monster(mob_iterator_t *iter) {
	debug("next monster!");
	//printf("DEBUG:  <<<<<< number of monsters = %d\n", iter->count);
	if(iter->count == 0 || iter->node == NULL){
		//debug("0 monsters, return null");
		return NULL;
	} else {
		debug("n > 0");
		//printf("DEBUG:  <<<<<< name of monsters = %s\n", iter->node->mon->name);
		mob_node_t *cnode;
		//debug("1");
		monster_t *temp;
		//debug("2");
		cnode = iter->node;
		//debug("3");
		temp = iter->node->mon;
		//debug("4");
		iter->node = iter->node->next;
		//debug("5");
		iter->count = iter->count - 1;
		//debug("6");
		//printf("monster name %s", temp->name);
		return temp;
	}
}

// given a mob iterator pointer, this function frees all memory associated with
// the iterator. it does not affect any underlying mob information, so that
// only the iterator is deleted.
void delete_mob_iterator(mob_iterator_t *iter) {
	debug("delete mob iterator");
	free(iter);
}
