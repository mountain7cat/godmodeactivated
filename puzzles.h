#ifndef PUZZLES_H
#define PUZZLES_H

#include "commands.h"

cmd_fxn_t get_puzzle_description(int idx);
cmd_fxn_t get_puzzle_solver(int idx);

void additional_status(void);

#endif
