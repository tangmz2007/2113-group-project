#include "map-visual.h"
#include <iostream>
#include <cmath>

using namespace std;

void printMapVisual(Node gameMap[MAX_LAYERS+2][MAX_PER_LAYER], const int layersizes[MAX_LAYERS+2]) {
    cout << "\nVisual Map:\n";

    const int canvasWidth = 79;
    const int centerX = canvasWidth / 2;
    const int lineGap = 4;
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
}