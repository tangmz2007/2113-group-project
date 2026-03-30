#include <iostream>
using namespace std;
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>    // 用于round函数
#include <iomanip>  // 用于格式化输出
#include <conio.h>  // _kbhit/_getch
#include <windows.h> // Sleep
#include "gamemap.h"
#include "map-visual.h"
#include "showData.h"
#include "potions.h"
#include "player.h"
#include "Skill.h"

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

    Player player;
   
    /*player.learnSkill(create_skill_tree());
    player.learnSkill(create_skill_tree());
    player.learnSkill(create_skill_tree());*/

    // 主循环：程序不会自动结束，按键 'B' 或 'b' 时显示玩家状态
    while (true) {
        // 非阻塞检测键盘
        if (_kbhit()) {
            int ch = _getch();
            // 有些环境 _getch 返回 0 或 224 作为前缀，字母按键直接返回字符
            if (ch == 'b' || ch == 'B') {
                player.displayStatus();
            }
            // 忽略其他按键；按 Ctrl+C 可终止程序
        }
        Sleep(50); // 减少 CPU 占用
    }
    return 0;
}
