#include "gamemap.h"

int randmod(int n) {
    return rand() % n;
}

void initialize(Node gameMap[][MAX_PER_LAYER], int layersizes[])
{
    layersizes[0] = 1; // Start layer has 1 node
    layersizes[MAX_LAYERS + 1] = 1; // End layer has 1 node

    // generate 10 random layer sizes (2~5)
    for (int i = 1;i <= MAX_LAYERS;i++) {
        layersizes[i] = randmod(MAX_PER_LAYER - 1) + 2;
    }

    // initialize nodes and defaults
    for (int layer = 0; layer < MAX_LAYERS + 2; layer++) {
        for (int idx = 0; idx < MAX_PER_LAYER; idx++) {
            gameMap[layer][idx].layer = layer;
            gameMap[layer][idx].index = idx;
            gameMap[layer][idx].prev_count = 0;
            gameMap[layer][idx].next_count = 0;
            gameMap[layer][idx].type = 0;
            for (int k = 0; k < MAX_PER_LAYER + 1; k++) {
                gameMap[layer][idx].prev[k] = -1;
                gameMap[layer][idx].next[k] = -1;
            }
        }
    }

    // assign special types for start/end
    gameMap[0][0].type = -1;  //start
    gameMap[MAX_LAYERS + 1][0].type = -2; //end

    // assign random node types for layers
    for (int layer = 1; layer <= MAX_LAYERS; layer++) {
        for (int idx = 0; idx < layersizes[layer]; idx++) {
            gameMap[layer][idx].type = randmod(4) + 1; // 1 monster,2 shop,3 campfire,4 chest
        }
    }

    // exception check: layer 1 must all be monster (1)
    if (layersizes[1] > 0) {
        for (int idx = 0; idx < layersizes[1]; idx++) {
            gameMap[1][idx].type = 1;
        }
    }

    // exception check: layer 10 must all be campfire (3)
    if (layersizes[10] > 0) {
        for (int idx = 0; idx < layersizes[10]; idx++) {
            gameMap[10][idx].type = 3;
        }
    }

    // connect layers with non-crossing link policy
    for (int layer = 0; layer <= MAX_LAYERS; layer++) {
        int n = layersizes[layer];
        int m = layersizes[layer + 1];

        for (int i = 0; i < n; i++) {
            int lo = (i * m) / n;
            int hi = ((i + 1) * m) / n - 1;
            if (hi < lo) hi = lo;
            if (lo >= m) lo = m - 1;
            if (hi >= m) hi = m - 1;

            for (int j = lo; j <= hi; j++) {
                // link current -> next
                Node& cur = gameMap[layer][i];
                Node& nxt = gameMap[layer + 1][j];

                if (cur.next_count < MAX_PER_LAYER + 1) {
                    cur.next[cur.next_count++] = j;
                }
                if (nxt.prev_count < MAX_PER_LAYER + 1) {
                    nxt.prev[nxt.prev_count++] = i;
                }
            }
        }

        // ensure each next node has at least one prev (可能在少数情况下漏掉)
        for (int j = 0; j < m; j++) {
            if (gameMap[layer + 1][j].prev_count == 0) {
                int best = (j * n) / m;
                if (best >= n) best = n - 1;
                Node& cur = gameMap[layer][best];
                Node& nxt = gameMap[layer + 1][j];
                if (cur.next_count < MAX_PER_LAYER + 1) cur.next[cur.next_count++] = j;
                if (nxt.prev_count < MAX_PER_LAYER + 1) nxt.prev[nxt.prev_count++] = best;
            }
        }



    }
}

void seed_generator()
{
    cout << "Generating game map...\n";
    cout << "press 1 for random seed, 0 for fixed seed: ";
    int seed_choice;
    cin >> seed_choice;
    if (seed_choice == 0) {
        cout << "enter seed value: ";
        unsigned int fixed_seed_value;
        cin >> fixed_seed_value;// fixed seed for reproducibility
        srand(fixed_seed_value);
    }
    else {
        srand((unsigned)time(NULL)); // random seed based on current time
        int rand_seed_value = unsigned(time(NULL));
        cout << "Using random seed: " << rand_seed_value << "\n";
    }
}

