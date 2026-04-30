#include "map.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <random>
#include <ctime>
using namespace std;

void seed_generator(unsigned int& seed) {//allows user to choose a random seed or input a fixed seed.
    //outputs the seed being used for the run
    cout << "Generating game map...\n";
    cout << "press 1 for random seed, 0 for fixed seed: " << flush;
    int seed_choice;
    cin >> seed_choice;
    if (seed_choice == 0) {
        cout << "enter seed value: " << flush;
        cin >> seed;
    } else {
        seed = (unsigned int)time(NULL);
        cout << "Using random seed: " << seed << "\n";
    }
    srand(seed);
}

NodeType GameMap::randomNormalType(int layer) {
    int r = randint(1, 100);
    if (layer <= 2) {
        if (r <= 52) return NodeType::Monster;
        if (r <= 67) return NodeType::Shop;
        if (r <= 82) return NodeType::Campfire;
        if (r <= 100) return NodeType::Event;
          // no chests in the first 3 layers, no elites in the first 3 layers
    }
    if (r <= 45) return NodeType::Monster;
    if (r <= 58) return NodeType::Shop;
    if (r <= 72) return NodeType::Campfire;
    if (r <= 84) return NodeType::Event;
    if (r <= 94) return NodeType::Chest;
    return NodeType::Elite;
}

string GameMap::nodeDisplay(const Node& n, bool cur) const {//returns the string to display for a node, based on its type and whether it's the current node or visited before
    //input a node and a boolean indicating whether it's the current node, output the string to display for the node
    auto fullName = [&](NodeType t) -> string {
        switch (t) {
            case NodeType::Start: return "start";
            case NodeType::Monster: return "monster";
            case NodeType::Elite: return "elite";
            case NodeType::Shop: return "shop";
            case NodeType::Campfire: return "campfire";
            case NodeType::Chest: return "chest";
            case NodeType::Event: return "???";
            case NodeType::Boss: return "boss";
        }
        return "unknown";
    };

    string base;
    if (n.hiddenAsQuestion && !n.revealed) base = "???";
    else base = fullName(n.actualType);

    if (cur) return "[@" + base + "]";//@ indicates current node
    if (n.visited) return "(" + base + "*)"; //* indicates visited before
    return "(" + base + ")";
}

void GameMap::putText(vector<string>& canvas, int y, int x, const string& s) const {
    //puts the string s on the canvas at position (y, x), centered around x. If y is out of bounds, does nothing. If the string exceeds canvas width, it will be truncated.
    //input a canvas, a position (y, x) and a string s, output the canvas with the string s put on it at the given position. The string will be centered around x, and truncated if it exceeds canvas width. If y is out of bounds, the canvas is returned unchanged.
    if (y < 0 || y >= (int)canvas.size()) return;
    int start = max(0, x - (int)s.size() / 2);
    for (int i = 0; i < (int)s.size() && start + i < (int)canvas[y].size(); ++i) {
        canvas[y][start + i] = s[i];
    }
}

void GameMap::drawPath(vector<string>& canvas, int y1, int x1, int y2, int x2) const {
    //draws a path on the canvas from (y1, x1) to (y2, x2) using ASCII characters. The path will be drawn vertically first, then horizontally. If the path goes out of bounds, it will be truncated.
    //input a canvas and two positions (y1, x1) and (y2, x2), output the canvas with a path drawn from (y1, x1) to (y2, x2). The path will be drawn vertically first, then horizontally. If the path goes out of bounds, it will be truncated.
    if (y1 == y2) return;
    int prevX = x1;
    for (int y = y1 + 1; y < y2; ++y) {
        if (y < 0 || y >= (int)canvas.size()) continue;
        float t = float(y - y1) / float(y2 - y1);
        int curX = int(round(x1 + (x2 - x1) * t));
        if (curX < 0 || curX >= CANVAS_W) continue;
        char ch;
        if (curX == prevX) ch = '|';
        else if (curX > prevX) ch = '\\';
        else ch = '/';
        if (canvas[y][curX] == ' ') canvas[y][curX] = ch;
        prevX = curX;
    }
}

int GameMap::preferredTarget(int curIndex, int curCount, int nextCount) const {
    //returns the preferred target index in the next layer for a node at index curIndex in the current layer, based on a linear mapping of the indices. 
    //input the current node index, current layer count and next layer count, output the preferred target index in the next layer
    if (curCount == 1) return nextCount / 2;
    double ratio = (double)curIndex / (curCount - 1);
    int mapped = (int)round(ratio * (nextCount - 1));
    return max(0, min(nextCount - 1, mapped));
}

//the following functions are mostly for map visualization
void GameMap::buildPositions() {
    //builds the posX and rowY vectors based on the number of layers and the number of nodes in each layer. 
    int L = (int)layers.size();
    posX.assign(L, {});
    rowY.assign(L, 0);
    for (int l = 0; l < L; ++l) {
        int cnt = (int)layers[l].size();
        posX[l].resize(cnt);
        rowY[l] = l * LAYER_GAP;
        int totalWidth = cnt * CELL_W;
        int left = (CANVAS_W - totalWidth) / 2 + CELL_W / 2;
        for (int i = 0; i < cnt; ++i) posX[l][i] = left + i * CELL_W;
    }
}

void GameMap::generateEdges() {
    //generates the edges between nodes in consecutive layers based on the preferred target and some randomness. 
    int L = (int)layers.size();
    for (int l = 0; l < L - 1; ++l) {
        int curCount = (int)layers[l].size();
        int nextCount = (int)layers[l + 1].size();
        for (int i = 0; i < curCount; ++i) {
            auto& node = layers[l][i];
            node.next.clear();
            int center = preferredTarget(i, curCount, nextCount);
            node.next.push_back(center);
            if (nextCount > 1 && randint(1, 100) <= 45) {
                vector<int> cand;
                if (center - 1 >= 0) cand.push_back(center - 1);
                if (center + 1 < nextCount) cand.push_back(center + 1);
                std::random_device rd;
                std::mt19937 rng(rd());
                shuffle(cand.begin(), cand.end(), rng);
                if (!cand.empty()) node.next.push_back(cand[0]);
            }
            sort(node.next.begin(), node.next.end());
            node.next.erase(unique(node.next.begin(), node.next.end()), node.next.end());
        }

        vector<int> indeg(nextCount, 0);
        for (int i = 0; i < curCount; ++i) for (int v : layers[l][i].next) indeg[v]++;
        for (int j = 0; j < nextCount; ++j) {
            if (indeg[j] > 0) continue;
            int bestI = 0;
            int bestDist = 1e9;
            for (int i = 0; i < curCount; ++i) {
                int d = abs(preferredTarget(i, curCount, nextCount) - j);
                if (d < bestDist) {
                    bestDist = d;
                    bestI = i;
                }
            }
            layers[l][bestI].next.push_back(j);
            sort(layers[l][bestI].next.begin(), layers[l][bestI].next.end());
            layers[l][bestI].next.erase(unique(layers[l][bestI].next.begin(), layers[l][bestI].next.end()), layers[l][bestI].next.end());
        }
    }
}

GameMap::GameMap() : GameMap((unsigned int)time(NULL)) {}//default constructor, generates a random map based on the current time as seed

GameMap::GameMap(unsigned int seed) {
    //inputs a seed for random generation, outputs a generated map with layers and edges based on the seed.
    srand(seed);
    vector<int> shape = {1, 2, 4, 6, 7, 6, 4, 3, 2, 1};//the number of nodes in each layer, can be adjusted for different map shapes
    int L = (int)shape.size();
    layers.resize(L);
    for (int l = 0; l < L; ++l) {
        layers[l].resize(shape[l]);
        for (int i = 0; i < shape[l]; ++i) {
            layers[l][i].layer = l;
            layers[l][i].index = i;
            if (l == 0) layers[l][i].actualType = NodeType::Start;
            else if (l == 4 && i == shape[l] / 2) layers[l][i].actualType = NodeType::Chest;
            else if (l == L - 1) layers[l][i].actualType = NodeType::Boss;
            else layers[l][i].actualType = randomNormalType(l);
            layers[l][i].hiddenAsQuestion = !(layers[l][i].actualType == NodeType::Start || layers[l][i].actualType == NodeType::Boss) && randint(1, 100) <= 28;
        }
    }
    buildPositions();
    generateEdges();
}

void GameMap::revealNode(int l, int i) {
    //inputs a node position (l, i), outputs the map with the node at (l, i) revealed and marked as visited. If the position is out of bounds, does nothing.
    if (l < 0 || l >= (int)layers.size() || i < 0 || i >= (int)layers[l].size()) return;
    layers[l][i].revealed = true;
    layers[l][i].visited = true;
}

const Node& GameMap::getNode(int l, int i) const { return layers[l][i]; }
vector<int> GameMap::getNextNodes(int l, int i) const { return layers[l][i].next; }
int GameMap::layerCount() const { return (int)layers.size(); }
int GameMap::rowSize(int l) const { return (int)layers[l].size(); }

void GameMap::printMap(int curL, int curI) const {
    //inputs the current node position (curL, curI), outputs a visualization of the map with the current node marked and revealed nodes shown. 
    int H = rowY.back() + 2;
    vector<string> canvas(H + 1, string(CANVAS_W, ' '));
    for (int l = 0; l < (int)layers.size() - 1; ++l) {
        for (int i = 0; i < (int)layers[l].size(); ++i) {
            int x1 = posX[l][i], y1 = rowY[l] + 1;
            for (int ni : layers[l][i].next) {
                int x2 = posX[l + 1][ni], y2 = rowY[l + 1] - 1;
                drawPath(canvas, y1, x1, y2, x2);
            }
        }
    }
    for (int l = 0; l < (int)layers.size(); ++l) {
        for (int i = 0; i < (int)layers[l].size(); ++i) {
            putText(canvas, rowY[l], posX[l][i], nodeDisplay(layers[l][i], l == curL && i == curI));
        }
    }
    cout << "\n===== MAP =====\n";
    for (const auto& row : canvas) {
        size_t end = row.find_last_not_of(' ');
        if (end != string::npos) cout << row.substr(0, end + 1) << "\n";
    }
    cout << "Legend: [@X]=Current [X*]=Visited [?]=Unknown\n";
    cout << "Types: ST Start, M Monster, E Elite, S Shop, C Campfire, CH Chest, ? Event, BO Boss\n";
}

bool GameMap::save(std::ofstream& out) const {
    //saves the map to a file in a simple text format. 
    //inputs an output file stream, outputs the map data written to the file in a format that can be loaded later.
    out << layerCount() << "\n";
    for (int l = 0; l < layerCount(); ++l) {
        out << rowSize(l) << "\n";
        for (int i = 0; i < rowSize(l); ++i) {
            const Node& n = layers[l][i];
            out << n.layer << ' ' << n.index << ' ' << (int)n.actualType << ' '
                << n.hiddenAsQuestion << ' ' << n.revealed << ' ' << n.visited << ' '
                << joinVec(n.next) << "\n";
        }
    }
    return true;
}

bool GameMap::load(std::ifstream& in) {
    //loads the map from a file in the format written by the save function.
    //inputs an input file stream, outputs the map data read from the file.
    int L;
    if (!(in >> L)) return false;
    string dummy;
    getline(in, dummy);
    layers.assign(L, {});
    for (int l = 0; l < L; ++l) {
        int cnt;
        if (!(in >> cnt)) return false;
        getline(in, dummy);
        layers[l].resize(cnt);
        for (int i = 0; i < cnt; ++i) {
            string line;
            getline(in, line);
            if (line.empty()) { --i; continue; }
            stringstream ss(line);
            Node n;
            int typeInt;
            if (!(ss >> n.layer >> n.index >> typeInt >> n.hiddenAsQuestion >> n.revealed >> n.visited)) return false;
            string nextStr;
            getline(ss >> std::ws, nextStr);
            n.actualType = (NodeType)typeInt;
            n.next = splitInts(nextStr);
            layers[l][i] = n;
        }
    }
    buildPositions();
    return true;
}

