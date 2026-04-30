#ifndef MAP_H
#define MAP_H

#include "types.h"
#include <vector>
#include <fstream>
using namespace std;
class GameMap {
//the class representing the game map, which consists of multiple layers of nodes. 
//Each node can be a monster, shop, campfire, chest, event, elite or boss. The map also handles the visualization and saving/loading of the map state.
private:
    vector<vector<Node>> layers;
    vector<vector<int>> posX;
    vector<int> rowY;

    static constexpr int CANVAS_W = 120;
    static constexpr int CELL_W = 10;
    static constexpr int LAYER_GAP = 4;

    std::string nodeDisplay(const Node& n, bool cur) const;
    void putText(vector<string>& canvas, int y, int x, const string& s) const;
    void drawPath(vector<string>& canvas, int y1, int x1, int y2, int x2) const;
    int preferredTarget(int curIndex, int curCount, int nextCount) const;
    void buildPositions();
    void generateEdges();

public:
    GameMap();
    GameMap(unsigned int seed);
    
    void revealNode(int l, int i);
    const Node& getNode(int l, int i) const;
    vector<int> getNextNodes(int l, int i) const;
    int layerCount() const;
    int rowSize(int l) const;
    void printMap(int curL, int curI) const;
    
    bool save(std::ofstream& out) const;
    bool load(std::ifstream& in);
    
    NodeType randomNormalType(int layer);
};

void seed_generator(unsigned int& seed);

#endif
