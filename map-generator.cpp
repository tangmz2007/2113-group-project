#include <iostream>
using namespace std;
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
const int MAX_PER_LAYER = 4;
const int MAX_LAYERS = 10;

typedef struct{
    int type;
    int layer, index;
    int prev[MAX_PER_LAYER+1];
    int next[MAX_PER_LAYER+1];
    int prev_count;
    int next_count;
} Node;

int randmod(int n){
    return rand() % n;
}

int main() {
    srand((unsigned)time(nullptr));

    Node gameMap[MAX_LAYERS+2][MAX_PER_LAYER];
    int layersizes[MAX_LAYERS+2];

    layersizes[0] = 1; // Start layer has 1 node
    layersizes[MAX_LAYERS+1] = 1; // End layer has 1 node

    // generate 10 random layer sizes (2~5)
    for(int i=1;i<=MAX_LAYERS;i++){
        layersizes[i] = randmod(MAX_PER_LAYER-1) + 2;
    }

    // initialize nodes and defaults
    for(int layer=0; layer<MAX_LAYERS+2; layer++){
        for(int idx=0; idx<MAX_PER_LAYER; idx++){
            gameMap[layer][idx].layer = layer;
            gameMap[layer][idx].index = idx;
            gameMap[layer][idx].prev_count = 0;
            gameMap[layer][idx].next_count = 0;
            gameMap[layer][idx].type = 0;
            for(int k=0; k<MAX_PER_LAYER+1; k++){
                gameMap[layer][idx].prev[k] = -1;
                gameMap[layer][idx].next[k] = -1;
            }
        }
    }

    // assign special types for start/end
    gameMap[0][0].type = -1;
    gameMap[MAX_LAYERS+1][0].type = -2;

    // assign random node types for layers
    for(int layer=1; layer<=MAX_LAYERS; layer++){
        for(int idx=0; idx<layersizes[layer]; idx++){
            gameMap[layer][idx].type = randmod(5); // 0 ???, 1 monster,2 shop,3 campfire,4 chest
        }
    }

    // exception check: layer 1 must all be monster (1)
    if(layersizes[1] > 0){
        for(int idx=0; idx<layersizes[1]; idx++){
            gameMap[1][idx].type = 1;
        }
    }

    // exception check: layer 10 must all be campfire (3)
    if(layersizes[10] > 0){
        for(int idx=0; idx<layersizes[10]; idx++){
            gameMap[10][idx].type = 3;
        }
    }

    // connect layers with non-crossing link policy
    for(int layer=0; layer<=MAX_LAYERS; layer++){
        int n = layersizes[layer];
        int m = layersizes[layer+1];

        for(int i=0; i<n; i++){
            int lo = (i * m) / n;
            int hi = ((i + 1) * m) / n - 1;
            if(hi < lo) hi = lo;
            if(lo >= m) lo = m - 1;
            if(hi >= m) hi = m - 1;

            for(int j=lo; j<=hi; j++){
                // link current -> next
                Node &cur = gameMap[layer][i];
                Node &nxt = gameMap[layer+1][j];

                if(cur.next_count < MAX_PER_LAYER+1) {
                    cur.next[cur.next_count++] = j;
                }
                if(nxt.prev_count < MAX_PER_LAYER+1) {
                    nxt.prev[nxt.prev_count++] = i;
                }
            }
        }

        // ensure each next node has at least one prev (可能在少数情况下漏掉)
        for(int j=0; j<m; j++){
            if(gameMap[layer+1][j].prev_count == 0){
                int best = (j * n) / m;
                if(best >= n) best = n - 1;
                Node &cur = gameMap[layer][best];
                Node &nxt = gameMap[layer+1][j];
                if(cur.next_count < MAX_PER_LAYER+1) cur.next[cur.next_count++] = j;
                if(nxt.prev_count < MAX_PER_LAYER+1) nxt.prev[nxt.prev_count++] = best;
            }
        }
    }

    // Output map structure
    cout << "layer sizes: ";
    for(int i=0; i<MAX_LAYERS+2; i++){
        cout << layersizes[i] << (i<MAX_LAYERS+1 ? "," : "\n");
    }

    for(int layer=0; layer<MAX_LAYERS+2; layer++){
        cout << "Layer " << layer << " (" << layersizes[layer] << " nodes):\n";
        for(int idx=0; idx<layersizes[layer]; idx++){
            Node &n = gameMap[layer][idx];
            cout << "  node["<<layer<<","<<idx<<"] type=" << n.type;
            if(n.type==-1) cout << "(START)";
            else if(n.type==-2) cout << "(END)";
            else if(n.type==1) cout << "(Monster)";
            else if(n.type==2) cout << "(Shop)";
            else if(n.type==3) cout << "(Campfire)";
            else if(n.type==4) cout << "(Chest)";
            else if(n.type==0) cout << "(???)"
            cout << " prev{";
            for(int p=0;p<n.prev_count;p++){
                if(p) cout << ",";
                cout << n.prev[p];
            }
            cout << "} next{";
            for(int q=0;q<n.next_count;q++){
                if(q) cout << ",";
                cout << n.next[q];
            }
            cout << "}";
            cout << "\n";
        }
    }
// an actual map of the game using | \/ \ to connect nodes, for better visualization
    cout << "\nVisual Map:\n";

    const int canvasWidth = 79; // center at 39
    const int centerX = canvasWidth / 2;
    const int lineGap = 4; // 2 intermediate lines between each node layer，完全填满， 不留空行
    const int totalLayers = MAX_LAYERS + 2;
    const int totalRows = (totalLayers - 1) * lineGap + 1;

    vector<string> canvas(totalRows, string(canvasWidth, ' '));

    auto putChar = [&](int x, int y, char ch){
        if(x < 0 || x >= canvasWidth || y < 0 || y >= totalRows) return;
        canvas[y][x] = ch;
    };

    auto nodeLabel = [&](int type)->string{
        if(type == -1) return "(start)";
        if(type == -2) return "(end)";
        if(type == 1) return "(monster)";
        if(type == 2) return "(shop)";
        if(type == 3) return "(campfire)";
        if(type == 4) return "(chest)";
        if(type == 0) return "(???)";
        return "(unknown)";
    };

    int maxLabelWidth = 0;
    for(int layer = 0; layer < totalLayers; layer++){
        for(int i = 0; i < layersizes[layer]; i++){
            int w = (int)nodeLabel(gameMap[layer][i].type).size();
            if(w > maxLabelWidth) maxLabelWidth = w;
        }
    }

    vector<vector<int>> pos(totalLayers, vector<int>(MAX_PER_LAYER, 0));

    for(int layer = 0; layer < totalLayers; layer++){
        int n = layersizes[layer];
        if(n <= 0) continue;
        if(n == 1){
            pos[layer][0] = centerX;
            continue;
        }

        int layerMaxWidth = 0;
        for(int i = 0; i < n; i++){
            int w = (int)nodeLabel(gameMap[layer][i].type).size();
            if(w > layerMaxWidth) layerMaxWidth = w;
        }

        int minSpacing = layerMaxWidth + 4;
        int available = centerX * 2;
        int spacing = minSpacing;
        if((n - 1) * spacing > available) {
            spacing = max(2, available / (n - 1));
        }

        int layerWidth = (n - 1) * spacing;
        int startX = centerX - layerWidth / 2;
        for(int i = 0; i < n; i++){
            pos[layer][i] = startX + i * spacing;
        }
    }

    for(int layer = 0; layer < totalLayers - 1; layer++){
        for(int i = 0; i < layersizes[layer]; i++){
            for(int ni = 0; ni < gameMap[layer][i].next_count; ni++){
                int childIdx = gameMap[layer][i].next[ni];
                if(childIdx < 0 || childIdx >= layersizes[layer+1]) continue;
                int x0 = pos[layer][i];
                int y0 = layer * lineGap;
                int x1 = pos[layer+1][childIdx];
                int y1 = (layer + 1) * lineGap;

                int prevX = x0;
                for(int d = 1; d < lineGap; d++){
                    float t = float(d) / float(lineGap);
                    int curX = int(round(x0 + (x1 - x0) * t));
                    int curY = y0 + d;
                    if(curX == prevX) putChar(curX, curY, '|');
                    else if(curX > prevX) putChar(curX, curY, '\\');
                    else putChar(curX, curY, '/');
                    prevX = curX;
                }
            }
        }
    }

    for(int layer = 0; layer < totalLayers; layer++){
        int y = layer * lineGap;
        for(int i = 0; i < layersizes[layer]; i++){
            string label = nodeLabel(gameMap[layer][i].type);
            int x = pos[layer][i] - (int)label.size() / 2;
            for(int k = 0; k < (int)label.size(); k++){
                putChar(x + k, y, label[k]);
            }
        }
    }

    for(int row = 0; row < totalRows; row++){
        cout << canvas[row] << "\n";
    }

    cout << "\nLegend: start, monster, campfire, chest, shop, ???, end\n";

    return 0;
}
