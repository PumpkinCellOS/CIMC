#pragma once

#include <types.h>

#define RAND_MAX 65535

void exit(u16 rc);
u16 rand(void);
void srand(u16 seed);
