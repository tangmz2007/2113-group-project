#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <iostream>
using namespace std;
#include <vector>
#include <cstdlib>
#include <ctime>
#define TEST_GAME_MAP
const int MAX_PER_LAYER = 4;
const int MAX_LAYERS = 10;

typedef struct {
    int type;  // 0 ???, 1 monster,2 shop,3 campfire,4 chest
    int layer, index;
    int prev[MAX_PER_LAYER + 1];
    int next[MAX_PER_LAYER + 1];
    int prev_count;
    int next_count;
} Node;


int randmod(int n);

void initialize(Node gameMap[][MAX_PER_LAYER], int layersizes[]);

void seed_generator();

#endif
