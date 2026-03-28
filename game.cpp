#include <iostream>
using namespace std;
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>    // 用于round函数
#include <iomanip>  // 用于格式化输出
using namespace std;
#include "gamemap.h"
#include "map-visual.h"

int main() {

    seed_generator();

    Node gameMap[MAX_LAYERS + 2][MAX_PER_LAYER];
    int layersizes[MAX_LAYERS + 2];

    initialize(gameMap, layersizes);


    

    // // Output map structure
    // cout << "layer sizes: ";
    // for (int i = 0; i < MAX_LAYERS + 2; i++) {
    //     cout << layersizes[i] << (i < MAX_LAYERS + 1 ? "," : "\n");
    // }

    // for (int layer = 0; layer < MAX_LAYERS + 2; layer++) {
    //     cout << "Layer " << layer << " (" << layersizes[layer] << " nodes):\n";
    //     for (int idx = 0; idx < layersizes[layer]; idx++) {
    //         Node& n = gameMap[layer][idx];
    //         cout << "  node[" << layer << "," << idx << "] type=" << n.type;
    //         if (n.type == -1) cout << "(START)";
    //         else if (n.type == -2) cout << "(END)";
    //         else if (n.type == 1) cout << "(Monster)";
    //         else if (n.type == 2) cout << "(Shop)";
    //         else if (n.type == 3) cout << "(Campfire)";
    //         else if (n.type == 4) cout << "(Chest)";
    //         cout << " prev{";
    //         for (int p = 0;p < n.prev_count;p++) {
    //             if (p) cout << ",";
    //             cout << n.prev[p];
    //         }
    //         cout << "} next{";
    //         for (int q = 0;q < n.next_count;q++) {
    //             if (q) cout << ",";
    //             cout << n.next[q];
    //         }
    //         cout << "}";
    //         cout << "\n";
    //     }
    // }

    printMapVisual(gameMap, layersizes);

    return 0;
}
